unit _AcThrds;

interface

uses
  Classes, AcsProto, Windows, ZComms, SysUtils, SyncObjs;

const
  ERR_OK             = 0;
  ERR_MAXCONNECTS    = 1;
  ERR_OPENPORT       = 2;
  ERR_PORTCFG        = 3;
  ERR_REFUSED        = 4;
  ERR_INVALIDCONNECT = 5;
  ERR_NOTCONNECTED   = 6;
  ERR_WRITE          = 7;
  ERR_READ           = 8;
  ERR_TSMR           = 9;
  ERR_MEMORY         = 10;
  ERR_UNKNOWN        = 11;

function MicexConnect(Port: PChar; BaudRate: Integer;
  var Idx: Integer): Integer; stdcall;
function MicexDisconnect(Idx: Integer): Integer; stdcall;
function MicexStructure(Idx: Integer; var Data: PChar): Integer; stdcall;
function MicexExecTrans(Idx: Integer; TransName, Params: PChar;
  var ResultMsg: PChar): Integer; stdcall;
function MicexOpenTable(Idx: Integer; TableName, Params: PChar;
  var Data: PChar): Integer; stdcall;
function MicexRefreshTable(Idx: Integer; TableName, Cookie: PChar;
  var Data: PChar): Integer; stdcall;
function MicexErrorMsg(ErrCode: Integer): PChar;

implementation

uses
  ExtUtils, GZLib;

type
  TMicexConnection = record
    FUsed: Boolean;
    FConnected: Boolean;
    FStream: TCommStream;
    FOutBuf: TOutStream;
    FHeader: TPacketHdr;
    FInBuf: PChar;
    FBufSize: Integer;
    FErrorMsg: array [0..255] of Char;
  end;

const
  MAXCONNECTS  = 20;

  SErrors: array [ERR_OK..ERR_UNKNOWN] of PChar =
    ('Нет ошибок.',
     'Установлено максимальное число соединений.',
     'Невозможно открыть заданный порт.',
     'Ошибка при настройке порта.',
     'Запрос на соединение отвергнут.',
     'Неверный номер соединения.',
     'Соединение не установлено.',
     'Торговый сервер не доступен.',
     'Ошибка чтения из порта.',
     'Торговый сервер вернул код ошибки.',
     'Не хватает памяти.',
     'Неизвестная ошибка.');

var
  ConnectGuard: TCriticalSection;
  Connects: array [0..MAXCONNECTS - 1] of TMicexConnection;

{ Micex.dll }

function SendPacket(Idx: Integer; MsgType: Byte): Integer;
var
  MsgLen, UncLen, MaxLen, OutBytes: Integer;
  OutBuf: Pointer;
begin
  Result := ERR_OK;
  with Connects[Idx] do
  try
    MsgLen := FOutBuf.Position;
    FHeader.MsgLen := EBSwap(MsgLen - SizeOf(FHeader));
    FHeader.UncLen := FHeader.MsgLen;
    FHeader.MsgType := MsgType;
    Move(FHeader, FOutBuf.Memory^, SizeOf(FHeader));
    if FStream.Write(FOutBuf.Memory^, MsgLen) = MsgLen then
    begin
      FOutBuf.Seek(SizeOf(FHeader), soFromBeginning);

      if FStream.Read(FHeader, SizeOf(FHeader)) = SizeOf(FHeader) then
      begin
        MsgLen := EBSwap(FHeader.MsgLen);
        UncLen := EBSwap(FHeader.UncLen);
        MaxLen := EMax(MsgLen, UncLen);
        if MaxLen > FBufSize then
        begin
          ReallocMem(FInBuf, MaxLen);
          FBufSize := MaxLen;
        end;
        if FStream.Read(FInBuf^, MsgLen) = MsgLen then
        begin
          if UncLen > MsgLen then        // Need to decompress
          begin
            DecompressBuf(FInBuf, MsgLen, UncLen, OutBuf, OutBytes);
            try
              Move(OutBuf^, FInBuf^, UncLen);
            finally
              FreeMem(OutBuf);
            end;
          end;
        end
        else Result := ERR_READ;
      end
      else Result := ERR_READ
    end
    else Result := ERR_WRITE;
  except
    on EOutOfMemory do Result := ERR_MEMORY
    else Result := ERR_UNKNOWN;
  end;
  if Result <> ERR_OK then Connects[Idx].FConnected := False;
end;

function CheckConnect(Idx: Integer): Integer;
begin
  if (Idx < 0) or (Idx >= MAXCONNECTS) or
    (not Connects[Idx].FUsed) then Result := ERR_INVALIDCONNECT
  else
    if not Connects[Idx].FConnected then Result := ERR_NOTCONNECTED
    else Result := ERR_OK;
end;

procedure FreeConnect(Idx: Integer);
begin
  ConnectGuard.Enter;
  try
    with Connects[Idx] do
    begin
      FStream.Free;
      FStream := nil;
      FOutBuf.Free;
      FOutBuf := nil;
      FreeMem(FInBuf);
      FInBuf := nil;
      FUsed := False;
      FBufSize := 0;
    end;
  finally
    ConnectGuard.Leave;
  end;
end;

function FindUnusedConnect(var Idx: Integer): Boolean;
var
  I: Integer;
begin
  ConnectGuard.Enter;
  try
    Result := False;
    for I := 0 to MAXCONNECTS - 1 do
      if not Connects[I].FUsed then
      begin
        Idx := I;
        Connects[I].FUsed := True;
        Result := True;
        Break;
      end;
  finally
    ConnectGuard.Leave;
  end;
end;

function MicexConnect(Port: PChar; BaudRate: Integer;
  var Idx: Integer): Integer;
begin
  if FindUnusedConnect(Idx) then
    with Connects[Idx] do
  begin
    try
      FOutBuf := TOutStream.Create;
      FOutBuf.Write(FHeader, SizeOf(FHeader));
      FStream := TCommStream.Create(Port, BaudRate);
      FStream.WriteTimeout := 1000;
      if FStream.Connect then
      begin
        Result := ERR_OK;
        FConnected := True;
      end
      else Result := ERR_REFUSED;
    except
      on ECommOpenError do Result := ERR_OPENPORT;
      on ECommConfigError do Result := ERR_PORTCFG;
      else Result := ERR_UNKNOWN;
    end;
    if Result <> ERR_OK then FreeConnect(Idx);
  end
  else
    Result := ERR_MAXCONNECTS;
end;

function MicexDisconnect(Idx: Integer): Integer;
begin
  if (Idx < 0) or (Idx >= MAXCONNECTS) or
    (not Connects[Idx].FUsed) then Result := ERR_INVALIDCONNECT
  else
  begin
    FreeConnect(Idx);
    Result := ERR_OK;
  end;
end;

function MicexStructure(Idx: Integer; var Data: PChar): Integer;
begin
  Result := CheckConnect(Idx);
  if Result = ERR_OK then
  begin
    Result := SendPacket(Idx, MT_STRUCTURE);
    if Result = ERR_OK then
      with Connects[Idx] do
    begin
      Data := FInBuf;
      if FHeader.MsgType <> MT_OK then Result := ERR_TSMR;
    end;
  end;
end;

function MicexExecute(Idx, MsgType: Integer; InParam1, InParam2: PChar;
  var OutParam: PChar): Integer;
begin
  Result := CheckConnect(Idx);
  if Result = ERR_OK then
    with Connects[Idx] do
  begin
    FOutBuf.WriteStr(InParam1);
    FOutBuf.WriteStr(InParam2);
    Result := SendPacket(Idx, MsgType);
    if (Result = ERR_OK) then
    begin
      if (FHeader.MsgType <> MT_OK) then Result := ERR_TSMR;
      OutParam := FInBuf;
    end;
  end;
end;

function MicexExecTrans(Idx: Integer; TransName, Params: PChar;
  var ResultMsg: PChar): Integer;
begin
  Result := MicexExecute(Idx, MT_EXECTRANS, TransName, Params, ResultMsg);
end;

procedure InitConnects;
begin
  ConnectGuard := TCriticalSection.Create;
  FillChar(Connects, SizeOf(Connects), 0);
end;

procedure DoneConnects;
var
  Idx: Integer;
begin
  for Idx := 0 to MAXCONNECTS - 1 do
    if Connects[Idx].FUsed then FreeConnect(Idx);
  ConnectGuard.Free;
end;

function MicexErrorMsg(ErrCode: Integer): PChar;
begin
  if (ErrCode >= ERR_OK) and (ErrCode <= ERR_UNKNOWN) then
    Result := SErrors[ErrCode] else Result := SErrors[ERR_UNKNOWN];
end;

function MicexOpenTable(Idx: Integer; TableName, Params: PChar;
  var Data: PChar): Integer;
begin
  Result := MicexExecute(Idx, MT_OPENTABLE, TableName, Params, Data);
end;

function MicexRefreshTable(Idx: Integer; TableName, Cookie: PChar;
  var Data: PChar): Integer;
begin
  Result := MicexExecute(Idx, MT_REFRESHTABLE, TableName, Cookie, Data);
end;

initialization

  InitConnects;

finalization

  DoneConnects;

end.
