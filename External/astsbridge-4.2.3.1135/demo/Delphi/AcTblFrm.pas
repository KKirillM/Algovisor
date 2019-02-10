unit AcTblFrm;

{$IFDEF VER120}
   {$DEFINE Post4}
{$ENDIF}
{$IFDEF VER130}
   {$DEFINE Post4}
{$ENDIF}
{$IFDEF VER140}
   {$DEFINE Post4}
{$ENDIF}
{$IFDEF VER150}
   {$DEFINE Post4}
{$ENDIF}

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  Grids, StdCtrls, ExtCtrls, AcStruc, MTEApi, mteTypes, ComCtrls, CommCtrl,
  acBase, Menus;

type
  PSecProp = ^TSecProp;
  TSecProp = record
    SecCode: string;
    ShortName: string;
    Decimals: Integer;
  end;

  PFirmProp = ^TFirmProp;
  TFirmProp = record
    ShortName: string;
  end;

  PUserProp = ^TUserProp;
  TUserProp = record
    UserName: string;
    FirmId: string;
    FirmName: string;
  end;

  TIdViewMode = (ivmIds, ivmNames, ivmAll);

  TSecProps = class(TStringList)
  private
    function GetProps(AIndex: Integer): PSecProp;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Clear; override;
    procedure Add(const ASecCode, AShortName: string; ADecimals: Integer); reintroduce;
    property Props[AIndex: Integer]: PSecProp read GetProps; default;
  end;

  TFirmProps = class(TStringList)
  private
    function GetProps(AIndex: Integer): PFirmProp;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Clear; override;
    property Props[AIndex: Integer]: PFirmProp read GetProps; default;
    procedure Add(const AFirmId, AFirmName: string); reintroduce;
  end;

  TUserProps = class(TStringList)
  private
    function GetProps(AIndex: Integer): PUserProp;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Clear; override;
    property Props[AIndex: Integer]: PUserProp read GetProps; default;
    procedure Add(const AUserId, AUserName, AFirmId, AFirmName: string); reintroduce;
  end;

  TMTEConnect = class
  public
    FIdx: Integer;
    FIface: TMicexIface;
    FSysTimeTable: Integer;
    FCaption: string;
    FTree: TTreeView;
    FSecList: TSecProps;
    FDisconnected,
    FLoggedon: Boolean;
    FFirmList,
    FBoardList,
    FSecCodeList: TFirmProps;
    FUserList: TUserProps;
    FParams: string;
    FInitialOpenMode: Boolean;
    constructor Create;
    destructor Destroy; override;
    procedure SetupKeepAlive;
  end;

  TTableIndex = class(TStringList)
  public
    function FindKey(const Key: string; var Index: Integer; SortedByKey: Boolean): Boolean;
    procedure AddKey(InsertPos: Integer; const Key: string; Index: Integer);
    procedure DeleteRow(const Index: Integer; SortedByKey: Boolean);
  end;

  TRowList = class(TList)
  private
    FMsg: TMicexMessage;
    FTableIndex: TTableIndex;
    FLastQuote: array [0..15] of Char; // SecBoard(4) + SecCode(12)
    FSecCodeOfs: Integer;
    FSecCodeLen: Integer;
    FDecimalsOfs: Integer;
    FDecimalsLen: Integer;
    FIsOrderBook: Boolean;
    FSecList: TSecProps;
    FBadDecimalsStr: string;
    function GetValue(ARow, ACol: Integer): string;
    function MakeKeyFromData(NumsCount: Integer; NumsPtr, DataPtr: PChar): string;
    function ClearOrderBook(Data: PChar; Len: Integer): Boolean;
  public
    constructor Create(AMsg: TMicexMessage; SecList: TSecProps);
    destructor Destroy; override;
    procedure AddRow(var Data: PChar);
    procedure Clear; {$IFDEF Post4} override; {$ENDIF}
    property Values[ARow, ACol: Integer]: string read GetValue; default;
    function RowDecimals(const ARow: Integer): Integer;
  end;

  PBaseMsgRec = ^TBaseMsgRec;
  TBaseMsgRec = record
    Msg: TMicexMessage;
    Rows: TRowList;
    RowNum: Integer;
  end;

  TTableForm = class(TBaseForm)
    LV: TListView;
    Splitter1: TSplitter;
    pmLink: TPopupMenu;
    miFilter: TMenuItem;
    N1: TMenuItem;
    miDelFilter: TMenuItem;
    miExport: TMenuItem;
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormDestroy(Sender: TObject);
    procedure LVData(Sender: TObject; Item: TListItem);
    procedure pmLinkPopup(Sender: TObject);
    procedure miFilterClick(Sender: TObject);
    procedure miDelFilterClick(Sender: TObject);
    procedure LVDblClick(Sender: TObject);
    procedure LVColumnClick(Sender: TObject; Column: TListColumn);
    procedure miExportClick(Sender: TObject);
  private
//    FIdx: Integer;
    FMsg: TMicexMessage;
    FRows: TRowList;
    FRefreshable: Boolean;
    FTblHandle: Integer;
    FCaption: string;
//    FIface: TMicexIface;
    FConnectCaption: string;
    FConnect: TMTEConnect;
    FFilterFieldCaption,
    FFilterValue: string;
    FFilterField, FSortField: TMicexField;
    FMapping: TList;
    FFilterFieldIndex, FSortFieldIndex: Integer;
    FFilterIndex: Integer;
    FIfaceName: string;
    FGridLayoutSaved: Boolean;
    FSortDescending: Boolean;
    FSortColumn: TListColumn;
    function GetRefreshable: Boolean;
    function SelectedRow: Integer;
    procedure SelectLastRec;
    procedure ExecuteLink(Sender: TObject);
    procedure DoFilter;
    procedure SaveGridLayout;
    function FormatValue(const Value: string; Fld: TMicexField;
      RowIndex: Integer; JustIds: Boolean): string;
  public
    function AddToPacket(Idx: Integer): Integer;
    procedure SetupStatics(AConnect: TMTEConnect; AMsg: TMicexMessage;
      Msg: PMTETable; ATblHandle: Integer; SecList: TSecProps; const AConnectCaption: string);
    procedure ParseData(var Msg: PMTETable; ForceRefilter: Boolean = False);
    property Refreshable: Boolean read GetRefreshable;
    function ConnectIdx: Integer;
    property ConnectPtr: TMTEConnect read FConnect;
  end;

function GetTsmrStr(Err: Integer; Msg: PMTEMsg): string;
procedure ShowError(const ACaption: string; Err: Integer; ErrMsg: PChar; ErrSource: string);
procedure DownloadSecurities(Connect: TMTEConnect);
procedure DownloadFirms(Connect: TMTEConnect);

{$IFDEF CLIENTLOG}
var
  DebugFile: Text;
{$ENDIF}

var
  IdViewMode: TIdViewMode = ivmIds;

implementation

uses
  ExtUtils, AcMain, acFilter, IniFiles, Consts;

{$R *.DFM}

var
  FieldSeqNums: array [0..255] of Char;
  CloseErrorCount: Integer = 0;

const
  SInvalidTime  = '#error!';
  SInvalidDate  = '#error!';
  MAX_COL_WIDTH = 300;

procedure TTableForm.FormClose(Sender: TObject; var Action: TCloseAction);
var
  Err: Integer;
begin
  LV.Items.Count := 0;
  SaveGridLayout;
  Err := MTECloseTable(FConnect.FIdx, FTblHandle);
  if (Err <> MTE_OK) and not FConnect.FDisconnected then
  begin
    // to avoid multiple "not connected" errors, show error just once
    if CloseErrorCount = 0 then ShowError(FConnect.FCaption, Err, '', 'MTECloseTable');
    Inc(CloseErrorCount);
  end;
  Action := caFree;
end;

procedure TTableForm.SelectLastRec;
begin
  if FRows.Count > 0 then
    with LV.Items[FRows.Count - 1] do
  begin
    MakeVisible(False);
    Selected := True;
    Focused := True;
  end;
end;

procedure TTableForm.ParseData(var Msg: PMTETable; ForceRefilter: Boolean = False);
var
  P: PChar;
begin
  try
    {$IFDEF CLIENTLOG}
    Writeln(DebugFile, '[' + FCaption + ']');
    Writeln(DebugFile, Format('RowCount = %d', [Msg.RowCount]));
    {$ENDIF}

    if Msg.RowCount > 0 then
    begin
      if FMsg.FClearOnUpdate and not FRows.FIsOrderBook then
      begin
        FRows.Clear;
        {$IFDEF CLIENTLOG}
        Writeln(DebugFile, 'FRows.Clear');
        {$ENDIF}
      end;

      FRows.FBadDecimalsStr := '';
      P := @Msg.TblData;
      repeat
        FRows.AddRow(P);
        Dec(Msg.RowCount);
      until Msg.RowCount = 0;
      Msg := PMTETable(P);

      if FRows.FIsOrderBook then
        // prepare FRow for next cycle
        FillChar(FRows.FLastQuote, SizeOf(FRows.FLastQuote), 0);

      DoFilter;
      if FRows.FBadDecimalsStr <> '' then
        MessageDlg(Format('Сервер вернул данные недопустимого формата: поле DECIMALS = "%s"',
          [FRows.FBadDecimalsStr]), mtError, [mbOk], 0);

//      if FRows.FIsOrderBook then UpdateOrderbook(FRows);
    end
    else
    begin
      Msg := @Msg.TblData; // Just skip 8 bytes and move to next table header
      if ForceRefilter then DoFilter;
    end;

(*    with LV, Items do
    begin
      DoScroll := (Count = 0) or (SelectedRow = Count - 1);
      DoFilter;
      if Assigned(FFilterField) then NewCount := FMapping.Count
      else NewCount := FRows.Count;
      if Count <> NewCount then
      begin
        Count := NewCount;
        if DoScroll then SelectLastRec;
      end;
      Invalidate;
    end;

    Caption := Format('%s - %d/%d - %s', [FCaption, NewCount, FRows.Count, FConnectCaption]);*)
  except
    on E: Exception do
    begin
      {$IFDEF CLIENTLOG}
      Writeln(DebugFile, 'EXCEPTION! ', E.ClassName, ': ', E.Message);
      Flush(DebugFile);
      {$ENDIF}
      FRefreshable := False;
      raise;
    end;
  end;

  {$IFDEF CLIENTLOG}
  Flush(DebugFile);
  {$ENDIF}
end;

procedure TTableForm.SetupStatics(AConnect: TMTEConnect;
  AMsg: TMicexMessage; Msg: PMTETable; ATblHandle: Integer; SecList: TSecProps; const AConnectCaption: string);
var
  I, DigitWidth, CharWidth, TotalWidth: Integer;
  Fld: TMicexField;
  Section, S: string;
  ColIndices: array of Integer;

  function ColumnWidth: Integer;
  var
    CW, CapSize: Integer;
  begin
    if Fld.FType = ftChar then CW := CharWidth else CW := DigitWidth;
    Result := CW * Fld.FSize;
    if Fld.FType = ftDate then Inc(Result, LV.Canvas.TextWidth(DateSeparator) * 3);
    if Fld.FType = ftTime then Inc(Result, LV.Canvas.TextWidth(TimeSeparator) * 3);
    if Fld.FType in [ftFixed, ftFloat] then Inc(Result, LV.Canvas.TextWidth(DecimalSeparator) * 2);
    CapSize := LV.Canvas.TextWidth(Fld.DisplayName);
    if CapSize > Result then Result := CapSize;
    if Fld.FType = ftChar then Inc(Result, 2);
    Inc(Result, 12);  // 6 left + 6 right spacing
    if Result > MAX_COL_WIDTH then Result := MAX_COL_WIDTH;
  end;

begin
  FConnect := AConnect;
  FMsg := AMsg;
  FConnectCaption := AConnectCaption;
  FRows := TRowList.Create(AMsg, SecList);
  FMapping := TList.Create;
  FTblHandle := ATblHandle;
  FCaption := Caption;
  FFilterIndex := -1;
  FIfaceName := FConnectCaption;
  for I := 1 to Length(FIfaceName) do
    if FIfaceName[I] in ['&', ' ', '[', ']'] then FIfaceName[I] := '_';

  DigitWidth := LV.Canvas.TextWidth('0123456789') div 10;
  CharWidth := LV.Canvas.TextWidth('ABCDEFGHIJKLMNOPQRSTUVWXYZ') div 26;

  LV.DoubleBuffered := True;
  LV.Items.BeginUpdate;
  LV.Columns.BeginUpdate;
  try
    // Delphi doesn't allow to align first column
    LV.Columns.Add.Width := 16;      // Add dummy not-alignable column
    TotalWidth := 20;

    with TIniFile.Create(IniFileName) do
    try
      Section := FIfaceName + '_' + FMsg.FName;

      for I := 0 to AMsg.FOutFields.Count - 1 do
      begin
        Fld := AMsg.FOutFields[I];
        with LV.Columns.Add do
        begin
          Caption := Fld.DisplayName;
          Width := ReadInteger(Section, Fld.FName, ColumnWidth);
          Tag := I;
          Inc(TotalWidth, Width);
          if Fld.FType <> ftChar then Alignment := taRightJustify;
        end;
      end;

      S := ReadString(Section, '@ColumnOrder', '');
      SetLength(ColIndices, LV.Columns.Count);
      for I := 0 to Length(ColIndices) - 1 do
        ColIndices[I] := EStrToInt(EWordNum(',', S, I + 1), I);
      ListView_SetColumnOrderArray(LV.Handle, Length(ColIndices), PInteger(ColIndices));
    finally
      Free;
    end;
  finally
    LV.Columns.EndUpdate;
    LV.Items.EndUpdate;
  end;

  if TotalWidth < ClientWidth then ClientWidth := TotalWidth;
  ParseData(Msg, True);
  // Correct form height: header + one extra row for spacing + space for scrollbar
  I := 4 + (FRows.Count + 3) * (LV.Canvas.TextHeight('Wg') + 2);
  if I < ClientHeight then ClientHeight := I;
  FRefreshable := True;
end;

function TTableForm.AddToPacket(Idx: Integer): Integer;
begin
  Result := MTEAddTable(Idx, FTblHandle, Integer(Self));
end;

function TTableForm.GetRefreshable: Boolean;
begin
  Result := Assigned(FMsg) and FMsg.FUpdateable and FRefreshable;
end;

{ TRowList }

function TRowList.MakeKeyFromData(NumsCount: Integer; NumsPtr, DataPtr: PChar): string;
var
  I, FieldIdx, Offset: Integer;
  KeyField: string;                  
begin
  Result := '';
  Offset := 0;

  for I := 0 to NumsCount - 1 do
  begin
    FieldIdx := Byte(NumsPtr[I]);
    with FMsg.FOutFields[FieldIdx] do
    begin
      if FKey then
      begin
        SetString(KeyField, DataPtr + Offset, FSize);
        Result := Result + KeyField;
      end;
      Inc(Offset, FSize);
    end;
  end;
end;

function TRowList.ClearOrderBook(Data: PChar; Len: Integer): Boolean;
var
  QuoteLen, I: Integer;
begin
  Result := False;
  QuoteLen := EMin(Len, SizeOf(FLastQuote));
  if CompareMem(@FLastQuote, Data, QuoteLen) then Exit; // not first occurence of the quote, do not clear quotes
//  ClearForSecurity(Data);

  {$IFDEF CLIENTLOG}
  Writeln(DebugFile, 'Clear quotes for given security');
  {$ENDIF}

  for I := Count - 1 downto 0 do
  begin
    if CompareMem(Data, Items[I], QuoteLen) then
    begin
      FreeMem(Items[I]);
      Delete(I);
      // delete reference for this row from Index
      FTableIndex.DeleteRow(I, FIsOrderBook);
    end;
  end;
  System.Move(Data^, FLastQuote, QuoteLen);
  Result := Len = SizeOf(FLastQuote);
end;

function GetFieldNums(NumsPtr: PChar; NumsCount: Integer): string;
var
  I: Integer;
begin
  Result := '';
  for I := 0 to NumsCount - 1 do
    Result := Result + '#' + IntToStr(Byte(NumsPtr[I]));
end;

procedure TRowList.AddRow(var Data: PChar);
var
  H: TMTERow;
  FldNum, RowNum, NumsCount, ReplySize, Decimals: Integer;
  FullRowPtr, NumsPtr, DataPtr: PChar;
  Append, SkipRow: Boolean;
  KeyForData, SecCode: string;
  {$IFDEF CLIENTLOG}
  S: string;
  Len: Integer;
  {$ENDIF}
begin
  System.Move(Data^, H, SizeOf(H));
  Inc(Data, SizeOf(H));

  {$IFDEF CLIENTLOG}
  Len := H.RowLen;
  SetString(S, PChar(Data + H.FldCount), Len);
  Writeln(DebugFile, Format('Flds=%d(%s),Len=%d,Data=''%s''',
    [H.FldCount, GetFieldNums(Data, H.FldCount), Len, S]));
  {$ENDIF}

  NumsCount := H.FldCount;
  NumsPtr := Data;
  DataPtr := Data + H.FldCount;
  if NumsCount = 0 then
  begin
    NumsCount := FMsg.FOutFields.Count;
    NumsPtr := FieldSeqNums;
  end;

  if FIsOrderBook then// clear quotes for given security?
    SkipRow := ClearOrderBook(DataPtr, H.RowLen)
  else
    SkipRow := False;

  if (not SkipRow) and (H.RowLen > 0) then
  begin
    FullRowPtr := nil;
    with FMsg.FOutFields do
    begin
      Append := True;

      if FKeyIdx >= 0 then
      begin
        KeyForData := MakeKeyFromData(NumsCount, NumsPtr, DataPtr);
        if FTableIndex.FindKey(KeyForData, RowNum, True{FIsOrderBook}) then
        begin
          Append := False;
          FullRowPtr := Items[RowNum];
        end
        else
          FTableIndex.AddKey(RowNum, KeyForData, Self.Count);
      end
      else
        RowNum := Self.Count;

      ReplySize := FSumSize;
      if Append then
      begin
        GetMem(FullRowPtr, ReplySize + SizeOf(Integer)); // + place for Decimals
        FillChar(FullRowPtr^, ReplySize, #32);
        PInteger(@FullRowPtr[ReplySize])^ := 0;
        if True{FIsOrderBook }then
          Self.Insert(RowNum, FullRowPtr)   // sort orderbook by price
        else
          Self.Add(FullRowPtr);
      end;
    end;

    for FldNum := 0 to NumsCount - 1 do
      with FMsg.FOutFields[Byte(NumsPtr[FldNum])] do
    begin
      System.Move(DataPtr^, FullRowPtr[FStart], FSize);
      Inc(DataPtr, FSize);
    end;

    if Append then
    begin
      if FDecimalsOfs >= 0 then
      begin
        // Get Decimals from the same row (e.g. INDEXES)
        SetString(SecCode, FullRowPtr + FDecimalsOfs, FDecimalsLen);
        Decimals := StrToIntDef(SecCode, -1);  {@@@@@}
        if Decimals < 0 then
        begin
          FBadDecimalsStr := SecCode;
          Decimals := 0;
        end;
        PInteger(@FullRowPtr[ReplySize])^ := Decimals;
      end
      else
        if (FSecCodeOfs >= 0) then // calculate Decimals for security in this row
        begin
          SetString(SecCode, FullRowPtr + FSecCodeOfs, FSecCodeLen);
          if FSecList.Find(SecCode, RowNum) then
            PInteger(@FullRowPtr[ReplySize])^ := FSecList[RowNum].Decimals;
        end;
    end;

  end;
  Inc(Data, H.FldCount + H.RowLen);
end;

procedure TRowList.Clear;
var
  I: Integer;
begin
  for I := 0 to Count - 1 do FreeMem(Items[I]);
  if Assigned(FTableIndex) then FTableIndex.Clear;
  inherited;
end;

constructor TRowList.Create(AMsg: TMicexMessage; SecList: TSecProps);
var
  I: Integer;
begin
  inherited Create;
  FMsg := AMsg;
  FSecList := SecList; 
  FTableIndex := TTableIndex.Create;
  FIsOrderBook := FMsg.FIsOrderbook or (FMsg.FName = 'ORDERBOOK') or (FMsg.FName = 'QUOTEBOOK');

  FSecCodeOfs := -1;
  FDecimalsOfs := -1;

  with AMsg do
    for I := 0 to FOutFields.Count - 1 do
      with FOutFields[I] do
        if FName = 'DECIMALS' then
        begin
          FDecimalsOfs := FStart;
          FDecimalsLen := FSize;
          Exit;
        end;

  with AMsg do
    for I := 0 to FOutFields.Count - 1 do
      with FOutFields[I] do
        if FSecCode then
        begin
          FSecCodeOfs := FStart;
          FSecCodeLen := FSize;
          
          if I > 0 then
            with FOutFields[I - 1] do
          begin
            Dec(FSecCodeOfs, FSize);
            Inc(FSecCodeLen, FSize);
          end;
          Break;
        end;
end;

function TRowList.GetValue(ARow, ACol: Integer): string;
begin
  with FMsg.FOutFields[ACol] do
    SetString(Result, PChar(Items[ARow]) + FStart, FSize);
end;

procedure TTableForm.SaveGridLayout;
var
  Section, S: string;
  I: Integer;
  ColIndices: array of Integer;
begin
  with TIniFile.Create(IniFileName) do
  try
    Section := FIfaceName + '_' + FMsg.FName;
    for I := 1 to LV.Columns.Count - 1 do
      WriteInteger(Section, FMsg.FOutFields[LV.Columns[I].Tag].FName,
        EMax(3, LV.Columns[I].Width));

    SetLength(ColIndices, LV.Columns.Count);
    ListView_GetColumnOrderArray(LV.Handle, Length(ColIndices), PInteger(ColIndices));
    S := '';
    for I := 0 to Length(ColIndices) - 1 do
    begin
      if S <> '' then S := S + ', ';
      S := S + IntToStr(ColIndices[I]);
    end;
    WriteString(Section, '@ColumnOrder', S);
    FGridLayoutSaved := True;
  finally
    Free;
  end;
end;

procedure TTableForm.FormDestroy(Sender: TObject);
begin
  FRows.Free;
  FMapping.Free;
  if not FGridLayoutSaved then SaveGridLayout;
end;

destructor TRowList.Destroy;
begin
  FTableIndex.Free;
  FTableIndex := nil;
  {$IFNDEF Post4}
  Clear;
  {$ENDIF}
  inherited;
end;

function TRowList.RowDecimals(const ARow: Integer): Integer;
begin
  Result := PInteger(PChar(Items[ARow]) + FMsg.FOutFields.FSumSize)^;
end;

{ TTableIndex }

procedure TTableIndex.AddKey(InsertPos: Integer; const Key: string; Index: Integer);
begin
  InsertObject(InsertPos, Key, TObject(Index));
end;

procedure TTableIndex.DeleteRow(const Index: Integer; SortedByKey: Boolean);
var
  I: Integer;
begin
  for I := 0 to Count - 1 do
    if (SortedByKey and (I = Index)) or (not SortedByKey and (Index = Integer(Objects[I]))) then
    begin
      Delete(I);
      Break;
    end;
end;

function TTableIndex.FindKey(const Key: string; var Index: Integer; SortedByKey: Boolean): Boolean;
begin
  Result := Find(Key, Index);
  if Result and not SortedByKey then
    Index := Integer(Objects[Index]);
end;

function ConvertStrToDateTime(const S: string; FieldType: TTEFieldType): TDateTime;

  function GetNumber(Buf: PChar): Integer;
  begin
    Result := (Byte(Buf^) - Ord('0')) * 10 + Byte((Buf + 1)^) - Ord('0');
  end;

var
  P: PChar;
begin
  Result := 0;
  try
    if FieldType = ftDate then
      if Length(S) >= 8 then
      begin
        P := PChar(S);
        Result := EncodeDate(StrToInt(Copy(S, 1, 4)), GetNumber(P + 4), GetNumber(P + 6));
      end
      else
    else
      if Length(S) >= 6 then
      begin
        P := PChar(S);
        Result := EncodeTime(GetNumber(P), GetNumber(P + 2), GetNumber(P + 4), 0);
      end;
  except
  end;
end;

function FormatTime(const S: string): string;
var
  DT: TDateTime;
begin
  if EIsZero(S) then Result := ''
  else
  begin
    DT := ConvertStrToDateTime(S, ftTime);
    if DT <> 0 then
      Result := TimeToStr(DT)
    else
      Result := SInvalidTime;
  end;
end;

function FormatDate(const S: string): string;
var
  DT: TDateTime;
begin
  if EIsZero(S) then Result := ''
  else
  begin
    DT := ConvertStrToDateTime(S, ftDate);
    if DT <> 0 then
      Result := DateToStr(DT)
    else
      Result := SInvalidDate;
  end;
end;

function TTableForm.SelectedRow: Integer;
begin
  Result := ListView_GetNextItem(LV.Handle, -1, LVNI_ALL or LVNI_SELECTED);
end;

function TTableForm.FormatValue(const Value: string; Fld: TMicexField; RowIndex: Integer;
  JustIds: Boolean): string;
var
  Index: Integer;
begin
  Result := Value;
  if Assigned(Fld.FEnumPtr) then
    with Fld.FEnumPtr do
      Result := GetShortName(Result)
  else
    case Fld.FType of
      ftChar:
        begin
          if (IdViewMode <> ivmIds) and not JustIds then
          begin
            case Fld.FNameType of
              fntUserId:
                if FConnect.FUserList.Find(Value, Index) then
                  with FConnect.FUserList[Index]^ do
                    FmtStr(Result, '%s, %s', [UserName, FirmName]);
              fntFirmId:
                if FConnect.FFirmList.Find(Value, Index) then
                  Result := FConnect.FFirmList[Index].ShortName;
              fntFirmUserId:
                if FConnect.FUserList.Find(Value, Index) then
                  with FConnect.FUserList[Index]^ do
                    FmtStr(Result, '%s, %s', [UserName, FirmName])
                else
                  if FConnect.FFirmList.Find(Value, Index) then
                    with FConnect.FFirmList[Index]^ do
                      Result := ShortName;
              fntSecCode:
                if FConnect.FSecCodeList.Find(Value, Index) then
                  Result := FConnect.FSecCodeList[Index].ShortName;
              fntSecBoard:
                if FConnect.FBoardList.Find(Value, Index) then
                  Result := FConnect.FBoardList[Index].ShortName;
              else
                Result := Trim(Value);  // FNameType = fntNone
            end;
            if (IdViewMode = ivmAll) and (Fld.FNameType <> fntNone) then FmtStr(Result, '%s (%s)', [Trim(Value), Result]);
          end
          else
            Result := Trim(Value);  // IdViewMode = ivmIds
        end;
      ftTime: Result := FormatTime(Trim(Value));
      ftDate: Result := FormatDate(Trim(Value));
      ftFixed: Result := EDisplayInt(EFormatNumber(Value, 2));
      ftInteger: Result := EDisplayInt(EFormatNumber(Value, 0));
      ftFloat: Result := EDisplayInt(EFormatNumber(Value, FRows.RowDecimals(RowIndex)));
    end;
  if not (FRows.FIsOrderbook and (Fld.FName = 'PRICE')) and EIsZero(Result) then Result := ''
end;

procedure TTableForm.LVData(Sender: TObject; Item: TListItem);
var
  I, RowIndex, FldIndex: Integer;
begin
  if Assigned(FFilterField) or Assigned(FSortField) then
    if (Item.Index < 0) or (Item.Index >= FMapping.Count) then Exit
    else RowIndex := Integer(FMapping[Item.Index])
  else RowIndex := Item.Index;

  with Item do
    if (RowIndex >= 0) and (RowIndex < FRows.Count) then
  begin
    if SelectedRow = RowIndex then Caption := '>' else Caption := '';

    for I := 1 to LV.Columns.Count - 1 do
    begin
      FldIndex := I - 1; //LV.Columns[I].Tag;
      with FMsg.FOutFields do
        SubItems.Add(FormatValue(FRows[RowIndex, FldIndex], Fields[FldIndex], RowIndex, False));
    end;
  end;
end;

function GetTsmrStr(Err: Integer; Msg: PMTEMsg): string;
begin
  if ((Err = MTE_TSMR) or (Err = MTE_TRANSREJECTED) or (Err = MTE_NOTIMPLEMENTED) or (Err = MTE_TEUNAVAIL))
    and Assigned(Msg) and (Msg.DataLen > 0) then
      SetString(Result, PChar(@Msg.Data), Msg.DataLen)
  else
    Result := '';
end;

procedure ShowError(const ACaption: string; Err: Integer; ErrMsg: PChar; ErrSource: string);
var
  S: string;
  MsgCaption: string;
begin
  MsgCaption := ACaption;
  if MsgCaption = '' then MsgCaption := SMsgDlgError;
  FmtStr(S, 'Код %d: %s', [Err, MTEErrorMsg(Err)]);
  case Err of
    MTE_OK:
      Application.MessageBox(PChar(S + ^M^M + ErrMsg), PChar(MsgCaption), MB_ICONINFORMATION or MB_OK or MB_SETFOREGROUND);
    MTE_TSMR, MTE_TRANSREJECTED:
      Application.MessageBox(PChar(S + ^M^M + ErrMsg), PChar(MsgCaption), MB_ICONHAND or MB_OK or MB_SETFOREGROUND);
    else Application.MessageBox(PChar(S + ' [' + ErrSource + ']' + ^M^M + ErrMsg), PChar(MsgCaption), MB_ICONHAND or MB_OK or MB_SETFOREGROUND);
  end
end;

function FindSecField(Secs: TMicexMessage; const FieldName: string;
  AlwaysFind: Boolean = True): Integer;
begin
  if not Secs.FOutFields.Find(FieldName, Result) then
   if AlwaysFind then
      raise Exception.CreateFmt('Can''t find mandatory field %s in %s table',
        [FieldName, Secs.FName])
   else Result := -1;
end;

const
  SSecTable = 'SECURITIES';
  SFoSecTypeTable = 'FOSECTYPES';

procedure FetchTable(Data: PMTEMsg; Rows: TRowList);
var
  Msg: PMTETable;
  P: PChar;
begin
  Msg := @Data.Data;
  if Msg.RowCount > 0 then
  begin
    P := @Msg.TblData;
    repeat
      Rows.AddRow(P);
      Dec(Msg.RowCount);
    until Msg.RowCount = 0;
  end;
end;

procedure DownloadSecTypes(Connect: TMTEConnect; Iface: TMicexIface;
  TableIndex: Integer; var FoSecTypes: TStringList; SecList: TSecProps);
var
  Table: TMicexMessage;
  Rows: TRowList;
  I, ErrCode, SecTypeIdx, DecimalsIdx: Integer;
  Data: PMTEMsg;
begin
  Table := Iface.FTables[TableIndex];
  Rows := TRowList.Create(Table, SecList);
  try
    ErrCode := MTEOpenTable(Connect.FIdx, SFoSecTypeTable,
      PChar(StringOfChar(#32, Table.FInFields.FSumSize)), Connect.FInitialOpenMode, Data);
    if ErrCode < MTE_OK then
    begin
      MainForm.ValidateConnect(Connect, ErrCode);
      ShowError('', ErrCode, PChar(GetTsmrStr(ErrCode, Data)), 'MTEOpenTable(' + SFoSecTypeTable + ')')
    end
    else
    try
      FetchTable(Data, Rows);
      SecTypeIdx := FindSecField(Table, 'FOSECTYPEID');
      DecimalsIdx := FindSecField(Table, 'DECIMALS');

      FoSecTypes := TStringList.Create;
      FoSecTypes.Sorted := True;
      for I := 0 to Rows.Count - 1 do
      begin
        FoSecTypes.AddObject(Rows.Values[I, SecTypeIdx],
          TObject(StrToInt(Rows.Values[I, DecimalsIdx])));
      end;
    finally
      MTECloseTable(Connect.FIdx, ErrCode);
    end;
  finally
    Rows.Free;
  end;
end;

procedure DownloadFirms(Connect: TMTEConnect);
var
  TblIdx, ErrCode, IdIdx, NameIdx, I, FirmIdIdx, FoundIdx: Integer;
  Firms, Users, Boards: TMicexMessage;
  Data: PMTEMsg;
  Rows: TRowList;
  FirmId, FirmName: string;
begin
  Connect.FFirmList.Clear;
  Connect.FUserList.Clear;
  Connect.FBoardList.Clear;
  if Connect.FIface.FTables.Find('FIRMS', TblIdx) and not Connect.FDisconnected then
  begin
    Firms := Connect.FIface.FTables[TblIdx];
    Rows := TRowList.Create(Firms, Connect.FSecList);
    try
      ErrCode := MTEOpenTable(Connect.FIdx, 'FIRMS',
        PChar(StringOfChar(#32, Firms.FInFields.FSumSize)), Connect.FInitialOpenMode, Data);
      if ErrCode < MTE_OK then
      begin
        MainForm.ValidateConnect(Connect, ErrCode);
        ShowError('', ErrCode, PChar(GetTsmrStr(ErrCode, Data)), 'MTEOpenTable(FIRMS)');
      end
      else
      try
        FetchTable(Data, Rows);
        IdIdx := FindSecField(Firms, 'FIRMID', False);
        if IdIdx < 0 then IdIdx := FindSecField(Firms, 'ID');
        NameIdx := FindSecField(Firms, 'FIRMNAME', False);
        if NameIdx < 0 then NameIdx := FindSecField(Firms, 'NAME');

        for I := 0 to Rows.Count - 1 do
          Connect.FFirmList.Add(Rows.Values[I, IdIdx], Trim(Rows.Values[I, NameIdx]));
      finally
        MTECloseTable(Connect.FIdx, ErrCode);
      end;
    finally
      Rows.Free;
    end;
  end
  else
    MainForm.ShowStatus('Warning: Can''t find FIRMS tables');

  if Connect.FIface.FTables.Find('USERS', TblIdx) and not Connect.FDisconnected then
  begin
    Users := Connect.FIface.FTables[TblIdx];
    Rows := TRowList.Create(Users, Connect.FSecList);
    try
      ErrCode := MTEOpenTable(Connect.FIdx, 'USERS',
        PChar(StringOfChar(#32, Users.FInFields.FSumSize)), Connect.FInitialOpenMode, Data);
      if ErrCode < MTE_OK then
      begin
        MainForm.ValidateConnect(Connect, ErrCode);
        ShowError('', ErrCode, PChar(GetTsmrStr(ErrCode, Data)), 'MTEOpenTable(USERS)');
      end
      else
      try
        FetchTable(Data, Rows);
        IdIdx := FindSecField(Users, 'USERID', False);
        if IdIdx < 0 then IdIdx := FindSecField(Users, 'ID');
        FirmIdIdx := FindSecField(Users, 'FIRMID');
        NameIdx := FindSecField(Users, 'USERNAME', False);
        if NameIdx < 0 then NameIdx := FindSecField(Users, 'NAME');

        for I := 0 to Rows.Count - 1 do
        begin
          if FirmIdIdx >= 0 then FirmId := Rows.Values[I, FirmIdIdx] else FirmId := '';
          if (FirmId <> '') and Connect.FFirmList.Find(FirmId, FoundIdx) then
            FirmName := Connect.FFirmList[FoundIdx].ShortName else FirmName := '';
          Connect.FUserList.Add(Rows.Values[I, IdIdx], Trim(Rows.Values[I, NameIdx]),
            FirmId, FirmName);
        end;
      finally
        MTECloseTable(Connect.FIdx, ErrCode);
      end;
    finally
      Rows.Free;
    end;
  end
  else
    MainForm.ShowStatus('Warning: Can''t find USERS tables');

  if Connect.FIface.FTables.Find('BOARDS', TblIdx) and not Connect.FDisconnected then
  begin
    Boards := Connect.FIface.FTables[TblIdx];
    Rows := TRowList.Create(Boards, Connect.FSecList);
    try
      ErrCode := MTEOpenTable(Connect.FIdx, 'BOARDS',
        PChar(StringOfChar(#32, Boards.FInFields.FSumSize)), Connect.FInitialOpenMode, Data);
      if ErrCode < MTE_OK then
      begin
        MainForm.ValidateConnect(Connect, ErrCode);
        ShowError('', ErrCode, PChar(GetTsmrStr(ErrCode, Data)), 'MTEOpenTable(BOARDS)');
      end
      else
      try
        FetchTable(Data, Rows);
        IdIdx := FindSecField(Boards, 'BOARDID', False);
        if IdIdx < 0 then IdIdx := FindSecField(Boards, 'ID');
        NameIdx := FindSecField(Boards, 'BOARDNAME', False);
        if NameIdx < 0 then NameIdx := FindSecField(Boards, 'NAME');

        for I := 0 to Rows.Count - 1 do
          Connect.FBoardList.Add(Rows.Values[I, IdIdx], Trim(Rows.Values[I, NameIdx]));
      finally
        MTECloseTable(Connect.FIdx, ErrCode);
      end;
    finally
      Rows.Free;
    end;
  end
  else
    MainForm.ShowStatus('Warning: Can''t find BOARDS tables');
end;

procedure DownloadSecurities(Connect: TMTEConnect);
var
  TblIdx, ErrCode, I, BoardIdx, SecCodeIdx, ShortNameIdx,
    DecimalsIdx, SecTypeIdx, Index: Integer;
  Rows: TRowList;
  Secs: TMicexMessage;
  Data: PMTEMsg;
  FoSecTypes: TStringList;
  SecCode, ShortName, SecTypeId, SecUniq: string;
begin
  // is it FO engine? if so DECIMALS must be taken from FOSECTYPES table
  Connect.FSecList.Clear;
  Connect.FSecCodeList.Clear;
  FoSecTypes := nil;
  try
    if Connect.FIface.FTables.Find(SFoSecTypeTable, TblIdx) and not Connect.FDisconnected then
      DownloadSecTypes(Connect, Connect.FIface, TblIdx, FoSecTypes, Connect.FSecList);

    if Connect.FIface.FTables.Find(SSecTable, TblIdx) and not Connect.FDisconnected then
    begin
      Secs := Connect.FIface.FTables[TblIdx];
      Rows := TRowList.Create(Secs, Connect.FSecList);
      try
        ErrCode := MTEOpenTable(Connect.FIdx, SSecTable,
          PChar(StringOfChar(#32, Secs.FInFields.FSumSize)), Connect.FInitialOpenMode, Data);
        if ErrCode < MTE_OK then
        begin
          MainForm.ValidateConnect(Connect, ErrCode);
          ShowError('', ErrCode, PChar(GetTsmrStr(ErrCode, Data)), 'MTEOpenTable(' + SSecTable + ')');
        end
        else
        try
          FetchTable(Data, Rows);
          BoardIdx := FindSecField(Secs, 'SECBOARD');
          SecCodeIdx := FindSecField(Secs, 'SECCODE');
          ShortNameIdx := FindSecField(Secs, 'SHORTNAME', False);
          if ShortNameIdx < 0 then ShortNameIdx := FindSecField(Secs, 'SECNAME');
          DecimalsIdx := FindSecField(Secs, 'DECIMALS', False);
          if DecimalsIdx < 0 then SecTypeIdx := FindSecField(Secs, 'FOSECTYPEID')
            else SecTypeIdx := -1;

          for I := 0 to Rows.Count - 1 do
          begin
            SecUniq := Rows.Values[I, SecCodeIdx];
            SecCode := Rows.Values[I, BoardIdx] + SecUniq;
            ShortName := Trim(Rows.Values[I, ShortNameIdx]);

            if DecimalsIdx < 0 then                           
            begin
              SecTypeId := Rows.Values[I, SecTypeIdx];
              if FoSecTypes.Find(SecTypeId, Index) then
                Connect.FSecList.Add(SecCode, ShortName,
                  Integer(FoSecTypes.Objects[Index]))
              else
                raise Exception.CreateFmt('Invalid FO security type: ''%s''',
                  [SecTypeId]);
            end
            else
              Connect.FSecList.Add(SecCode, ShortName,
                StrToInt(Rows.Values[I, DecimalsIdx]));

            if not Connect.FSecCodeList.Find(SecUniq, Index) then
              Connect.FSecCodeList.Add(SecUniq, ShortName);
          end;
        finally
          MTECloseTable(Connect.FIdx, ErrCode);
        end;
      finally
        Rows.Free;
      end;
    end
    else
//      raise Exception.CreateFmt('Can''t find %s tables', [SSecTable]);
      MainForm.ShowStatus(Format('Warning: Can''t find %s tables', [SSecTable]));
  finally
    FoSecTypes.Free;
  end;
end;

{ TSecPropList }

procedure TSecProps.Add(const ASecCode, AShortName: string; ADecimals: Integer);
var
  P: PSecProp;
begin
  New(P);
  try
    P.SecCode := ASecCode;
    P.Decimals := ADecimals;
    P.ShortName := Trim(AShortName);
    if P.ShortName = '' then P.ShortName := ASecCode;
    inherited AddObject(ASecCode, TObject(P));
  except
    Dispose(P);
    raise;
  end;
end;

procedure TSecProps.Clear;
var
  I: Integer;
begin
  for I := 0 to Count - 1 do
    Dispose(PSecProp(Objects[I]));
  inherited Clear;
end;

constructor TSecProps.Create;
begin
  inherited;
  Sorted := True;
end;

destructor TSecProps.Destroy;
begin
  Clear;
  inherited;
end;

function TSecProps.GetProps(AIndex: Integer): PSecProp;
begin
  Result := PSecProp(Objects[AIndex]);
end;

procedure TTableForm.ExecuteLink(Sender: TObject);
var
  Msg: TMicexMessage;
  FromMsg: TBaseMsgRec;
begin
  Msg := TMicexMessage((Sender as TMenuItem).Tag);
  if Assigned(Msg) then
  begin
    FromMsg.Msg := FMsg;
    FromMsg.Rows := FRows;
    FromMsg.RowNum := LV.Selected.Index;
    if Assigned(FFilterField) or Assigned(FSortField) then
      FromMsg.RowNum := Integer(FMapping[FromMsg.RowNum]);
    with MainForm do
      if Msg.FIsTable then OpenTable(ConnectPtr, Msg, @FromMsg)
      else ExecuteTransaction(ConnectPtr, Msg, @FromMsg);
  end;
end;

procedure TTableForm.pmLinkPopup(Sender: TObject);

  function HasCommonFields(InMsg, OutMsg: TMicexMessage): Boolean;
  var
    I, Dummy: Integer;
  begin
    Result := True;
    for I := 0 to InMsg.FInFields.Count - 1 do
      if OutMsg.FOutFields.Find(InMsg.FInFields[I].FName, Dummy) then
        Exit;
    Result := False;
  end;

  procedure AddMenuItem(const ACaption: string; Msg: TMicexMessage);
  var
    Item: TMenuItem;
  begin
    Item := TMenuItem.Create(pmLink);
    with Item do
    begin
      Caption := ACaption;
      Tag := Integer(Msg);
      OnClick := ExecuteLink;
    end;
    pmLink.Items.Add(Item);
  end;

  procedure AddMessages(Messages: TMicexMessages);
  var
    Msg: TMicexMessage;
    I: Integer;
  begin
    for I := 0 to Messages.Count - 1 do
    begin
      Msg := Messages[I];
      if HasCommonFields(Msg, FMsg) then
        AddMenuItem(Msg.FName + ' - ' + Msg.FCaption, Msg);
    end;
  end;

var
  I: Integer;
begin
  inherited;
  for I := pmLink.Items.Count - 1 downto 0 do
    if pmLink.Items[I].Tag <> 0 then
      pmLink.Items.Delete(I);

  miDelFilter.Enabled := Assigned(FFilterField);
  if not Assigned(LV.Selected) then Exit;

  AddMessages(FConnect.FIface.FTables);
  I := pmLink.Items.Count;
  if I > 0 then
  begin
    AddMenuItem('-', nil);
    Inc(I);
  end;
  AddMessages(FConnect.FIface.FTransactions);
  with pmLink.Items do
    if (Count > 0) and (Count = I) then Delete(Count - 1);
end;

{ TMTEConnect }

constructor TMTEConnect.Create;
begin
  inherited Create;
  FSecList := TSecProps.Create;
  FSecCodeList := TFirmProps.Create;
  FFirmList := TFirmProps.Create;
  FUserList := TUserProps.Create;
  FBoardList := TFirmProps.Create;
  FInitialOpenMode := True;
end;

destructor TMTEConnect.Destroy;
var
  TmpHandle, Err: Integer;
begin
  if FIdx >= 0 then
  begin
    // close "keep-alive" (systime) table
    TmpHandle := FSysTimeTable;
    FSysTimeTable := -1;
    if TmpHandle >= MTE_OK then MTECloseTable(FIdx, TmpHandle);

    MainForm.CutTrees(Self);
    Err := MTEDisconnect(FIdx);
    if (Err <> MTE_OK) and not FDisconnected then ShowError(FCaption, Err, '', 'MTEDisconnect');
    FIface.Free;
  end;

  FSecList.Free;
  FFirmList.Free;
  FUserList.Free;
  FSecCodeList.Free;
  FBoardList.Free;
  inherited Destroy;
end;


var
  I: Integer;

function TTableForm.ConnectIdx: Integer;
begin
  Result := FConnect.FIdx;
end;

var
  CurrentTableForm: TTableForm;

function ListSortCompare(Item1, Item2: Pointer): Integer;
var
  Value1, Value2: string;
  Decimals1, Decimals2: Integer;
  F: Extended;
begin
  with CurrentTableForm do
  begin
    Value1 := FRows.Values[Integer(Item1), FSortFieldIndex];
    Decimals1 := FRows.RowDecimals(Integer(Item1));
    Value2 := FRows.Values[Integer(Item2), FSortFieldIndex];
    Decimals2 := FRows.RowDecimals(Integer(Item2));

    case FSortField.FType of
      ftInteger:
        Result := EStrToInt(Value1, 0) - EStrToInt(Value2, 0);

      ftChar, ftDate, ftTime:
        Result := CompareStr(Value1, Value2);

      ftFloat, ftFixed:
        begin
          if FSortField.FType = ftFixed then
            F := StrToFloatDef(EFormatNumber(Value1, 2), 0) -
              StrToFloatDef(EFormatNumber(Value2, 2), 0)
          else
            F := StrToFloatDef(EFormatNumber(Value1, Decimals1), 0) -
              StrToFloatDef(EFormatNumber(Value2, Decimals2), 0);
          if F < 0 then Result := -1 else
            if F > 0 then Result := 1 else
              Result := 0;
        end;

      else Result := 0;
    end;
    if Result = 0 then Result := Integer(Item1) - Integer(Item2);
    if FSortDescending then Result := -Result;
  end;
end;

procedure TTableForm.DoFilter;
var
  I, NewCount: Integer;
  Value: string;
  DoScroll: Boolean;
begin
  FMapping.Clear;
  if Assigned(FFilterField) then
  begin
    case FFilterIndex of
      0:  // filter by specified value
        begin
          Value := AnsiUpperCase(FFilterValue);
          for I := 0 to FRows.Count - 1 do
            if Pos(Value, AnsiUpperCase(FRows[I, FFilterFieldIndex])) > 0 then
              FMapping.Add(Pointer(I));
        end;
      1:  // show only not empty values
        for I := 0 to FRows.Count - 1 do
        begin
          Value := Trim(FRows[I, FFilterFieldIndex]);
          if (Value <> '') and not EIsZero(Value) then FMapping.Add(Pointer(I));
        end;
      2:  // show only empty values
        for I := 0 to FRows.Count - 1 do
        begin
          Value := Trim(FRows[I, FFilterFieldIndex]);
          if (Value = '') or EIsZero(FRows[I, FFilterFieldIndex]) then FMapping.Add(Pointer(I));
        end;
    end;
  end
  else
    if Assigned(FSortField) then
      for I := 0 to FRows.Count - 1 do
        FMapping.Add(Pointer(I));

  if Assigned(FSortField) then
  begin
    CurrentTableForm := Self;
    FMapping.Sort(ListSortCompare);
  end;

  with LV, Items do
  begin
    DoScroll := (Count = 0) or (SelectedRow = Count - 1);
    if Assigned(FFilterField) or Assigned(FSortField) then NewCount := FMapping.Count
    else NewCount := FRows.Count;
    if Count <> NewCount then
    begin
      Count := NewCount;
      if DoScroll then SelectLastRec;
    end;
    Invalidate;
  end;
  Caption := Format('%s - %d/%d - %s', [FCaption, NewCount, FRows.Count, FConnectCaption]);
end;

procedure TTableForm.miFilterClick(Sender: TObject);
var
  I, W, MouseX: Integer;
begin
  inherited;
  if FFilterFieldCaption = '' then
  begin
    W := LV.Columns[0].Width;
    MouseX := LV.ScreenToClient(Mouse.CursorPos).X;
    for I := 1 to LV.Columns.Count - 1 do
    begin
      Inc(W, LV.Columns[I].Width);
      if MouseX < W then
      begin
        FFilterFieldCaption := FMsg.FOutFields[I - 1].FCaption;
        Break;
      end;
    end;
  end;

  with TFilterForm.Create(Self) do
  try
    with cbFields do
    begin
      for I := 0 to FMsg.FOutFields.Count - 1 do
        Items.Add(FMsg.FOutFields[I].FCaption);
      ItemIndex := Items.IndexOf(FFilterFieldCaption);
      if ItemIndex < 0 then ItemIndex := 0;
    end;
    BaseTable := FMsg;
    ShowPossibleFieldValues;
    SetFilterValue(FFilterValue);
    rbValue.Checked := FFilterIndex = 0;
    rbNotEmpty.Checked := FFilterIndex = 1;
    rbEmpty.Checked := FFilterIndex = 2;

    if ShowModal = mrOk then
    begin
      if rbValue.Checked then FFilterIndex := 0
      else
        if rbNotEmpty.Checked then FFilterIndex := 1
        else
          if rbEmpty.Checked then FFilterIndex := 2
          else FFilterIndex := -1;
      FFilterFieldCaption := cbFields.Text;
      FFilterFieldIndex := cbFields.ItemIndex;
      FFilterField := FMsg.FOutFields[FFilterFieldIndex];
      FFilterValue := GetFilterValue;
      DoFilter;
    end;
  finally
    Free;
  end;
end;

procedure TTableForm.miDelFilterClick(Sender: TObject);
begin
  inherited;
  FFilterField := nil;
  FFilterFieldIndex := -1;
  DoFilter;
end;

procedure TTableForm.LVDblClick(Sender: TObject);
begin
  inherited;
  miFilterClick(miFilter);
end;

procedure TTableForm.LVColumnClick(Sender: TObject; Column: TListColumn);
var
  NewSortField: TMicexField;
begin
  inherited;
  if Assigned(FSortColumn) then
    FSortColumn.Caption := FMsg.FOutFields[FSortColumn.Tag].FCaption;

  if (Column.Tag >= 0) and (Column.Tag < FMsg.FOutFields.Count) then
  begin
    NewSortField := FMsg.FOutFields[Column.Tag];
    if NewSortField = FSortField then
      if FSortDescending then
        FSortField := nil // sequence is "No sort" -> "Ascending" -> "Descending"
      else
        FSortDescending := True
    else
    begin
      FSortDescending := False;
      FSortField := NewSortField;
      FSortFieldIndex := Column.Tag;
    end;
    FSortColumn := Column;

    if Assigned(FSortColumn) and Assigned(FSortField) then
      if FSortDescending then
        FSortColumn.Caption := '^ ' + FMsg.FOutFields[FSortColumn.Tag].FCaption
      else
        FSortColumn.Caption := 'V ' + FMsg.FOutFields[FSortColumn.Tag].FCaption;

    DoFilter;
  end;
end;

procedure TMTEConnect.SetupKeepAlive;
var
  SysTimeIdx: Integer;
  Data: PMTEMsg;
begin
  if FIface.FTables.Find('SYSTIME', SysTimeIdx) then
    FSysTimeTable := MTEOpenTable(FIdx, 'SYSTIME', '', False, Data)
  else
    if FIface.FTables.Find('TESYSTIME', SysTimeIdx) then
      FSysTimeTable := MTEOpenTable(FIdx, 'TESYSTIME', '', False, Data);
end;

{ TFirmProps }

procedure TFirmProps.Add(const AFirmId, AFirmName: string);
var
  P: PFirmProp;
begin
  New(P);
  P.ShortName := AFirmName;
  inherited AddObject(AFirmId, TObject(P));
end;

procedure TFirmProps.Clear;
var
  I: Integer;
begin
  for I := 0 to Count - 1 do
    Dispose(Props[I]);
  inherited;
end;

constructor TFirmProps.Create;
begin
  inherited;
  Sorted := True;
end;

destructor TFirmProps.Destroy;
begin
  Clear;
  inherited;
end;

function TFirmProps.GetProps(AIndex: Integer): PFirmProp;
begin
  Result := PFirmProp(Objects[AIndex]);
end;

{ TUserProps }

procedure TUserProps.Add(const AUserId, AUserName, AFirmId, AFirmName: string);
var
  P: PUserProp;
begin
  New(P);
  P.UserName := AUserName;
  P.FirmId := AFirmId;
  P.FirmName := AFirmName;
  inherited AddObject(AUserId, TObject(P));
end;

procedure TUserProps.Clear;
var
  I: Integer;
begin
  for I := 0 to Count - 1 do
    Dispose(Props[I]);
  inherited;
end;

constructor TUserProps.Create;
begin
  inherited;
  Sorted := True;
end;

destructor TUserProps.Destroy;
begin
  Clear;
  inherited;
end;

function TUserProps.GetProps(AIndex: Integer): PUserProp;
begin
  Result := PUserProp(Objects[AIndex]);
end;

procedure TTableForm.miExportClick(Sender: TObject);
var
  RowCount, RowIndex, ColIndex, FldIndex, RealRow: Integer;
  T: TextFile;
  S: string;
begin
  MainForm.ExportDlg.FileName := FMsg.FName;
  if MainForm.ExportDlg.Execute then
  begin
    if Assigned(FFilterField) or Assigned(FSortField) then RowCount := FMapping.Count
    else RowCount := FRows.Count;

    AssignFile(T, MainForm.ExportDlg.FileName);
    Rewrite(T);

    // save column headers
    S := '';
    for ColIndex := 1 to LV.Columns.Count - 1 do
    begin
      FldIndex := ColIndex - 1; //LV.Columns[I].Tag;
      if S <> '' then S := S + #9;
      with FMsg.FOutFields do
        S := S + Fields[FldIndex].FName;
    end;
    Writeln(T, S);

    // save table rows
    for RowIndex := 0 to RowCount - 1 do
    begin
      if Assigned(FFilterField) or Assigned(FSortField) then RealRow := Integer(FMapping[RowIndex])
      else RealRow := RowIndex;

      S := '';
      for ColIndex := 1 to LV.Columns.Count - 1 do
      begin
        FldIndex := ColIndex - 1; //LV.Columns[I].Tag;
        if S <> '' then S := S + #9;
        with FMsg.FOutFields do
          S := S + {FormatValue(}Trim(FRows[RealRow, FldIndex]){, Fields[FldIndex], RowIndex, True)};
      end;
      Writeln(T, S);
    end;
    CloseFile(T);
  end;
end;                                    

initialization

  for I := Low(FieldSeqNums) to High(FieldSeqNums) do
    FieldSeqNums[I] := Char(I);
  {$IFDEF CLIENTLOG}
  AssignFile(DebugFile, 'TEClient.Log');
  Rewrite(DebugFile);  
  {$ENDIF}

finalization

  {$IFDEF CLIENTLOG}
  CloseFile(DebugFile);
  {$ENDIF}

end.
