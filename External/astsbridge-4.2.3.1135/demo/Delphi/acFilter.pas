unit acFilter;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, acBase, StdCtrls, ActnList, AcStruc;

type
  TFilterForm = class(TBaseForm)
    Label1: TLabel;
    cbFields: TComboBox;
    btnOk: TButton;
    btnCancel: TButton;
    Actions: TActionList;
    actOk: TAction;
    rbValue: TRadioButton;
    rbEmpty: TRadioButton;
    rbNotEmpty: TRadioButton;
    edValue: TComboBox;
    procedure actOkExecute(Sender: TObject);
    procedure ActionsUpdate(Action: TBasicAction;
      var Handled: Boolean);
    procedure edValueChange(Sender: TObject);
    procedure cbFieldsChange(Sender: TObject);
  private
    { Private declarations }
  public
    BaseTable: TMicexMessage;
    procedure ShowPossibleFieldValues;
    function GetFilterValue: string;
    procedure SetFilterValue(const Value: string);
  end;

implementation

{$R *.dfm}

procedure TFilterForm.actOkExecute(Sender: TObject);
begin
  inherited;
  ModalResult := mrOK;
end;

procedure TFilterForm.ActionsUpdate(Action: TBasicAction;
  var Handled: Boolean);
begin
  inherited;
  actOk.Enabled := (cbFields.ItemIndex >= 0) and
    ((rbValue.Checked and (Trim(edValue.Text) <> '')) or rbEmpty.Checked or rbNotEmpty.Checked);
  Handled := True;
end;

procedure TFilterForm.edValueChange(Sender: TObject);
begin
  inherited;
  rbValue.Checked := True;
end;

procedure TFilterForm.cbFieldsChange(Sender: TObject);
begin
  inherited;
  ShowPossibleFieldValues;
end;

procedure TFilterForm.SetFilterValue(const Value: string);
begin
  edValue.Text := Value;
  if cbFields.ItemIndex >= 0 then
    with BaseTable.FOutFields[cbFields.ItemIndex] do
      if Assigned(FEnumPtr) then
        edValue.ItemIndex := FEnumPtr.GetConstIndex(Value);
end;

function TFilterForm.GetFilterValue: string;
begin
  Result := edValue.Text;
  if cbFields.ItemIndex >= 0 then
    with BaseTable.FOutFields[cbFields.ItemIndex] do
      if Assigned(FEnumPtr) then
        Result := Copy(Result, 1, FEnumPtr.FSize);
end;

procedure TFilterForm.ShowPossibleFieldValues;
var
  I: Integer;
  Style: TComboBoxStyle;
  OldText: string;
begin
  OldText := edValue.Text;
  edValue.Items.Clear;
  Style := csDropDown;
  if cbFields.ItemIndex >= 0 then
    with BaseTable.FOutFields[cbFields.ItemIndex] do
      if Assigned(FEnumPtr) then
      begin
        for I := 0 to FEnumPtr.GetConstCount - 1 do
          edValue.Items.Add(FEnumPtr.GetConstAndName(I));
        Style := csDropDownList;
      end;
  edValue.Style := Style;
  if (Style = csDropDownList) and (OldText <> '') then
    edValue.ItemIndex := edValue.Items.IndexOf(OldText);
end;

end.
