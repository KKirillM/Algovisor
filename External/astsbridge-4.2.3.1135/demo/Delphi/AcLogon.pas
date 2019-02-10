unit AcLogon;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, ComCtrls, acBase, IniFiles;

type
  TLogonForm = class(TBaseForm)
    OkBtn: TButton;
    btnCancel: TButton;
    Label3: TLabel;
    Params: TRichEdit;
    btnCom: TButton;
    btnPipe: TButton;
    btnTCP: TButton;
    btnLocal: TButton;
    btnEPTS: TButton;
    lblSpecDll: TLabel;
    cbProfiles: TComboBox;
    btnTEAP: TButton;
    cbSoftConnect: TCheckBox;
    procedure FormCreate(Sender: TObject);
    procedure OkBtnClick(Sender: TObject);
    procedure btnLocalClick(Sender: TObject);
    procedure btnComClick(Sender: TObject);
    procedure btnPipeClick(Sender: TObject);
    procedure btnTCPClick(Sender: TObject);
    procedure btnEPTSClick(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure cbProfilesChange(Sender: TObject);
    procedure cbProfilesDrawItem(Control: TWinControl; AIndex: Integer;
      Rect: TRect; State: TOwnerDrawState);
    procedure ParamsKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure ParamsKeyUp(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure ParamsMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure btnTEAPClick(Sender: TObject);
  private
    FIniFile: TIniFile;
    function GetProfileCaption(const S: TStrings): string;
    procedure HighlightSyntax(RE: TRichEdit);
    function CheckProfile: Boolean;
  public
    { Public declarations }
  end;

implementation

uses
  MTEApi;

{$R *.DFM}

const
  SConnectParams = 'ConnectParams';
  SGeneral       = 'General';
  SNewProfile    = '< New profile >';
  SProfile       = 'Profile';

function TLogonForm.GetProfileCaption(const S: TStrings): string;
var
  P1, P2, User: string;
begin
  if (S.IndexOfName('SERVER') >= 0) and (S.IndexOfName('HOST') < 0) then
  begin
    P1 := S.Values['SERVER'];
    P2 := S.Values['SERVICE'];
  end
  else
    if S.IndexOfName('PORT') >= 0 then
    begin
      P1 := S.Values['PORT'];
      P2 := S.Values['BAUDRATE'];
    end
    else
      begin
        P1 := S.Values['HOST'];
        if S.IndexOfName('SERVICE') >= 0 then
          P2 := S.Values['SERVICE']
        else
          P2 := S.Values['PIPE'];
      end;
  if S.Values['USERID'] <> '' then User := ' ' + S.Values['USERID'];
  FmtStr(Result, '%s:%s%s', [P1, P2, User]);
end;

procedure TLogonForm.FormCreate(Sender: TObject);
var
  I: Integer;
  S: TStringList;
  ShowAllServerTypes: Boolean;
begin
  inherited;
  FIniFile := TIniFile.Create(LogonIniFileName);
  S := TStringList.Create;
  try
    S.Sorted := True;
    FIniFile.ReadSections(S);
    cbProfiles.Items.Assign(S);
  finally
    S.Free;
  end;
  cbProfiles.Items.Insert(0, SNewProfile);
  for I := 0 to cbProfiles.Items.Count - 1 do
    if cbProfiles.Items[I] = SGeneral then
    begin
      cbProfiles.Items.Delete(I);
      Break;
    end;
  cbProfiles.ItemIndex := cbProfiles.Items.IndexOf(FIniFile.ReadString(SGeneral, SProfile, ''));
  if cbProfiles.ItemIndex < 0 then cbProfiles.ItemIndex := 0;
  cbProfilesChange(cbProfiles);
  cbSoftConnect.Visible := msaSoftConnect in GetMTESrlAddons;

  ShowAllServerTypes := FindCmdLineSwitch('all', ['-', '/'], True);
  btnPipe.Visible := ShowAllServerTypes;
  btnTCP.Visible := ShowAllServerTypes;
  lblSpecDll.Visible := ShowAllServerTypes;
  btnLocal.Visible := ShowAllServerTypes;
  btnEpts.Visible := ShowAllServerTypes;
end;

procedure TLogonForm.OkBtnClick(Sender: TObject);
var
  I: Integer;
  Section: string;
begin
  inherited;
  FIniFile.EraseSection(cbProfiles.Text);
  Section := GetProfileCaption(Params.Lines);
  with Params.Lines do
    for I := 0 to Count - 1 do
      FIniFile.WriteString(Section, Names[I], Values[Names[I]]);
  FIniFile.WriteString(SGeneral, SProfile, Section);
end;

const
  CRLF = #13#10;

procedure TLogonForm.btnLocalClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'IFACENAME={interface name}' + CRLF +
    'EXTREFS={extref_file_name}' + CRLF +
    'NOFILTER={table1,table2,...}' + CRLF +
    'BOARDS={brd1,brd2,brd3,...}' + CRLF +
    'SERVER={trd_server}' + CRLF +
    'SERVICE={trd_service}' + CRLF +
    'USERID={userid}' + CRLF +
    'PASSWORD={password}' + CRLF +
    'TIMEOUT=0' + CRLF +
    'CONNECTTIME=0' + CRLF +
    'MAXPROCESSINGTIME=1000' + CRLF +
    'LOGGING=1';
  HighlightSyntax(Params);
end;

procedure TLogonForm.btnComClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'PORT=COM1' + CRLF +
    'BAUDRATE=115200' + CRLF +
    'TIMEOUT=30000' + CRLF +
    'SYNCTIME=1' + CRLF +
    'LOGGING=1';
  HighlightSyntax(Params);
end;

procedure TLogonForm.btnPipeClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'HOST={host_name}' + CRLF +
    'PIPE={pipe_name}' + CRLF +
    'TIMEOUT=30000' + CRLF +
    'SYNCTIME=1' + CRLF +
    'LOGGING=1';
  HighlightSyntax(Params);
end;

procedure TLogonForm.btnTCPClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'HOST={host_name_or_ip}' + CRLF +
    'SERVICE={tcp_service}' + CRLF +
    'TIMEOUT=30000' + CRLF +
    'SERVERKEY={xxxxSSSSSS}' + CRLF +
    'KEY={xxxxSSSSSSnn}' + CRLF +
    'SYNCTIME=1' + CRLF +
    'LOGGING=1';
  HighlightSyntax(Params);
end;

procedure TLogonForm.btnEPTSClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'HOST={host_ip}' + CRLF +
    'SERVICE={service_number}' + CRLF +
    'USERID={userid}' + CRLF +
    'PASSWORD={password}' + CRLF +
    'CLIENTID={client application}' + CRLF +
    'BOARDS={brd1,brd2,brd3,...}' + CRLF +
    'LOGGING=1' + CRLF +
    'PACKETSIZE=32000' + CRLF +
    'CRYPTOPROVIDER=Validata' + CRLF +
    'PROFILENAME={profilename}' + CRLF +
    'NOKEYUNLOAD=0';
  HighlightSyntax(Params);
end;

procedure TLogonForm.FormDestroy(Sender: TObject);
begin
  inherited;
  FIniFile.Free;
end;

procedure TLogonForm.cbProfilesChange(Sender: TObject);
begin
  inherited;
  FIniFile.ReadSectionValues(cbProfiles.Text, Params.Lines);
  HighlightSyntax(Params);
end;

procedure TLogonForm.cbProfilesDrawItem(Control: TWinControl;
  AIndex: Integer; Rect: TRect; State: TOwnerDrawState);
begin
  inherited;
  with cbProfiles, Canvas do
  begin
    if AIndex = 0 then Font.Style := [fsBold];
    TextRect(Rect, Rect.Left + 3, Rect.Top + 1, cbProfiles.Items[AIndex]);
  end;
end;

type
  TToken = (toEOL, toPlain, toVar, toKeyword);

function IsKeyword(const S: string): Boolean;
const
  SKeywords: array [0..28] of string =
    ('HOST', 'SERVER', 'IFACENAME', 'SERVICE', 'PORT', 'PIPE', 'BAUDRATE',
     'TIMEOUT', 'SYNCTIME', 'LOGGING', 'PACKETSIZE', 'CRYPTOPROVIDER',
     'PROFILENAME', 'USERID', 'PASSWORD', 'BOARDS', 'SERVERKEY', 'KEY',
     'CONNECTTIME', 'MAXPROCESSINGTIME', 'EXTREFS', 'NOFILTER', 'CLIENTID',
     'EPTSMRLOG', 'NOKEYUNLOAD', 'INTERFACE', 'RETRIES', 'CONNECTTIME',
     'FEEDBACK');
var
  I: Integer;
  Keyword: string;
begin
  Result := True;
  Keyword := UpperCase(S);
  for I := Low(SKeywords) to High(SKeywords) do
    if SKeywords[I] = Keyword then Exit;
  Result := False;
end;

function GetToken(const Text: string; StartFrom: Integer; out Len: Integer): TToken;
var
  I, From: Integer;
  S: string;
begin
  if StartFrom > Length(Text) then
  begin
    Result := toEOL;
    Exit;
  end;

  if Text[StartFrom] = '{' then
  begin
    I := StartFrom + 1;
    while (I <= Length(Text)) and (Text[I] <> '}') do Inc(I);
    Len := I - StartFrom + 1;
    Result := toVar;
    Exit;
  end;

  I := StartFrom;
  repeat
    while (I <= Length(Text)) and not (UpCase(Text[I]) in ['A'..'Z', '_', '{']) do Inc(I);

    if Text[I] = '{' then
    begin
      Len := I - StartFrom;
      Result := toPlain;
      Exit;
    end;

    if UpCase(Text[I]) in ['A'..'Z', '_'] then
    begin
      From := I;
      while (I <= Length(Text)) and (UpCase(Text[I]) in ['A'..'Z', '_', '0'..'9']) do Inc(I);
      S := Copy(Text, From, I - From);
      if ((From = 1) or (Text[From - 1] in [#13, #10])) and IsKeyword(S) then
      begin
        if From = StartFrom then
        begin
          Result := toKeyword;
          Len := I - From;
        end
        else
        begin
          Result := toPlain;
          Len := From - StartFrom;
        end;
        Exit;
      end;
    end
    else
    begin
      Result := toPlain;
      Len := Length(Text) - StartFrom + 1;
      Exit;
    end;
  until False;
end;

procedure TLogonForm.HighlightSyntax(RE: TRichEdit);
var
  OldSelStart, OldSelLength, StartPos, Len: Integer;
  Text: string;
  Token: TToken;
begin
  RE.Lines.BeginUpdate;
  try
    OldSelStart := RE.SelStart;
    OldSelLength := RE.SelLength;
    Text := RE.Lines.Text;
    StartPos := 1;

    repeat
      Token := GetToken(Text, StartPos, Len);
      RE.SelStart := StartPos - 1;
      RE.SelLength := Len;
      RE.SelAttributes := RE.DefAttributes;
      case Token of
        toKeyword: RE.SelAttributes.Style := [fsBold];
        toVar: RE.SelAttributes.Color := clRed;
      end;
      Inc(StartPos, Len);
    until Token = toEOL;
    
    RE.SelStart := OldSelStart;
    RE.SelLength := OldSelLength;
  finally
    RE.Lines.EndUpdate;
  end;
end;

procedure TLogonForm.ParamsKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  inherited;
  if Key = VK_ESCAPE then ModalResult := mrCancel;
end;

procedure TLogonForm.ParamsKeyUp(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  inherited;
  HighlightSyntax(Params);
end;

procedure TLogonForm.ParamsMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  inherited;
  HighlightSyntax(Params);
end;

function TLogonForm.CheckProfile: Boolean;
var
  RetVal: Integer;
begin
  Result := True;
  if cbProfiles.ItemIndex > 0 then
  begin
    RetVal := MessageDlg('Вы пытаетесь изменить существующий профиль соединения, текущие настройки будут затерты. '^M^M +
      'Может быть вместо этого Вы желаете создать новый профиль и поместить туда новые настройки?',
        mtConfirmation, mbYesNoCancel, 0);
    Result := RetVal <> mrCancel;
    if RetVal = mrYes then
      cbProfiles.ItemIndex := 0;
  end
end;

procedure TLogonForm.btnTEAPClick(Sender: TObject);
begin
  inherited;
  if not CheckProfile then Exit;
  Params.Text :=
    'HOST={ip1:port1,ip2:port2,...}' + CRLF +
    'SERVER={te_access_point_id}' + CRLF +
    'USERID={te_userid}' + CRLF +
    'PASSWORD={te_password}' + CRLF +
    'INTERFACE={te_interface_id}' + CRLF +
    'FEEDBACK={contact_info}' + CRLF +
    '#BOARDS={brd1,brd2,brd3,...}' + CRLF +
    'PROFILENAME={validata_profile_name}' + CRLF +
    '#LOGGING={0|1|2}' + CRLF +
    '#SYNCTIME={0|1}' + CRLF +
    '#TIMEOUT={read_timeout_ms}' + CRLF +
    '#RETRIES={reconnect_retry_count}' + CRLF +
    '#CONNECTTIME={max_(re)connect_time_ms}';
  HighlightSyntax(Params);
end;

end.
