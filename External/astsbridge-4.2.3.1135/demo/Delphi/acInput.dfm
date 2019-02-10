inherited InputForm: TInputForm
  Left = 230
  Top = 203
  BorderStyle = bsDialog
  Caption = 'InputForm'
  ClientHeight = 318
  ClientWidth = 512
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object paButtons: TPanel
    Left = 0
    Top = 282
    Width = 512
    Height = 36
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    object Bevel: TBevel
      Left = 0
      Top = 0
      Width = 512
      Height = 4
      Align = alTop
      Shape = bsTopLine
    end
    object cbRawValues: TCheckBox
      Left = 12
      Top = 8
      Width = 129
      Height = 17
      Caption = '"'#1057#1099#1088#1099#1077'" '#1079#1085#1072#1095#1077#1085#1080#1103
      TabOrder = 0
      OnClick = ToggleRawValues
    end
    object btnMore: TButton
      Left = 140
      Top = 4
      Width = 21
      Height = 25
      Caption = '+'
      TabOrder = 1
      OnClick = MoreClick
    end
    object btnLess: TButton
      Left = 164
      Top = 4
      Width = 21
      Height = 25
      Caption = '-'
      TabOrder = 2
      OnClick = LessClick
    end
    object btnOk: TButton
      Left = 192
      Top = 4
      Width = 75
      Height = 25
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 3
    end
    object btnCancel: TButton
      Left = 272
      Top = 4
      Width = 75
      Height = 25
      Cancel = True
      Caption = #1054#1090#1084#1077#1085#1072
      ModalResult = 2
      TabOrder = 4
    end
    object paRight: TPanel
      Left = 504
      Top = 4
      Width = 8
      Height = 32
      Align = alRight
      BevelOuter = bvNone
      TabOrder = 5
    end
    object paLeft: TPanel
      Left = 0
      Top = 4
      Width = 8
      Height = 32
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 6
    end
  end
  object paTop: TPanel
    Left = 0
    Top = 0
    Width = 512
    Height = 8
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
  end
end
