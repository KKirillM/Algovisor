unit acInput;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ExtCtrls, AcStruc, ExtUtils, IniFiles, acBase, acTblFrm, MTEApi, mteTypes, ComCtrls;

type
  TInputForm = class;
  TFieldPanel = class(TCustomPanel)
  private
    FForm: TInputForm;
    FRawValue: TStaticText;
    FField: TMicexField;
    FGroupIndex: Integer;
    FControl: TWinControl;
    procedure OnChange(Comp: TObject);
    procedure paDblClick(Sender: TObject);
  public
    constructor Create(AOwner: TInputForm; AField: TMicexField; ATop, AHeight: Integer); reintroduce;
    function Prev: TFieldPanel;
    function Next: TFieldPanel;
  end;

  TPanelList = class(TList)
  private
    function GetPanel(AIndex: Integer): TFieldPanel;
  public
    function Last: TFieldPanel;
    property Panels[AIndex: Integer]: TFieldPanel read GetPanel; default;
  end;

  TInputForm = class(TBaseForm)
    paButtons: TPanel;
    cbRawValues: TCheckBox;
    btnMore: TButton;
    btnLess: TButton;
    btnOk: TButton;
    btnCancel: TButton;
    Bevel: TBevel;
    paRight: TPanel;
    paLeft: TPanel;
    paTop: TPanel;
    procedure ToggleRawValues(Sender: TObject);
    procedure MoreClick(Sender: TObject);
    procedure LessClick(Sender: TObject);
  private
    FFields: TList;
    FPanelList: TPanelList;
    // unhandled stuff
    FDecimals: Integer;
    FGroupCount: Integer;
    FGroupHeight: Integer;
    FSecList: TSecProps;
    FFromMsg: PBaseMsgRec;
    procedure OnKeyPress(Sender: TObject; var Key: Char);
    function StrToNumber(const S: string; Fld: TMicexField): string;
    function HasVarBlock: Boolean;
    procedure CreateInputField(FieldPos: Integer; Field: TMicexField; var Params: string);
    procedure AdjustHeight;
    procedure AddGroup(var Params: string; Adjust: Boolean);
    procedure FormatNumbers;
  protected
    procedure UpdateActions; override;
  public
    procedure CreateParams(var Params: TCreateParams); override;
    destructor Destroy; override;
    function GetInputParams(const AId: string; var Params: string): Boolean;
{    property Fields: IFields read FFields;
    property Tsmr: TZTsmr read FTsmr;}
  end;

function GetInputParams(AMsg: TMicexMessage; const ACaption: string; AFields: TList; AIface: TMicexIface;
  out AParams: string; AFromMsg: PBaseMsgRec; ASecList: TSecProps): Boolean;

implementation

uses TypInfo;

{$R *.dfm}

const
  SMALL_WIDTH     = 363;
  FULL_WIDTH      = 520;

resourcestring
  sRawValues      = '"Сырые" значения';
  sRawValueHint   = 'Строка, которая будет отправлена в торговую систему';

type
  THWinControl = class(TWinControl) end;

var
  ShowRawValues: Boolean = False;

function GetLastParam(Msg: TMicexMessage; Iface: TMicexIface): string;
const
  Prefixes: array [Boolean] of string = ('trn_', 'tbl_');
var
  Params: string;
  I: Integer;
begin
  with TIniFile.Create(IniFileName) do
  try
    Params := ReadString('ifc_' + Iface.FName, Prefixes[Msg.FIsTable] + Msg.FName, '');
    I := 1;
    Result := '';
    while I <= Length(Params) do
    begin
      if Params[I] = '%' then
      begin
        Inc(I);
        if Params[I] = '%' then Result := Result + '%'
        else
        begin
          Result := Result + Char(StrToIntDef('$' + Copy(Params, I, 2), 32));
          Inc(I);
        end;
      end
      else Result := Result + Params[I];
      Inc(I);
    end;
  finally
    Free;
  end;
end;

function ConvertDateTimeToStr(const DT: TDateTime; FieldType: TTEFieldType): string;
begin
  if FieldType = ftDate then
    Result := FormatDateTime('yyyymmdd', DT)
  else
    Result := FormatDateTime('hhnnss', DT);
end;

function ConvertStrToDateTime(const S: string; FieldType: TTEFieldType): TDateTime;

  function GetNumber(Buf: PChar): Integer;
  begin
    Result := (Byte(Buf^) - Ord('0')) * 10 + Byte((Buf + 1)^) - Ord('0');
  end;

var
  P: PChar;
begin
  Result := -1;
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

function ConvertInteger(AValue: string; ASize: Integer; AType: TTEFieldType; Decimals: Integer): string;
var
  PointPos, Index, PointLen: Integer;
begin
  if Decimals >= 0 then
  begin
    PointPos := LastDelimiter('.,', AValue);
    if PointPos > 0 then PointLen := Length(AValue) - PointPos else PointLen := 0;
    if PointLen < Decimals then
      for Index := 1 to Decimals - PointLen do
        AValue := AValue + '0'
    else
      if PointLen > Decimals then
        for Index := 1 to PointLen - Decimals do
          Delete(AValue, Length(AValue), 1);
  end;
  PointPos := LastDelimiter('.,', AValue);
  if PointPos > 0 then Delete(AValue, PointPos, 1);

  if (AValue <> '') and (AValue[1] = '-') then Index := 2 else Index := 1;
  while Length(AValue) < ASize do Insert('0', AValue, Index);
  while Length(AValue) > ASize do Delete(AValue, Index, 1);
  Result := Copy(AValue, 1, ASize);
end;

function ConvertParameter(const AValue: string; AKind: TTEFieldType;
  ASize: Integer; Decimals: Integer): string;
var
  S: string;
begin
  case AKind of
    ftFixed, ftFloat, ftInteger:
      if Trim(AValue) = '' then S := EPadRight(AValue, ASize)
      else
        case AKind of
          ftFixed: S := ConvertInteger(AValue, ASize, AKind, 2);
          ftFloat: S := ConvertInteger(AValue, ASize, AKind, Decimals);
          else S := ConvertInteger(AValue, ASize, AKind, 0);
        end;

    else S := EPadRight(AValue, ASize);
  end;
  SetString(Result, PChar(S), ASize);
end;

function StripField(var S: string; Size: Integer; AType: TTEFieldType): string;
var
  Decimals: Integer;
begin
  Result := Trim(Copy(S, 1, Size));
  if (Result <> '') and (AType in [ftInteger, ftFixed, ftFloat]) then
  begin
    if AType = ftFixed then Decimals := 2 else Decimals := 0;
    Result := EFormatNumber(Result, Decimals);
  end;
  Delete(S, 1, Size);
end;

function GetFieldHint(Fld: TMicexField): string;
var
  SizeStr: string;
begin
  Result := Copy(GetEnumName(TypeInfo(TTEFieldType),
    Integer(Fld.FType)), 3, MaxInt);

  if Fld.FDecimals > 0 then FmtStr(SizeStr, '%d.%d', [Fld.FSize, Fld.FDecimals])
  else SizeStr := EIntToStr(Fld.FSize);

  if Assigned(Fld.FEnumPtr) then
    SizeStr := SizeStr + ' (' + Fld.FEnumPtr.FName + ')';

  Result := Result + ': ' + SizeStr + #13#10 + Fld.FCaption;
end;

function GetMinNeededLength(Fields: TList): Integer;
var
  I: Integer;
begin
  Result := 0;
  for I := 0 to Fields.Count - 1 do
    with TMicexField(Fields[I]) do
      if FFlags and ffVarBlock = 0 then Inc(Result, FSize);
end;

function GetFieldFromMsg(FieldId: string; FromMsg: PBaseMsgRec; AType: TTEFieldType): string;
var
  FieldIndex, Decimals: Integer;
begin
  Result := '';
  if Assigned(FromMsg) then
    with FromMsg^ do
    begin
      if SameText(Msg.FName, 'BCMESSAGES') then
      begin
        if SameText(FieldId, 'USERIDTO') then FieldId := 'FROMUSER';
      end;

      if Msg.FOutFields.Find(FieldId, FieldIndex) then
      begin
        Result := Rows.Values[RowNum, FieldIndex];
        if (Result <> '') and (AType in [ftInteger, ftFixed, ftFloat]) then
        begin
          if AType = ftFixed then Decimals := 2 else Decimals := 0;
          Result := EFormatNumber(Result, Decimals);
        end;
      end
    end;
end;

function GetInputParams(AMsg: TMicexMessage; const ACaption: string; AFields: TList; AIface: TMicexIface;
  out AParams: string; AFromMsg: PBaseMsgRec; ASecList: TSecProps): Boolean;
begin
  if not Assigned(AFields) or (AFields.Count = 0) then
  begin
    AParams := '';
    Result := True;
    Exit;
  end;

  AParams := GetLastParam(AMsg, AIface);
  with TInputForm.Create(Application) do
  try
    FFields := AFields;
    FSecList := ASecList;
    FFromMsg := AFromMsg;
    Caption := ACaption;
    Result := GetInputParams(ACaption, AParams);
  finally
    Free;
  end;
end;

{ TInputForm }

procedure TInputForm.CreateParams(var Params: TCreateParams);
begin
  inherited;
  Params.WndParent := Screen.Forms[0].Handle;
  FDecimals := -1;
end;

destructor TInputForm.Destroy;
begin
  FreeAndNil(FPanelList);
  inherited;
end;

procedure TInputForm.ToggleRawValues(Sender: TObject);
const
  Widths: array [Boolean] of Integer = (SMALL_WIDTH, FULL_WIDTH);
begin
  Width := Widths[(Sender as TCheckBox).Checked];
end;

function TInputForm.StrToNumber(const S: string; Fld: TMicexField): string;
begin
  Result := Trim(S);
  if Result <> '' then
  if (Fld.FType in [ftFixed, ftFloat]) and (LastDelimiter('.,', S) = 0) then
  begin
    if Fld.FDecimals < 0 then Result := EFormatNumber(S, FDecimals)
    else Result := EFormatNumber(S, Fld.FDecimals);
  end;
end;

procedure TInputForm.OnKeyPress(Sender: TObject; var Key: Char);
var
  Fld: TMicexField;
begin
  Fld := TMicexField((Sender as TComponent).Tag);
  if Fld.FType in [ftInteger, ftFixed, ftFloat] then
  begin
    if (Fld.FDecimals <> 0) and (not (Key in ['0'..'9', '.', ',', #8])) then Key := #0;
    if (Fld.fDecimals = 0) and (not (Key in ['0'..'9', #8])) then Key := #0;
  end;
end;

function TInputForm.HasVarBlock: Boolean;
var
  I: Integer;
begin
  for I := 0 to FFields.Count - 1 do
    if TMicexField(FFields[I]).FFlags and ffVarBlock <> 0 then
    begin
      Result := True;
      Exit;
    end;
  Result := False;
end;

procedure TInputForm.AddGroup(var Params: string; Adjust: Boolean);
var
  StartIndex, EndIndex, PanelIndex, I{, PrevHeight}: Integer;
begin
  // find first and last field of repeatable group
  StartIndex := -1;
  EndIndex := -1;
  for I := FFields.Count - 1 downto 0 do
    if TMicexField(FFields[I]).FFlags and ffVarBlock <> 0 then
    begin
      if EndIndex < 0 then EndIndex := I;
      StartIndex := I;
    end;

  PanelIndex := FPanelList.Count;
  // find panel before which to insert a new group
  if EndIndex < FFields.Count - 1 then
    for I := FPanelList.Count - 1 downto 0 do
      if FPanelList[I].FField = FFields[EndIndex + 1] then
      begin
        PanelIndex := I;
        Break;
      end;

  Inc(FGroupCount);
  for I := StartIndex to EndIndex do
  begin
    CreateInputField(PanelIndex, FFields[I], Params);
    Inc(PanelIndex);
  end;

//  PrevHeight := Height;
  AdjustHeight;
//  Top := Top - (Height - PrevHeight) div 2;
end;

procedure TInputForm.MoreClick(Sender: TObject);
var
  Params: string;
begin
  Params := '';
  AddGroup(Params, True);
end;

procedure TInputForm.LessClick(Sender: TObject);
var
  I{, PrevHeight}: Integer;
begin
  for I := FPanelList.Count - 1 downto 0 do
    if FPanelList[I].FGroupIndex = FGroupCount then
    begin
      FPanelList[I].Free;
      FPanelList.Delete(I);
    end;

  Dec(FGroupCount);
//  PrevHeight := Height;
  AdjustHeight;
//  Top := Top + (PrevHeight - Height) div 2;
end;

procedure TInputForm.CreateInputField(FieldPos: Integer; Field: TMicexField; var Params: string);
const
  sLabels: array [Boolean] of string = ('%s:', '%s [%d]:');
  sChecks: array [Boolean] of string = ('%s', '%s [%d]');
  sGroups: array [Boolean] of string = (' %s ', ' %s [%d] ');
var
  Value, FullSecCode: string;
  CI, ConstIdx, FieldTop, FieldHeight: Integer;
  FieldPanel: TFieldPanel;
begin
  with Field do
  begin
    if not Assigned(FEnumPtr) or (FEnumPtr.FKind <> ekGroup) then FieldHeight := 24
    else FieldHeight := (FEnumPtr.GetConstCount + 1) shr 1 * 17 + 21;

    if FPanelList.Count = 0 then FieldTop := 0
    else
      if FieldPos < FPanelList.Count then FieldTop := FPanelList[FieldPos].Top
      else with FPanelList.Last do FieldTop := Top + Height;

    FieldPanel := TFieldPanel.Create(Self, Field, FieldTop, FieldHeight);
    FPanelList.Insert(FieldPos, FieldPanel);

    with TLabel.Create(Self) do
    begin
      if not Assigned(FEnumPtr) or (FEnumPtr.FKind = ekCombo) then
        Caption := Format(sLabels[FFlags and ffVarBlock <> 0], [Field.DisplayName, FGroupCount]);
      SetBounds(12, (FieldHeight - Height) div 2, 165, Height);
      Parent := FieldPanel;
    end;

    if not Assigned(FEnumPtr) then
    begin
      if FType in [ftDate, ftTime] then
      begin
        FieldPanel.FControl := TDateTimePicker.Create(Self);
        with TDateTimePicker(FieldPanel.FControl) do
        begin
          Value := GetFieldFromMsg(FName, FFromMsg, FType);
          if Value = '' then
            if Params = '' then Value := FDefValue
            else Value := StripField(Params, FSize, FType)
          else StripField(Params, FSize, FType);

          if FType = ftTime then
          begin
            DateMode := dmUpDown;
            Kind := dtkTime;
          end;
          ShowCheckBox := True;
          DateTime := ConvertStrToDateTime(Value, FType);
          SetBounds(181, (FieldHeight - Height) div 2, 165, Height);
          Tag := Integer(Field);
          Hint := GetFieldHint(Field);
          OnChange := FieldPanel.OnChange;
          Parent := FieldPanel;
          if DateTime = -1 then
          begin
            DateTime := Now;;
            Checked := False;
          end;
        end;
      end
      else
        if FFlags and ffSecCode <> 0 then
        begin
          FieldPanel.FControl := TComboBox.Create(Self);
          with TComboBox(FieldPanel.FControl) do
          begin
            Parent := FieldPanel;
            Hint := GetFieldHint(Field);
            Style := csDropDown;
            SetBounds(181, (FieldHeight - Height) div 2, 165, Height);
            Tag := Integer(Field);
            MaxLength := FSize;

            Sorted := True;
            for CI := 0 to FSecList.Count - 1 do
              with FSecList[CI]^ do
                Items.AddObject(Format('%s - %s',
                  [ShortName, Copy(SecCode, 1, 4)]), TObject(FSecList[CI]));

            Value := GetFieldFromMsg(FName, FFromMsg, FType);
            if Value = '' then
              if Params = '' then Value := FDefValue
              else Value := StripField(Params, FSize, FType)
            else StripField(Params, FSize, FType);

            Text := Value;
            OnChange := FieldPanel.OnChange;

            FullSecCode := Trim(Value);
            // form SECBOARD + SECCODE
            if FieldPanel.Prev <> nil then
              FullSecCode := EPadRight(THWinControl(FieldPanel.Prev.FControl).Text, 4) + FullSecCode;

            for CI := 0 to Items.Count - 1 do
              if Trim(PSecProp(Items.Objects[CI]).SecCode) = FullSecCode then
              begin
                ItemIndex := CI;
                OnChange(FieldPanel.FControl);
                Break;
              end;

          end;
        end
        else
        begin
          FieldPanel.FControl := TEdit.Create(Self);
          with TEdit(FieldPanel.FControl) do
          begin
            Value := GetFieldFromMsg(FName, FFromMsg, FType);
            if Value = '' then
              if Params = '' then Value := FDefValue
              else Value := StripField(Params, FSize, FType)
            else StripField(Params, FSize, FType);

            Text := StrToNumber(Value, Field);
            SetBounds(181, (FieldHeight - Height) div 2, 165, Height);
            if (FType in [ftInteger, ftFixed, ftFloat]) and (FDecimals <> 0) then
              MaxLength := FSize + 1
            else
              MaxLength := FSize;
            Tag := Integer(Field);
            Hint := GetFieldHint(Field);
            OnChange := FieldPanel.OnChange;
            Parent := FieldPanel;
          end;
        end;
    end
    else    // handle EnumType
    begin
      Value := GetFieldFromMsg(FName, FFromMsg, FType);
      if Value = '' then
        if Params = '' then Value := FDefValue
        else Value := StripField(Params, FSize, FType)
      else StripField(Params, FSize, FType);

      ConstIdx := FEnumPtr.GetConstIndex(PChar(Value));
      if ConstIdx < 0 then ConstIdx := 0;

      case FEnumPtr.FKind of
        ekCheck:
          begin
            FieldPanel.FControl := TCheckBox.Create(Self);
            with TCheckBox(FieldPanel.FControl) do
            begin
              Caption := Format(sChecks[Field.FFlags and ffVarBlock <> 0], [Field.DisplayName, FGroupCount]);
              Hint := GetFieldHint(Field);
              SetBounds(12, (FieldHeight - Height) div 2, 334, Height);
              Tag := Integer(Field);
              Checked := Boolean(ConstIdx);
              OnClick := FieldPanel.OnChange;
              Parent := FieldPanel;
            end;
          end;

        ekGroup:
          begin
            FieldPanel.FControl := TRadioGroup.Create(Self);
            with TRadioGroup(FieldPanel.FControl) do
            begin
              Caption := Format(sGroups[Field.FFlags and ffVarBlock <> 0], [Field.DisplayName, FGroupCount]);
              Hint := GetFieldHint(Field);
              Tag := Integer(Field);
              Columns := 2;

              Items.BeginUpdate;
              with FEnumPtr do
              begin
                SetBounds(12, 2, 334, FieldHeight - 4);
                for CI := 0 to GetConstCount - 1 do
                  Items.Add(GetLongName(CI));
              end;

              Parent := FieldPanel;
              Items.EndUpdate;
              OnClick := FieldPanel.OnChange;
              ItemIndex := ConstIdx;
            end;
          end;

        ekCombo:
          begin
            FieldPanel.FControl := TComboBox.Create(Self);
            with TComboBox(FieldPanel.FControl) do
            begin
              Parent := FieldPanel;
              Hint := GetFieldHint(Field);
              Style := csDropDownList;
              SetBounds(181, (FieldHeight - Height) div 2, 165, Height);
              Tag := Integer(Field);
              with FEnumPtr do
                for CI := 0 to GetConstCount - 1 do
                  Items.Add(GetLongName(CI));
              ItemIndex := ConstIdx;
              OnChange := FieldPanel.OnChange;
            end;
          end;
      end;
    end;

    FieldPanel.OnChange(FieldPanel.FControl);
    THWinControl(FieldPanel.FControl).OnKeyPress := Self.OnKeyPress;
  end;
end;

procedure TInputForm.AdjustHeight;
var
  I: Integer;
begin
  if FPanelList.Count > 0 then
     with FPanelList.Last do
       Self.ClientHeight := Top + Height + paButtons.Height + 8
  else Self.ClientHeight := 50;

  // adjust TabOrder of all created panels
  for I := 0 to FPanelList.Count - 1 do
    FPanelList[I].TabOrder := I;
end;

function TInputForm.GetInputParams(const AId: string; var Params: string): Boolean;
var
  I, H, DecNum, ParamsLen, MinNeededLength, TailLength, GroupLength, LastGroupIndex: Integer;
  Comp: TComponent;
  Fld: TMicexField;
  Value, TailParams: string;
begin
  FPanelList := TPanelList.Create;
  Width := FULL_WIDTH;
  Height := Screen.Height;
  ParamsLen := Length(Params);
  GroupLength := 0;

  btnMore.Visible := HasVarBlock;
  btnLess.Visible := btnMore.Visible;

  MinNeededLength := GetMinNeededLength(FFields);
  if (ParamsLen = 0) or (ParamsLen >= MinNeededLength) then FGroupCount := 1
  else FGroupCount := 0;  // not enough parameters even for 1 group of repeatable fields

  TailLength := 0;
  LastGroupIndex := MaxInt;
  H := 0;
  for I := FFields.Count - 1 downto 0 do
    if TMicexField(FFields[I]).FFlags and ffVarBlock = 0 then Inc(H, TMicexField(FFields[I]).FSize)
    else
    begin
      LastGroupIndex := I;
      TailLength := H;
      Break;
    end;

  TailParams := Copy(Params, Length(Params) - TailLength + 1, TailLength);
  Params := Copy(Params, 1, Length(Params) - TailLength);

  for I := 0 to FFields.Count - 1 do
  begin
    // Skip first USERID field
    if (TMicexField(FFields[I]).FFlags and ffVarBlock = 0) or (FGroupCount > 0) then
      if I <= LastGroupIndex then
        CreateInputField(FPanelList.Count, TMicexField(FFields[I]), Params)
      else
        CreateInputField(FPanelList.Count, TMicexField(FFields[I]), TailParams);
    if TMicexField(FFields[I]).FFlags and ffVarBlock <> 0 then
    begin
      Inc(FGroupHeight, FPanelList.Last.Height);
      Inc(GroupLength, TMicexField(FFields[I]).FSize);
    end;
  end;

  if (GroupLength <> 0) and (Length(Params) >= GroupLength) then
    for I := 0 to Length(Params) div GroupLength - 1 do
      AddGroup(Params, False);
  AdjustHeight;

  cbRawValues.Checked := ShowRawValues;
  ToggleRawValues(cbRawValues);
  Params := '';
  FormatNumbers;

  if ShowModal = mrOk then
  begin
    Result := True;
    for I := 0 to FPanelList.Count - 1 do
    begin
      Comp := FPanelList[I].FControl;
      Fld := FPanelList[I].FField;
      if Comp is TEdit then Value := TEdit(Comp).Text
      else
        if Comp is TDateTimePicker then
        begin
          if TDateTimePicker(Comp).Checked then
            Value := ConvertDateTimeToStr(TDateTimePicker(Comp).DateTime, Fld.FType)
          else
            Value := '';
        end
        else
        begin
          if Comp is TComboBox then
          begin
            if Fld.FFlags and ffSecCode <> 0 then
              with TComboBox(Comp) do
                if ItemIndex >= 0 then
                  Value := Copy(PSecProp(Items.Objects[ItemIndex]).SecCode, 5, 12)
                else
                  Value := Text
            else
              Value := Fld.FEnumPtr.GetConst(TComboBox(Comp).ItemIndex);
          end
          else
          begin
            if Comp is TCheckBox then H := Byte(TCheckBox(Comp).Checked) else
              H := TRadioGroup(Comp).ItemIndex;
            with Fld.FEnumPtr do
              Value := GetConst(H);
          end;
        end;

      if Fld.FDecimals < 0 then DecNum := FDecimals else DecNum := Fld.FDecimals;
      with Fld do Value := ConvertParameter(Value, FType, FSize, DecNum);
      Params := Params + Value;
    end;
  end
  else Result := False;
  ShowRawValues := cbRawValues.Checked;
end;

procedure TInputForm.UpdateActions;
begin
  inherited;
  btnMore.Enabled := Height + FGroupHeight < Screen.Height;
  btnLess.Enabled := FGroupCount > 0;
end;

procedure TInputForm.FormatNumbers;
var
  I: Integer;
  Panel: TFieldPanel;
begin
  if FDecimals >= 0 then
    for I := 0 to FPanelList.Count - 1 do
    begin
      Panel := FPanelList[I];
      if Panel.FField.FType = ftFloat then Panel.OnChange(Panel.FControl);
    end;
end;

{ TFieldPanel }

constructor TFieldPanel.Create(AOwner: TInputForm; AField: TMicexField; ATop, AHeight: Integer);
begin
  inherited Create(AOwner);
  FForm := AOwner;
  FField := AField;
  Height := AHeight;
  Align := alTop;
  Top := ATop;
  Parent := AOwner as TWinControl;
  ParentFont := True;
  BevelOuter := bvNone;
  Caption := '';
  OnDblClick := paDblClick;
  if AField.FFlags and ffVarBlock <> 0 then FGroupIndex := AOwner.FGroupCount;

  FRawValue := TStaticText.Create(Self);
  with FRawValue do
  begin
    SetBounds(358, (AHeight - 17) div 2, Self.Width - 362, 17);
    Font.Name := 'Courier New';
    Caption := 'raw value';
    Hint := sRawValueHint;
    BorderStyle := sbsSunken;
    Parent := Self;
  end;
end;

function TFieldPanel.Next: TFieldPanel;
var
  I: Integer;
begin
  with FForm.FPanelList do
  begin
    I := IndexOf(Self) + 1;
    if I < Count then Result := Panels[I] else Result := nil;
  end;
end;

function TFieldPanel.Prev: TFieldPanel;
var
  I: Integer;
begin
  with FForm.FPanelList do
  begin
    I := IndexOf(Self) - 1;
    if I >= 0 then Result := Panels[I] else Result := nil;
  end;
end;

procedure TFieldPanel.OnChange(Comp: TObject);
var
  Value: string;
  H, {I, }DecNum: Integer;
begin
  if Comp is TEdit then Value := TEdit(Comp).Text
  else
    if Comp is TDateTimePicker then
    begin
      if TDateTimePicker(Comp).Checked then
        Value := ConvertDateTimeToStr(TDateTimePicker(Comp).DateTime, FField.FType)
      else
        Value := '';
    end
    else
    begin
      if Comp is TComboBox then
      begin
        if FField.FFlags and ffSecCode <> 0 then
          with TComboBox(Comp) do
        begin

          if ItemIndex >= 0 then
          begin
            with PSecProp(Items.Objects[ItemIndex])^ do
            begin
              if Prev <> nil then
              begin
                THWinControl(Prev.FControl).Text := Copy(SecCode, 1, 4);
              end;
              Value := Copy(SecCode, 5, 12);
              FForm.FDecimals := Decimals;
            end;
          end
          else
          begin
            Value := Text;
            FForm.FDecimals := -1;
          end;

          FForm.FormatNumbers;
        end
        else
          Value := FField.FEnumPtr.GetConst(TComboBox(Comp).ItemIndex);
      end
      else
      begin
        if Comp is TCheckBox then H := Byte(TCheckBox(Comp).Checked) else
          H := TRadioGroup(Comp).ItemIndex;
        with FField.FEnumPtr do
          Value := GetConst(H);
      end;
    end;

  if FField.FDecimals < 0 then DecNum := FForm.FDecimals else DecNum := FField.FDecimals;
  with FField do Value := ConvertParameter(Value, FType, FSize, DecNum);

  FRawValue.Caption := Value;
end;

procedure TFieldPanel.paDblClick(Sender: TObject);
begin
//  ShowMessageFmt('Font Name="%s", Size=%d', [FRawValue.Font.Name, FRawValue.Font.Size]);
  FRawValue.Font.Style := [fsItalic];
end;

{ TPanelList }

function TPanelList.GetPanel(AIndex: Integer): TFieldPanel;
begin
  Result := Items[AIndex];
end;

function TPanelList.Last: TFieldPanel;
begin
  Result := Items[Count - 1];
end;

end.
