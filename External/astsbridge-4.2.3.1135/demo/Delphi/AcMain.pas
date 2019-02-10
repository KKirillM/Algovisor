unit AcMain;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ToolWin, AcInput, Menus, AcStruc, StdActns, acTblFrm,
  ExtCtrls, ImgList, ActnList, ComCtrls, acBase, MTEApi, mteTypes, XPMan, StdCtrls;
                                                       
type
  TMTEConnectList = class(TList)
  private
    function GetConnect(AIndex: Integer): TMTEConnect;
  public                          
    function Add(AHandle: Integer; AIface: TMicexIface;
      const ACaption: string; ATree: TTreeView): TMTEConnect;
    procedure Clear; override;
    procedure Delete(AIndex: Integer);
    property Connects[AIndex: Integer]: TMTEConnect read GetConnect; default;
  end;

  TMainForm = class(TBaseForm)
    Actions: TActionList;
    TreeImages: TImageList;
    ObjSplitter: TSplitter;
    StatusBar: TStatusBar;
    tbMain: TToolBar;
    tb1: TToolButton;
    tb2: TToolButton;
    tb3: TToolButton;
    actConnect: TAction;
    actDisconnect: TAction;
    actRefreshTable: TAction;
    ToolButton10: TToolButton;
    MainMenu: TMainMenu;
    miFile: TMenuItem;
    Connect1: TMenuItem;
    Disconnect1: TMenuItem;
    N2: TMenuItem;
    actSessionExit: TAction;
    N3: TMenuItem;
    actAbout: TAction;
    WindowArrange: TWindowArrange;
    WindowCascade: TWindowCascade;
    WindowClose: TWindowClose;
    WindowMinimizeAll: TWindowMinimizeAll;
    WindowTileHorizontal: TWindowTileHorizontal;
    WindowTileVertical: TWindowTileVertical;
    miWindow: TMenuItem;
    Cascade1: TMenuItem;
    TileHorizontally1: TMenuItem;
    TileVertically1: TMenuItem;
    Arrange1: TMenuItem;
    MinimizeAll1: TMenuItem;
    N6: TMenuItem;
    Close1: TMenuItem;
    WindowCloseAll: TAction;
    N7: TMenuItem;
    miAction: TMenuItem;
    OpenTable1: TMenuItem;
    RefreshTable1: TMenuItem;
    tbRefresh: TToolButton;
    RefreshTimer: TTimer;
    N10: TMenuItem;
    miCompleteOpen: TMenuItem;
    miFreeBuf: TMenuItem;
    actSnapshot: TAction;
    N4: TMenuItem;
    actRestore: TAction;
    Snapshot1: TMenuItem;
    CoolBar1: TCoolBar;
    miHelp: TMenuItem;
    N1: TMenuItem;
    BigCmd: TImageList;
    BigCmdGray: TImageList;
    actExecute: TAction;
    pmRefreshRates: TPopupMenu;
    N5: TMenuItem;
    miOneSecond: TMenuItem;
    N32: TMenuItem;
    mi5sec: TMenuItem;
    N102: TMenuItem;
    N302: TMenuItem;
    N303: TMenuItem;
    Bevel1: TBevel;
    actGetExtData: TAction;
    N8: TMenuItem;
    SaveDlg: TSaveDialog;
    miSendIp: TMenuItem;
    KeepAliveTimer: TTimer;
    actExport: TAction;
    SaveHtmlDlg: TSaveDialog;
    N9: TMenuItem;
    BasePanel: TPanel;
    SessionPanel: TPanel;
    cbSessions: TComboBox;
    N11: TMenuItem;
    miShowIds: TMenuItem;
    miShowNames: TMenuItem;
    miShowAll: TMenuItem;
    XPManifest1: TXPManifest;
    ExportDlg: TSaveDialog;
    actDllVer: TAction;
    MTESRLDLL1: TMenuItem;
    N12: TMenuItem;
    actShowCert: TAction;
    N13: TMenuItem;
    actServInfo: TAction;
    N14: TMenuItem;
    actLogon: TAction;
    N15: TMenuItem;
    N16: TMenuItem;
    N17: TMenuItem;
    procedure actRefreshTableExecute(Sender: TObject);
    procedure actDisconnectExecute(Sender: TObject);
    procedure actConnectExecute(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure WindowCloseAllExecute(Sender: TObject);
    procedure actAboutExecute(Sender: TObject);
    procedure RefreshTimerTimer(Sender: TObject);
    procedure miActionClick(Sender: TObject);
    procedure miCompleteOpenClick(Sender: TObject);
    procedure miFreeBufClick(Sender: TObject);
    procedure actSnapshotExecute(Sender: TObject);
    procedure actRestoreExecute(Sender: TObject);
    procedure actSessionExitExecute(Sender: TObject);
    procedure ActionsUpdate(Action: TBasicAction;
      var Handled: Boolean);
    procedure FormCreate(Sender: TObject);
    procedure actExecuteExecute(Sender: TObject);
    procedure RefreshRateClick(Sender: TObject);
    procedure pmRefreshRatesPopup(Sender: TObject);
    procedure ObjTreeChange(Sender: TObject; Node: TTreeNode);
    procedure ObjTreeDblClick(Sender: TObject);
    procedure actGetExtDataExecute(Sender: TObject);
    procedure miSendIpClick(Sender: TObject);
    procedure KeepAliveTimerTimer(Sender: TObject);
    procedure actExportExecute(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure SessionChange(Sender: TObject);
    procedure SessionPanelResize(Sender: TObject);
    procedure cbSessionsDrawItem(Control: TWinControl; AIndex: Integer;
      Rect: TRect; State: TOwnerDrawState);
    procedure miShowAllClick(Sender: TObject);
    procedure actDllVerExecute(Sender: TObject);
    procedure StatusBarDblClick(Sender: TObject);
    procedure actShowCertExecute(Sender: TObject);
    procedure actServInfoExecute(Sender: TObject);
    procedure actLogonExecute(Sender: TObject);
  private
    FCompleteOpen: Boolean;
    FFreeBuffer: Boolean;
    FSendClientIp: Boolean;
    FConnects: TMTEConnectList;
    FTreeButtonClick: Boolean;
    FTreeButtonClickTime,
    FLastRequestTime: DWORD;
    FModifyTime: Boolean;
    procedure FreeBuffer(Connect: TMTEConnect);
    procedure SaveRequestParams(Connect: TMTEConnect; Msg: TMicexMessage; const Params: string);
    procedure PlantTrees(Connect: TMTEConnect);
    procedure ShowTreeHint(Node: TTreeNode);
    procedure SaveExtData(Data: PMTEMsg; const FileName: string);
    function GetActiveConnect: TMTEConnect;
    procedure CloseWindows(Connect: TMTEConnect);
    procedure ShowStats;
    procedure TreeViewExpanding(Sender: TObject; Node: TTreeNode;
      var AllowExpansion: Boolean);
    procedure TreeViewMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure InvalidateWindows;
    function ConnectToServer(const Params: string; UseSoftConnect: Boolean): TMTEConnect;
    procedure ShowCertificates;
    function DownloadStructure(Idx: Integer; AConnect: TMTEConnect): TMicexIface;
    procedure DownloadDictionary(Connect: TMTECOnnect);
    procedure Logon;
    function IsCmdSwitch(const S: string): Boolean;
  public
    procedure OpenTable(Connect: TMTEConnect; Tbl: TMicexMessage; FromMsg: PBaseMsgRec);
    procedure ExecuteTransaction(Connect: TMTEConnect; Msg: TMicexMessage; FromMsg: PBaseMsgRec);
    procedure CutTrees(Connect: TMTEConnect);
    procedure ShowStatus(const Msg: string);
    procedure ValidateConnect(Connect: TMTEConnect; ErrorCode: Integer = MTE_OK);
  end;

var
  MainForm: TMainForm;

implementation

uses
  AcLogon, ShellApi, IniFiles, acExport, ExtUtils, acCertInfo;

{$R *.DFM}

const
  ntTables       = 0;
  ntTransactions = 1;
  ntTable        = 2;
  ntTransaction  = 3;
  ntInFields     = 4;
  ntOutFields    = 5;
  ntInField      = 6;
  ntOutField     = 7;
  ntEnumTypes    = 8;
  ntEnumType     = 9;
  ntIface        = 10;
  ntUpdateTable  = 20;

resourcestring
  rsConnecting        = 'Connecting to server...';
  rsLoadingStructure  = 'Loading market definition...';
  rsLoadingSecs       = 'Loading SECURITIES table...';
  rsLoadingFirms      = 'Loading FIRMS table...';

{ Common routines }

{ TMainForm }

procedure TMainForm.FreeBuffer(Connect: TMTEConnect);
begin
  if FFreeBuffer then MTEFreeBuffer(Connect.FIdx);
end;

procedure TMainForm.SaveRequestParams(Connect: TMTEConnect; Msg: TMicexMessage; const Params: string);
const
  Prefixes: array [Boolean] of string = ('trn_', 'tbl_');
var
  I: Integer;
  PlainParams: string;
begin
  with TIniFile.Create(IniFileName) do
  try
    PlainParams := '';
    for I := 1 to Length(Params) do
      case Params[I] of
        #0..#31: PlainParams := PlainParams + '%' + IntToHex(Byte(Params[I]), 2);
        '%': PlainParams := PlainParams + '%%';
        else PlainParams := PlainParams + Params[I];
      end;

    WriteString('ifc_' + Connect.FIface.FName, Prefixes[Msg.FIsTable] + Msg.FName,
      AnsiQuotedStr(PlainParams, '"'));
  finally
    Free;
  end;
end;

procedure TMainForm.ExecuteTransaction(Connect: TMTEConnect; Msg: TMicexMessage; FromMsg: PBaseMsgRec);
var
  Params: string;
  ErrMsg: TMTEErrorMsg;
  Err: Integer;
begin
  with Msg do
    if GetInputParams(Msg, DisplayName, FInFields.FList, Connect.FIface, Params, FromMsg, Connect.FSecList) then
  begin
    SaveRequestParams(Connect, Msg, Params);
    FLastRequestTime := GetTickCount;
    if FSendClientIp then
      Err := MTEExecTransIP(Connect.FIdx, PChar(Msg.FName), PChar(Params), @ErrMsg, $0100007f)
    else
      Err := MTEExecTrans(Connect.FIdx, PChar(Msg.FName), PChar(Params), @ErrMsg);
    ValidateConnect(Connect, Err);
    ShowError(Connect.FCaption, Err, ErrMsg, Format('MTEExecTrans(%s)', [Msg.FName]));
    FreeBuffer(Connect);
  end;
end;

procedure TMainForm.OpenTable(Connect: TMTEConnect; Tbl: TMicexMessage; FromMsg: PBaseMsgRec);
var
  Params: string;
  Err, Index: Integer;
  Data: PMTEMsg;
begin
  if GetInputParams(Tbl, Tbl.DisplayName, Tbl.FInFields.FList, Connect.FIface, Params, FromMsg, Connect.FSecList) then
  begin
    SaveRequestParams(Connect, Tbl, Params);
    Screen.Cursor := crHourGlass;
    FLastRequestTime := GetTickCount;
    Err := MTEOpenTable(Connect.FIdx, PChar(Tbl.FName), PChar(Params), FCompleteOpen, Data);
    ShowStatus(Format('MTEOpenTable(%s): %d ms', [Tbl.FName, GetTickCount - FLastRequestTime]));
    if Err < MTE_OK then
    begin
      ValidateConnect(Connect, Err);
      ShowError(Connect.FCaption, Err, PChar(GetTsmrStr(Err, Data)), Format('MTEOpenTable(%s)', [Tbl.FName]));
    end
    else
      with TTableForm.Create(Self) do
      begin
        if Trim(Params) <> '' then
        begin
          if Tbl.FIsOrderbook or (Tbl.FName = 'ORDERBOOK') or (Tbl.FName = 'QUOTEBOOK') then
            if Connect.FSecList.Find(Params, Index) then
              with Connect.FSecList[Index]^ do
                FmtStr(Params, '%s (%s)', [ShortName, Trim(SecCode)]);

          Caption := Tbl.DisplayName + ' - ' + Params
        end
        else
          Caption := Tbl.DisplayName;
        if Assigned(Connect) then
          SetupStatics(Connect, Tbl, @Data.Data, Err, Connect.FSecList, Connect.FCaption);
      end;
    FreeBuffer(Connect);
    ShowStats;
    Screen.Cursor := crDefault;
  end;
end;

procedure TMainForm.actRefreshTableExecute(Sender: TObject);
begin
  RefreshTimerTimer(Self);
end;

procedure TMainForm.PlantTrees(Connect: TMTEConnect);

  function AddNode(Root: TTreeNode; const ACaption: string; Obj: TObject; Image: Integer): TTreeNode;
  begin
    Result := Connect.FTree.Items.AddChildObject(Root, ACaption, Obj);
    Result.ImageIndex := Image;
    Result.SelectedIndex := Image;
  end;

  procedure PlantFields(Root: TTreeNode; Fields: TMicexFields);
  var
    I, Idx, Image: Integer;
  begin
    if Fields.FIsInput then Idx := ntInField else Idx := ntOutField;
    for I := 0 to Fields.Count - 1 do
    begin
      if Fields[I].FKey then Image := 14 else Image := Idx;
      AddNode(Root, Fields[I].DisplayName, Fields[I], Image);
    end;
  end;

  procedure PlantMessages(Root: TTreeNode; Messages: TMicexMessages);
  var
    I, Idx: Integer;
    Node: TTreeNode;
  begin                              
    for I := 0 to Messages.Count - 1 do
    begin
      if Messages.FIsTable then
        if Messages[I].FUpdateable then Idx := ntUpdateTable
        else Idx := ntTable
      else Idx := ntTransaction;

      Node := AddNode(Root, Messages[I].DisplayName, Messages[I], Idx);
      with Messages[I] do
      begin
        if FInFields.Count > 0 then
          PlantFields(AddNode(Node, 'Запрос', FInFields, ntInFields), FInFields);
        if FIsTable then
          PlantFields(AddNode(Node, 'Ответ', FOutFields, ntOutFields), FOutFields);
      end;
    end;
  end;

var
  Node: TTreeNode;              
  I: Integer;
begin
  with Connect.FTree.Items do
  begin
    BeginUpdate;
    Clear;
    EndUpdate;
  end;

  if Assigned(Connect.FIface) then
    with Connect.FTree, Connect.FIface do
    begin
      Node := AddNode(nil, 'Перечислимые типы', FTypes, ntEnumTypes);
      for I := 0 to FTypes.Count - 1 do
        AddNode(Node, FTypes[I].DisplayName, FTypes[I], ntEnumType);

      PlantMessages(AddNode(nil, 'Таблицы', FTables, ntTables), FTables);
      PlantMessages(AddNode(nil, 'Транзакции', FTransactions, ntTransactions), FTransactions);
    end
  else
    AddNode(nil, 'Not logged on', nil, -1);
end;

procedure TMainForm.CutTrees(Connect: TMTEConnect);
begin
{  with Connect.FTree.Items do
  begin
    BeginUpdate;
    Clear;
    EndUpdate;
  end;}
  ShowStatus('');
end;

procedure TMainForm.CloseWindows(Connect: TMTEConnect);
var
  I: Integer;
begin
  for I := 0 to MDIChildCount - 1 do
    if MDIChildren[I] is TTableForm then
      with TTableForm(MDIChildren[I]) do
        if (Connect.FIdx = ConnectIdx) then Close;
end;

procedure TMainForm.actDisconnectExecute(Sender: TObject);
var
  Connect: TMTEConnect;
  Panel: TPanel;
  OldIndex: Integer;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
//    RefreshTimer.Enabled := False;
    Panel := TPanel(cbSessions.Items.Objects[cbSessions.ItemIndex]);
    CloseWindows(Connect);
    FConnects.Remove(Connect);

    if Assigned(Panel) then
    begin
      if cbSessions.Items.Count = 1 then
      begin
        BasePanel.Visible := False;
        ObjSplitter.Visible := False;
      end;
      Panel.Free;
      with cbSessions do
      begin
        OldIndex := ItemIndex;
        Items.Delete(OldIndex);
        if OldIndex >= Items.Count then OldIndex := Items.Count - 1;
        ItemIndex := OldIndex;
      end;
      SessionChange(cbSessions);
    end;
    Connect.Free;
  end;
end;

function GetIfaceCaption(Iface: TMicexIface; const Params: string): string;
var
  S: TStringList;
  P1, P2, IfaceCaption, IfaceName: string;
begin
  if Params = '' then Result := 'New Iface'
  else
  begin
    S := TStringList.Create;
    try
      S.Text := Params;
      if S.IndexOfName('SERVER') >= 0 then
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

      if Assigned(Iface) then
      begin
        IfaceName := Iface.FName;
        IfaceCaption := Iface.FCaption;
      end
      else
      begin
        IfaceName := 'Not logged on';
        IfaceCaption := '';
      end;
      if IfaceCaption <> '' then IfaceCaption := ' - ' + IfaceCaption;
      if S.Values['USERID'] <> '' then
        Result := Format('%s [%s%s]', [S.Values['USERID'], IfaceName, IfaceCaption])
      else
        Result := Format('%s:%s [%s%s]', [P1, P2, IfaceName, IfaceCaption]);
    finally
      S.Free;
    end;
  end;
end;

function TMainForm.DownloadStructure(Idx: Integer; AConnect: TMTEConnect): TMicexIface;
var
  Data: PMTEMsg;
  Err: Integer;
  StructErrMsg: string;
  Buf: PChar;
begin
  Result := nil;
  ShowStatus(rsLoadingStructure);
  Data := nil;
  Err := MTEStructure(Idx, Data);
  if Err <> MTE_OK then
  begin
    StructErrMsg := GetTsmrStr(Err, Data);
    if Assigned(AConnect) then AConnect.FDisconnected := True
    else MTEDisconnect(Idx);
    cbSessions.Invalidate;
    ShowError('', Err, PChar(StructErrMsg), 'MTEStructure');
    Exit;
  end
  else
    begin
      Result := TMicexIface.Create;
      Buf := @Data.Data;
      Result.LoadFromBuf(Buf);
    end;
end;

procedure TMainForm.DownloadDictionary(Connect: TMTECOnnect);
begin
  ShowStatus(rsLoadingSecs);
  DownloadSecurities(Connect);
  ShowStatus(rsLoadingFirms);
  DownloadFirms(Connect);
end;

function TMainForm.ConnectToServer(const Params: string; UseSoftConnect: Boolean): TMTEConnect;
var
//  Err: Integer;
  ErrMsg: TMTEErrorMsg;
  Idx: Integer;
  Iface: TMicexIface;
  ACaption, VerStr: string;
  Tree: TTreeView;
  Panel: TPanel;
begin
  Result := nil;
  Iface := nil;
  Screen.Cursor := crHourGlass;
  ShowStatus(rsConnecting);
  try
    ErrMsg := '';
    if UseSoftConnect then
      Idx := MTESoftConnect(PChar(Params), @ErrMsg)
    else
      Idx := MTEConnect(PChar(Params), @ErrMsg);

    if Idx < MTE_OK then
    begin
      MessageDlg(Format('Ошибка %d: %s'#13#10#13#10'%s',
        [Idx, MTEErrorMsg(Idx), ErrMsg]), mtWarning, [mbOk], 0);
      Exit;
    end
    else
    begin
      if not UseSoftConnect then
      begin
        Iface := DownloadStructure(Idx, nil);
        if not Assigned(Iface) then Exit;
      end;
      ACaption := GetIfaceCaption(Iface, Params);

      Panel := TPanel.Create(Self);
      Panel.Align := alClient;
      Panel.BevelOuter := bvNone;
      Panel.Parent := BasePanel;
      Panel.Caption := ACaption;

      Tree := TTreeView.Create(Panel);
      Tree.Parent := Panel;
      Tree.Align := alClient;
      Tree.Images := TreeImages;
      Tree.HideSelection := False;
      Tree.ReadOnly := True;
      Tree.OnChange := ObjTreeChange;
      Tree.OnDblClick := ObjTreeDblClick;
      Tree.OnExpanding := TreeViewExpanding;
      Tree.OnCollapsing := TreeViewExpanding;
      Tree.OnMouseDown := TreeViewMouseDown;

      Result := FConnects.Add(Idx, Iface, ACaption, Tree);
      Result.FLoggedon := not UseSoftConnect;
      Result.FParams := Params;

      if msaGetVersion in GetMteSrlAddons then
      begin
        VerStr := MTEGetVersion;
        // for MICEX MTESRL.DLL we can open dictionary tables with parameter Complete = False (for speed)
        // fo CMA MTESRL.DLL we must open dictionary table with Complete = True, otherwise we'll get only first 30K of each table
        if Pos('MICEX', UpperCase(VerStr)) > 0 then Result.FInitialOpenMode := False;
      end;

      Tree.Tag := Integer(Result);
      cbSessions.AddItem(ACaption, Panel);
      cbSessions.ItemIndex := cbSessions.Items.IndexOf(ACaption);
      PlantTrees(Result);
  //        RefreshTimer.Enabled := True;
      BasePanel.Visible := True;
      ObjSplitter.Visible := True;
      ObjSplitter.Left := MaxInt;

      if not UseSoftConnect then
        DownloadDictionary(Result);
    //        UpdateCaption(Params.Text);
      SessionChange(cbSessions);
      FreeBuffer(Result);
      ShowStats;
    end;
  finally
    ShowStatus('');
    Screen.Cursor := crDefault;
  end;
end;

procedure TMainForm.actConnectExecute(Sender: TObject);
var
  Connect: TMTEConnect;
  SyncTime: string;
  Stats: TMTEConnStats;
begin
  with TLogonForm.Create(Self) do
  try
    if ShowModal = mrOk then
    begin
      SyncTime := UpperCase(Params.Lines.Values['SYNCTIME']);
      FModifyTime := not ((SyncTime = '0') or (SyncTime = 'N') or (SyncTime = 'F'));
      Connect := ConnectToServer(Params.Text, cbSoftConnect.Checked);

      if Assigned(Connect) and ([msaConnStats, msaConnCerts] <= GetMTESrlAddons) then
      begin
        FillChar(Stats, SizeOf(Stats), 0);
        Stats.Size := SizeOf(Stats);
        if (MTEConnectionStats(Connect.FIdx, Stats) = MTE_OK)
          and (Stats.Properties and (FLAG_ENCRYPTED or FLAG_SIGNING_ON) <> 0) then
            ShowCertificates;
      end;
    end;
  finally
    Free;
  end;
end;

procedure TMainForm.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  while cbSessions.Items.Count > 0 do
    actDisconnectExecute(Self);
end;

procedure TMainForm.WindowCloseAllExecute(Sender: TObject);
var
  Index: Integer;
begin
  { Must be done backwards through the MDIChildren array }
  for Index := MDIChildCount - 1 downto 0 do MDIChildren[Index].Close;
end;

procedure TMainForm.actAboutExecute(Sender: TObject);
begin
  ShellAbout(Handle, 'TE Client 2',
    'MICEX TradeEngine Interface Client'^M'Copyright (c) 1999-2001 MICEX', Icon.Handle);
end;

function GetConnectPropsStr(Props: DWORD; Reconnects: Integer): string;
begin
  if Props and ZLIB_COMPRESSED <> 0 then Result := 'СЖАТ' else Result := '';
  if Props and FLAG_ENCRYPTED <> 0 then
  begin
    if Result <> '' then Result := Result + ', ';
    Result := Result + 'ШИФР';
  end;
  if Props and FLAG_SIGNING_ON <> 0 then
  begin
    if Result <> '' then Result := Result + ', ';
    Result := Result + 'ЭЦП';
  end;
  if Reconnects > 0 then
  begin
    if Result <> '' then Result := Result + ', ';
    Result := Result + Format('%d обрывов связи', [Reconnects]);
  end;
end;

procedure TMainForm.ShowStats;
var
  C: TMTEConnect;
  Stats: TMTEConnStats;
begin
  C := GetActiveConnect;
  if Assigned(C) then
  begin
    FillChar(Stats, SizeOf(Stats), 0);
    Stats.Size := SizeOf(Stats);
    if MTEConnectionStats(C.FIdx, Stats) = MTE_OK then
    begin
      StatusBar.Panels[1].Text := Format('-> %d / %d (%d pkts)', [Stats.SentBytes, Stats.SentUncompressed, Stats.SentPackets]);
      StatusBar.Panels[2].Text := Format('<- %d / %d (%d pkts)', [Stats.RecvBytes, Stats.RecvUncompressed, Stats.RecvPackets]);
      StatusBar.Panels[3].Text := Format('%d Kb in %d pcs', [AllocMemSize div 1024, AllocMemCount]);
      if Stats.ServerIpAddress <> 0 then
        StatusBar.Panels[4].Text := Einet_ntoa(Stats.ServerIpAddress);
      StatusBar.Panels[5].Text := GetConnectPropsStr(Stats.Properties, Stats.ReconnectCount);
    end;
  end;
end;

function Min(A, B: Integer): Integer;
begin
  if A < B then Result := A else Result := B;
end;

procedure TMainForm.RefreshTimerTimer(Sender: TObject);
var
  I, Err, RefCount, Cidx: Integer;
  Msg: PMTEMsg;
  Tbl: PMTETable;
  Form: TForm;
  Connect: TMTEConnect;
begin
  for Cidx := 0 to FConnects.Count - 1 do
  begin
    Connect := FConnects[Cidx];
    if Connect.FDisconnected then Continue; // connection was lost - do not update tables
    RefCount := 0;

    for I := 0 to MDIChildCount - 1 do
      if MDIChildren[I] is TTableForm then
        with TTableForm(MDIChildren[I]) do
          if (Connect.FIdx = ConnectIdx) and Refreshable then
          begin
            {$IFDEF CLIENTLOG}
            Writeln(DebugFile, 'Requesting ' + Caption);
            {$ENDIF}
            Err := AddToPacket(ConnectIdx);
            if Err = MTE_OK then Inc(RefCount)
            else
            begin
              ValidateConnect(Connect, Err);
              ShowError(Connect.FCaption, Err, '', 'MTEAddTable');
              Break;
            end;
          end;

    if RefCount > 0 then
    begin
      FLastRequestTime := GetTickCount;
      Err := MTERefresh(Connect.FIdx, Msg);
      if Err <> MTE_OK then
      begin
        ValidateConnect(Connect, Err);
        ShowError(Connect.FCaption, Err, PChar(GetTsmrStr(Err, Msg)), 'MTERefresh')
      end
      else
      begin
        // Cycle throu all refresh groups
        RefCount := PMTETables(@Msg.Data).TblCount;

        {$IFDEF CLIENTLOG}
        Writeln(DebugFile, Format('%d table(s) refreshed', [RefCount]));
        Flush(DebugFile);
        {$ENDIF}

        if RefCount > 0 then
        begin
          Tbl := @PMTETables(@Msg.Data).Tables;
          for I := 0 to RefCount - 1 do
          begin
            Form := TForm(Tbl.Ref);
            TTableForm(Form).ParseData(Tbl);
          end;
        end;
      end;
      FreeBuffer(Connect);
    end;
  end;
  ShowStats;
end;

procedure TMainForm.miActionClick(Sender: TObject);
begin
  miCompleteOpen.Checked := FCompleteOpen;
  miFreeBuf.Checked := FFreeBuffer;
  miSendIp.Checked := FSendClientIp;
end;

procedure TMainForm.miCompleteOpenClick(Sender: TObject);
begin
  FCompleteOpen := not FCompleteOpen;
end;

procedure TMainForm.miFreeBufClick(Sender: TObject);
begin
  FFreeBuffer := not FFreeBuffer;
end;

procedure TMainForm.actSnapshotExecute(Sender: TObject);
var
  Snapshot: PByte;
  Err, Len: Integer;
  S: string;
  Connect: TMTEConnect;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    Err := MTEGetSnapshot(Connect.FIdx, Snapshot, Len);
    ValidateConnect(Connect, Err);
    case Err of
      MTE_OK:
        begin
          with TFileStream.Create('Snapshot.bin', fmCreate) do
          try
            WriteBuffer(Snapshot^, Len);
          finally
            Free;
          end;
          MessageDlg('Snapshot saved in ".\Snapshot.bin"', mtInformation, [mbOk], 0);
        end;
      MTE_TSMR:
        begin
          SetString(S, PAnsiChar(Snapshot), Len);
          MessageDlg(S, mtError, [mbOk], 0);
        end;
      else MessageDlg(MTEErrorMsg(Err), mtError, [mbOk], 0);
    end;
    FreeBuffer(Connect);
  end;
end;

procedure TMainForm.actRestoreExecute(Sender: TObject);
var
  Snapshot: PByte;
  Err, Len: Integer;
  ErrMsg: TMTEErrorMsg;
  Connect: TMTEConnect;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    with TFileStream.Create('Snapshot.bin', fmOpenRead) do
    try
      Len := Size;
      GetMem(Snapshot, Len);
      try
        ReadBuffer(Snapshot^, Len);
        Err := MTESetSnapshot(Connect.FIdx, Snapshot, Len, @ErrMsg);
        ValidateConnect(Connect, Err);
        ShowError(Connect.FCaption, Err, ErrMsg, 'MTESetSnapshot');
      finally
        FreeMem(Snapshot);
      end;
    finally
      Free;
    end;
  end;
end;

procedure TMainForm.actSessionExitExecute(Sender: TObject);
begin
  Close;
end;

procedure TMainForm.ActionsUpdate(Action: TBasicAction;
  var Handled: Boolean);
var
  Obj: TObject;
  Connect: TMTEConnect;
  Idx: Integer;
begin
  inherited;
  Connect := GetActiveConnect;
  if Assigned(Connect) and Assigned(Connect.FTree.Selected) then
    Obj := TObject(Connect.FTree.Selected.Data) else Obj := nil;
  if Assigned(Connect) then Idx := Connect.FIdx else Idx := -1;

//  actConnect.Enabled := Idx < 0;
  actDisconnect.Enabled := Idx >= 0;
  WindowCloseAll.Enabled := MDIChildCount > 0;
  actRefreshTable.Enabled := (MDIChildCount > 0) and not Connect.FDisconnected and Connect.FLoggedon
    and (MDIChildren[0] is TTableForm) and TTableForm(MDIChildren[0]).Refreshable;
  actExecute.Enabled := (Idx >= 0) and (Obj is TMicexMessage) and not Connect.FDisconnected and Connect.FLoggedon;
  actGetExtData.Enabled := (Idx >= 0) and (msaGetExtData in GetMTESrlAddOns) and not Connect.FDisconnected and Connect.FLoggedon;

  actSnapshot.Enabled := (Idx >= 0) and not Connect.FDisconnected and Connect.FLoggedon;
  actRestore.Enabled := (Idx >= 0) and not Connect.FDisconnected and Connect.FLoggedon;
  actExport.Enabled := (Idx >= 0) and Connect.FLoggedon;

  miSendIp.Enabled := msaExecTransIP in GetMTESrlAddOns;
  miShowIds.Checked := IdViewMode = ivmIds;
  miShowNames.Checked := IdViewMode = ivmNames;
  miShowAll.Checked := IdViewMode = ivmAll;

  actDllver.Enabled := msaGetVersion in GetMTESrlAddons;
  actShowCert.Enabled := (Idx >= 0) and ([msaConnStats, msaConnCerts] <= GetMTESrlAddons);
  actServInfo.Enabled := (Idx >= 0) and (msaGetServInfo in GetMTESrlAddons);
  actLogon.Enabled := (Idx >= 0) and not Connect.FDisconnected and (msaSoftConnect in GetMTESrlAddons);
end;

procedure TMainForm.FormCreate(Sender: TObject);
begin
  inherited;
  FConnects := TMTEConnectList.Create; 
  RefreshRateClick(mi5sec);
  with TIniFile.Create(IniFileName) do
  try
    IdViewMode := TIdViewMode(ReadInteger('General', 'IdViewMode', 1));
  finally
    Free;
  end;
end;

function TMainForm.GetActiveConnect: TMTEConnect;
begin
  if (cbSessions.Items.Count > 0) and (cbSessions.ItemIndex >= 0) then
    Result := TMTEConnect(TPanel(cbSessions.Items.Objects[cbSessions.ItemIndex]).Components[0].Tag)
  else
    Result := nil;
end;

procedure TMainForm.actExecuteExecute(Sender: TObject);
var
  Connect: TMTEConnect;
  Msg: TMicexMessage;
begin
  inherited;
  Connect := GetActiveConnect;
  if Assigned(Connect) then
    if Assigned(Connect.FTree.Selected) and (TObject(Connect.FTree.Selected.Data) is TMicexMessage) then
    begin
      Msg := TMicexMessage(Connect.FTree.Selected.Data);
      if Msg.FIsTable then OpenTable(Connect, Msg, nil)
      else ExecuteTransaction(Connect, Msg, nil);
    end;
end;

procedure TMainForm.RefreshRateClick(Sender: TObject);
begin
  inherited;
  RefreshTimer.Interval := (Sender as TComponent).Tag * 1000;
  if (Sender as TComponent).Tag <> 0 then
    tbRefresh.Caption := (Sender as TMenuItem).Caption
  else
    tbRefresh.Caption := 'Обновить';
end;

procedure TMainForm.pmRefreshRatesPopup(Sender: TObject);
var
  I: Integer;
begin
  inherited;
  with (Sender as TPopupMenu) do
    for I := 0 to Items.Count - 1 do
      with Items[I] do
        Checked := Longword(Tag * 1000) = RefreshTimer.Interval;
end;

procedure TMainForm.ShowStatus(const Msg: string);
begin
  with StatusBar do
  begin
    if (Msg <> '') or (Copy(Panels[0].Text, 1, 7) <> 'Warning') then
      Panels[0].Text := Msg;

    if Msg = '' then
    begin
      Panels[1].Text := '';
      Panels[2].Text := '';
      Panels[3].Text := '';
      Panels[4].Text := '';
      Panels[5].Text := '';
    end;
    Repaint;
  end;
end;

procedure TMainForm.ShowTreeHint(Node: TTreeNode);

  function GetDecimals(Field: TMicexField): string;
  begin
    if Field.FType = ftFixed then Result := '.' + IntToStr(Field.FDecimals)
    else Result := '';
  end;

  function GetProp(const Name, Value: string): string;
  begin
    if Value <> '' then FmtStr(Result, ', %s=''%s''', [Name, Value])
    else Result := '';
  end;

const
  STypes: array [TTEFieldType] of string =
    ('Char', 'Integer', 'Fixed', 'Price', 'Date', 'Time', 'Float');
  SBools: array [Boolean] of PChar = ('Нет', 'Да');
  sTheKey: array [Boolean] of PChar = ('', ', Ключ');
  sSecCode: array [Boolean] of PChar = ('', ', КодИнстр.');
  sNotNull: array [Boolean] of PChar = ('', ', НеПусто');
  sVarBlock: array [Boolean] of PChar = ('', ', Повтор');
var
  Obj: TMicexObject;
begin
  if Assigned(Node) then
  begin
    Obj := TObject(Node.Data) as TMicexObject;
    if Assigned(Obj) then
    begin
      if Obj is TMicexField then
        with TMicexField(Obj) do
          ShowStatus(
            Format('%s: %s(%d%s)%s%s%s%s%s%s',
              [Obj.FName, STypes[FType], FSize, GetDecimals(TMicexField(Obj)),
              sTheKey[FKey], sSecCode[FSecCode], sNotNull[FFlags and ffNotNull <> 0],
              sVarBlock[FFlags and ffVarBlock <> 0], GetProp('Enum', FEnumType), GetProp('Default', FDefValue)]))
      else
        if Obj is TMicexMessage then
          with TMicexMessage(Obj) do
        begin
          if FIsTable then
            ShowStatus(
              Format('%s, Обновлять = %s, Очищать = %s', [FName, SBools[FUpdateable],
                SBools[FClearOnUpdate]]))
          else
            ShowStatus(FName);
        end
        else
          if Obj is TMicexEnumType then
            with TMicexEnumType(Obj) do
          begin
            ShowStatus(Format('%s(%d): %s', [Obj.FName, FSize, GetAsText]))
          end
          else
            if Obj is TMicexList then
              ShowStatus(Format('Объектов: %d', [TMicexList(Obj).Count]))
            else
              ShowStatus(Format('%s', [Obj.FName]))
    end
    else ShowStatus('');
  end
  else ShowStatus('');
end;

procedure TMainForm.ObjTreeChange(Sender: TObject; Node: TTreeNode);
begin
  inherited;
  ShowTreeHint(Node);
end;

procedure TMainForm.ObjTreeDblClick(Sender: TObject);
var
  Connect: TMTEConnect;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) and not Connect.FLoggedon then Logon
  else
    if actExecute.Enabled then actExecuteExecute(nil);
end;

procedure TMainForm.SaveExtData(Data: PMTEMsg; const FileName: string);
var
  F: TFileStream;
begin
  F := TFileStream.Create(FileName, fmCreate);
  try
    F.WriteBuffer(Data.Data, Data.DataLen);
  finally
    F.Free;
  end;
end;

procedure TMainForm.actGetExtDataExecute(Sender: TObject);
var
  Data: PMTEMsg;
  Err, I: Integer;
  Value, DataSet, FileName: string;
  Connect: TMTEConnect;
begin
  inherited;
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    with TIniFile.Create(IniFileName) do
    try
      Value := ReadString('General', 'GetExtData', 'DataSet\FileName');
    finally
      Free;
    end;

    if InputQuery('Загрузка файла из ТС',
      'Введите имя набора данных и файла:', Value) then
    begin
      I := LastDelimiter('\/:?@,.', Value);
      if I <= 0 then
      begin
        MessageDlg('Вы неверно задали имя набора данных\файла',
          mtInformation, [mbOk], 0);
        Exit;
      end;

      with TIniFile.Create(IniFileName) do
      try
        WriteString('General', 'GetExtData', Value);
      finally
        Free;
      end;

      DataSet := Copy(Value, 1, I - 1);
      FileName := Copy(Value, I + 1, MaxInt);
      Err := MTEGetExtData(Connect.FIdx, PChar(DataSet), PChar(FileName), Data);
      if Err <> MTE_OK then
      begin
        ValidateConnect(Connect, Err);
        ShowError(Connect.FCaption, Err, PChar(GetTsmrStr(Err, Data)), 'MTEGetExtData')
      end
      else
        if SaveDlg.Execute then SaveExtData(Data, SaveDlg.FileName);
    end;
  end;
end;

procedure TMainForm.miSendIpClick(Sender: TObject);
begin
  FSendClientIp := not FSendClientIp;
end;

procedure TMainForm.KeepAliveTimerTimer(Sender: TObject);
var
  Msg: PMTEMsg;
  I, Err: Integer;
begin
  inherited;
  for I := 0 to FConnects.Count - 1 do
    with FConnects[I] do
    if (FIdx >= 0) and not FDisconnected and (FSysTimeTable >= MTE_OK) and (GetTickCount - FLastRequestTime > 10000) then
    begin
      Err := MTEAddTable(FIdx, FSysTimeTable, 0);
      if Err = MTE_OK then Err := MTERefresh(FIdx, Msg);
      if Err <> MTE_OK then
      begin
        ValidateConnect(FConnects[I], Err);
        ShowError(FCaption, Err, PChar(GetTsmrStr(Err, Msg)), 'MTERefresh(SYSTIME) - keep alive timer')
      end
    end;
end;

procedure TMainForm.actExportExecute(Sender: TObject);
var
  Connect: TMTEConnect;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
    if SaveHtmlDlg.Execute then
    begin
      ExportDescToFile(Connect.FIface, SaveHtmlDlg.FileName, Connect.FIface.FName);
      ShellExecute(0, nil, PChar(SaveHtmlDlg.FileName), nil, nil, SW_SHOW);
    end;
end;

function TMainForm.IsCmdSwitch(const S: string): Boolean;
begin
  Result := (S <> '') and (S[1] in ['-', '/']);  
end;

procedure TMainForm.FormShow(Sender: TObject);
var
  Params: TStringList;
  I: Integer;
begin
  inherited;
//  ObjPanel.Width := 50;
  I := 1;
  while (I <= ParamCount) and IsCmdSwitch(ParamStr(I)) do Inc(I);
  if I > ParamCount then Exit;

  with TIniFile.Create(ExtractFilePath(ParamStr(0)) + ParamStr(I)) do
  try
    Params := TStringList.Create;
    try
      ReadSectionValues('Default', Params);
      if Params.Count > 0 then
        ConnectToServer(Params.Text, False)
      else
        MessageDlg('Invalid INI-file specified:'#13#10 + ParamStr(I), mtWarning, [mbOk], 0);
    finally
      Params.Free;
    end;
  finally
    Free;
  end;
end;

procedure TMainForm.FormDestroy(Sender: TObject);
begin
  inherited;
  FConnects.Free;
  with TIniFile.Create(IniFileName) do
  try
    WriteInteger('General', 'IdViewMode', Integer(IdViewMode));
  finally
    Free;
  end;
end;

{ TMTEConnectList }

function TMTEConnectList.Add(AHandle: Integer; AIface: TMicexIface;
  const ACaption: string; ATree: TTreeView): TMTEConnect;
begin
  Result := TMTEConnect.Create;
  Result.FIdx := AHandle;
  Result.FIface := AIface;
  Result.FCaption := ACaption;
  Result.FTree := ATree;
  Result.FSysTimeTable := -1;
  if Assigned(AIface) then Result.SetupKeepAlive;
  inherited Add(Result);
end;

procedure TMTEConnectList.Clear;
var
  I: Integer;
begin
  for I := 0 to Count - 1 do
    Connects[I].Free;
  inherited Clear;
end;

procedure TMTEConnectList.Delete(AIndex: Integer);
begin
  Connects[AIndex].Free;
  inherited Delete(AIndex);
end;

function TMTEConnectList.GetConnect(AIndex: Integer): TMTEConnect;
begin
  Result := Items[AIndex];
end;

procedure TMainForm.SessionChange(Sender: TObject);
begin
  inherited;
  if (cbSessions.Items.Count > 0) and (cbSessions.ItemIndex >= 0) then
  begin
    Caption := 'TE Client 2 - ' + cbSessions.Text;
    TPanel(cbSessions.Items.Objects[cbSessions.ItemIndex]).BringToFront;
  end
  else
    Caption := 'TE Client 2';
  ShowStats;
end;

procedure TMainForm.SessionPanelResize(Sender: TObject);
begin
  inherited;
  cbSessions.Width := SessionPanel.Width;
end;

procedure TMainForm.TreeViewExpanding(Sender: TObject; Node: TTreeNode;
  var AllowExpansion: Boolean);
begin
  AllowExpansion := (GetTickCount - FTreeButtonClickTime > GetDoubleClickTime)
    or FTreeButtonClick;
end;

procedure TMainForm.TreeViewMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
var
  Tree: TTreeView;
begin
  Tree := Sender as TTreeView;
  with Tree.ScreenToClient(Mouse.CursorPos) do
    FTreeButtonClick := htOnButton in Tree.GetHitTestInfoAt(X, Y);
  FTreeButtonClickTime := GetTickCount;
end;

procedure TMainForm.cbSessionsDrawItem(Control: TWinControl;
  AIndex: Integer; Rect: TRect; State: TOwnerDrawState);
var
  Connect: TMTEConnect;
begin
  inherited;
  with cbSessions, Canvas do
  begin
    if (AIndex < Items.Count) and (AIndex >= 0) then
      Connect := TMTEConnect(TPanel(Items.Objects[AIndex]).Components[0].Tag)
    else
      Connect := nil;

    if Assigned(Connect) and Connect.FDisconnected then Font.Color := clRed;
    TextRect(Rect, Rect.Left + 3, Rect.Top + 1, cbSessions.Items[AIndex]);
  end;
end;

procedure TMainForm.InvalidateWindows;
var
  I: Integer;
begin
  for I := 0 to MDIChildCount - 1 do
    if MDIChildren[I] is TTableForm then
      with TTableForm(MDIChildren[I]) do LV.Invalidate;
end;

procedure TMainForm.miShowAllClick(Sender: TObject);
begin
  inherited;
  IdViewMode := TIdViewMode((Sender as TMenuItem).Tag);
  InvalidateWindows;
end;

procedure TMainForm.ValidateConnect(Connect: TMTEConnect; ErrorCode: Integer = MTE_OK);
var
  ConnValid: Boolean;
begin
  if msaConnStatus in GetMTESrlAddons then
    ConnValid := MTEConnectionStatus(Connect.FIdx) = MTE_OK
  else
    ConnValid := (ErrorCode >= MTE_OK) or (ErrorCode = MTE_TSMR) or (ErrorCode = MTE_TRANSREJECTED);
  if not ConnValid then
  begin
    Connect.FDisconnected := True;
    cbSessions.Invalidate;
  end;
end;

procedure TMainForm.actDllVerExecute(Sender: TObject);
begin
  inherited;
  if msaGetVersion in GetMTESrlAddons then
    MessageDlg('Версия используемой библиотеки MTESRL.DLL:'#13#10#13#10 + MTEGetVersion, mtInformation, [mbOk], 0)
  else MessageDlg('Not implemented', mtWarning, [mbOk], 0);
end;

function GetCertAltName(AltName: TMTECertAltName): string;
begin
  FmtStr(Result, #9'Организация: %s'#13#10#9'Фамилия: %s'#13#10#9'Описание: %s'#13#10#9'(%s)',
    [AltName.OrganizationName, AltName.Surname, AltName.Description, AltName.Subject]);
end;

function GetCertInfo(Cert: TMTEConnCertificate): string;
begin
  Result := 'Информация о владельце сертификата:' + #13#10 +
    GetCertAltName(Cert.Owner);

  if Result <> '' then Result := Result + #13#10;
  Result := Result + 'Информация об издателе сертификата:' + #13#10 +
    GetCertAltName(Cert.Issuer);

  if Result <> '' then Result := Result + #13#10;
  Result := Result + Format('Действителен с %s по %s',
    [FormatDateTime('dddddd', SystemTimeToDateTime(Cert.ValidFrom)),
      FormatDateTime('dddddd', SystemTimeToDateTime(Cert.ValidTo))]);
end;

function NotNullStr(const S, NullStr: string): string;
begin
  if Trim(S) = '' then Result := NullStr else Result := S;
end;

procedure TMainForm.ShowCertificates;
var
  Connect: TMTEConnect;
  MyCert, ServerCert: TMTEConnCertificate;
  Err: Integer;
  Stats: TMTEConnStats;
//  S: string;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    Err := MTEConnectionCerts(Connect.FIdx, @MyCert, @ServerCert);
    if Err = MTE_OK then
    begin
      FillChar(Stats, SizeOf(Stats), 0);
      Stats.Size := SizeOf(Stats);
      Err := MTEConnectionStats(Connect.FIdx, Stats);
      if Err = MTE_OK then
      begin
{        if Assigned(ServerCert.Owner.Subject) then
          S := AnsiUpperCase('Сертификат сервера:'#13#10) + GetCertInfo(ServerCert)
        else
          if Stats.Properties and (FLAG_ENCRYPTED or FLAG_SIGNING_ON) <> 0 then
            S := AnsiUpperCase('ОШИБКА: Шифрование или ЭЦП включены, но сертификат сервера получить не удается!')
          else
            S := 'Шифрование и ЭЦП не используется, сертификат сервера неизвестен';

        if S <> '' then S := S + #13#10#13#10;
        if Assigned(MyCert.Owner.Subject) then
          S := S + AnsiUpperCase('Собственный сертификат:'#13#10) + GetCertInfo(MyCert)
        else
          if Stats.Properties and (FLAG_ENCRYPTED or FLAG_SIGNING_ON) <> 0 then
            S := S + AnsiUpperCase('ОШИБКА: Шифрование или ЭЦП включены, но собственный сертификат получить не удается!')
          else
            S := S + 'Шифрование и ЭЦП не используется, собственный сертификат неизвестен';

        MessageDlg(Connect.FCaption + #13#10 + StringOfChar('-', 80) + #13#10 + S,
          mtInformation, [mbOk], 0);}

        with TCertInfoForm.Create(Self) do
        try
          Caption := Caption + NotNullStr(Stats.ServerName, 'TEServer') + ' - ' + Connect.FIface.FName;
          SetCertificates(@MyCert, @ServerCert, @Stats);
          ShowModal;
        finally
          Free;
        end;
      end
      else ShowError(Connect.FCaption, Err, '', 'MTEConnectionStats');
    end
    else ShowError(Connect.FCaption, Err, '', 'MTEConnectionCerts');
  end;
end;

procedure TMainForm.StatusBarDblClick(Sender: TObject);
begin
  if actShowCert.Enabled then ShowCertificates;
end;

procedure TMainForm.actShowCertExecute(Sender: TObject);
begin
  ShowCertificates;
end;

function TimeAsStr(Value: Integer): string;
var
  H, M, S: Integer;
begin
  H := EDivMod(Value, 10000, M);
  M := EDivMod(M, 100, S);
  FmtStr(Result, '%d:%.2d:%.2d', [H, M, S]);
end;

function DateAsStr(Value: Integer): string;
var
  Y, M, D: Integer;
begin
  D := EDivMod(Value, 1000000, M);
  M := EDivMod(M, 10000, Y);
  FmtStr(Result, '%.2d.%.2d.%.4d', [D, M, Y]);
end;

procedure TMainForm.actServInfoExecute(Sender: TObject);
var
  Connect: TMTEConnect;
  Err, Len, SrvNameLen: Integer;
  ServInfo: PByte;
  ptr: PChar;
  S, VerStr, Boards, MicexIp: string;
  P: PMTEServInfo;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    Err := MTEGetServInfo(Connect.FIdx, ServInfo, Len);
    if Err = MTE_OK then
    begin
      if Len >= SizeOf(TMTEServInfo) then
      begin
        P := PMTEServInfo(ServInfo);
        FmtStr(VerStr, '%d.%d.%d (beta=%d, debug=%d, test=%d)',
          [P.Version_Major, P.Version_Minor, P.Version_Build, P.Beta_version, P.Debug_flag, P.Test_flag]);

        // try to extract MICEX server IP:
        // it goes after in Micex_Server_Name after terminating NULL-char if enough space
        MicexIp := 'N/A';
        SrvNameLen := StrLen(P.MICEX_Sever_Name) + 1;
        if SrvNameLen + SizeOf(Cardinal) <= SizeOf(P.MICEX_Sever_Name) then
        begin
          ptr := P.MICEX_Sever_Name + SrvNameLen;
          if PInteger(ptr)^ <> 0 then MicexIp := Einet_ntoa(PInteger(ptr)^);
        end;

        FmtStr(S, 'Connected to Micex: %u'#13#10 +
          'Session_Id: %u (started %s)'#13#10 +
          'MICEX_Sever_Name: %s'#13#10 +
          'MICEX_Server_IP: %s'#13#10 +
          'TEServer version: %s'#13#10 +
          'Время работы: %s - %s (%s)'#13#10 +
          'Next_Event: %u'#13#10 +
          'Event_Date: %s',
          [P.Connected_To_MICEX, P.Session_Id, DateTimeToStr(EUnixStampToDateTime(P.Session_Id)), P.MICEX_Sever_Name, MicexIp, VerStr,
            TimeAsStr(P.Start_Time), TimeAsStr(P.Stop_Time_Min), TimeAsStr(P.Stop_Time_Max),
              P.Next_Event, DateAsStr(P.Event_Date)]);

        if Len > SizeOf(TMTEServInfo) then
        begin
          SetString(Boards, PChar(@P.Boards), Len - SizeOf(TMTEServInfo));
          S := S + #13#10'Список режимов: ' + Boards;
        end;

        MessageDlg(S, mtinformation, [mbOk], 0);
      end
      else MessageDlg('MTEGetServInfo() вернула слишком короткий буфер', mtError, [mbOk], 0);
    end
    else
    begin
      ValidateConnect(Connect, Err);
      SetString(S, PAnsiChar(ServInfo), Len);
      ShowError(Connect.FCaption, Err, PChar(S), 'MTEGetServInfo');
    end;
  end;
end;

procedure TMainForm.Logon;
var
  Connect: TMTEConnect;
  Err, SessionIndex: Integer;
  ACaption: string;
  Panel: TPanel;
begin
  Connect := GetActiveConnect;
  if Assigned(Connect) then
  begin
    CloseWindows(Connect);

    Err := MTELogon(Connect.FIdx, Integer(FModifyTime));
    if Err <> MTE_OK then
    begin
      if Err <> MTE_TEUNAVAIL then ValidateConnect(Connect, Err);
      ShowError(Connect.FCaption, Err, '', 'MTELogon');
      Exit;
    end;

    Connect.FLoggedon := True;
    Connect.FIface := DownloadStructure(Connect.FIdx, Connect);
    if Assigned(Connect.FIface) then
    begin
      ACaption := GetIfaceCaption(Connect.FIface, Connect.FParams);
      SessionIndex := cbSessions.ItemIndex;
      Panel := TPanel(cbSessions.Items.Objects[SessionIndex]);
      if Assigned(Panel) then Panel.Caption := ACaption;
      cbSessions.Items[SessionIndex] := ACaption;
      Connect.FCaption := ACaption;
      cbSessions.ItemIndex := SessionIndex;
      PlantTrees(Connect);
      Connect.SetupKeepAlive;
      DownloadDictionary(Connect);
      SessionChange(cbSessions);
      FreeBuffer(Connect);
    end;
    ShowStatus('');
    ShowStats;
  end;
end;

procedure TMainForm.actLogonExecute(Sender: TObject);
begin
  Logon;
end;

end.

