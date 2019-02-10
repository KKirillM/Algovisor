inherited FilterForm: TFilterForm
  ActiveControl = cbFields
  BorderStyle = bsDialog
  Caption = #1060#1080#1083#1100#1090#1088' '#1087#1086' '#1087#1086#1083#1102
  ClientHeight = 135
  ClientWidth = 342
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 20
    Width = 29
    Height = 13
    Caption = #1055#1086#1083#1077':'
  end
  object cbFields: TComboBox
    Left = 84
    Top = 16
    Width = 237
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    OnChange = cbFieldsChange
  end
  object btnOk: TButton
    Left = 160
    Top = 96
    Width = 75
    Height = 25
    Action = actOk
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object btnCancel: TButton
    Left = 248
    Top = 96
    Width = 75
    Height = 25
    Cancel = True
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 2
  end
  object rbValue: TRadioButton
    Left = 16
    Top = 51
    Width = 97
    Height = 17
    Caption = #1047#1085#1072#1095#1077#1085#1080#1077':'
    TabOrder = 3
  end
  object rbEmpty: TRadioButton
    Left = 16
    Top = 99
    Width = 113
    Height = 17
    Caption = #1055#1091#1089#1090#1086#1077
    TabOrder = 4
  end
  object rbNotEmpty: TRadioButton
    Left = 16
    Top = 75
    Width = 113
    Height = 17
    Caption = #1053#1077' '#1087#1091#1089#1090#1086#1077
    TabOrder = 5
  end
  object edValue: TComboBox
    Left = 116
    Top = 48
    Width = 205
    Height = 21
    ItemHeight = 13
    TabOrder = 6
    OnChange = edValueChange
  end
  object Actions: TActionList
    OnUpdate = ActionsUpdate
    Left = 40
    Top = 80
    object actOk: TAction
      Caption = 'OK'
      OnExecute = actOkExecute
    end
  end
end
