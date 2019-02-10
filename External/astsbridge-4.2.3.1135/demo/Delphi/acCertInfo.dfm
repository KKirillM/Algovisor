inherited CertInfoForm: TCertInfoForm
  Left = 303
  Top = 176
  BorderStyle = bsDialog
  Caption = #1057#1077#1088#1090#1080#1092#1080#1082#1072#1090#1099': '
  ClientHeight = 513
  ClientWidth = 510
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object btnClose: TButton
    Left = 420
    Top = 480
    Width = 75
    Height = 25
    Cancel = True
    Caption = #1047#1072#1082#1088#1099#1090#1100
    Default = True
    ModalResult = 2
    TabOrder = 0
  end
  object tcCerts: TTabControl
    Left = 8
    Top = 12
    Width = 489
    Height = 457
    TabOrder = 1
    Tabs.Strings = (
      #1057#1077#1088#1090#1080#1092#1080#1082#1072#1090' '#1089#1077#1088#1074#1077#1088#1072
      #1057#1077#1088#1090#1080#1092#1080#1082#1072#1090' '#1082#1083#1080#1077#1085#1090#1072)
    TabIndex = 0
    OnChange = tcCertsChange
    object Label1: TLabel
      Left = 16
      Top = 372
      Width = 143
      Height = 13
      Caption = #1057#1077#1088#1090#1080#1092#1080#1082#1072#1090' '#1076#1077#1081#1089#1090#1074#1080#1090#1077#1083#1077#1085' '#1089
    end
    object Label7: TLabel
      Left = 304
      Top = 372
      Width = 12
      Height = 13
      Caption = #1087#1086
    end
    object lblValidFrom: TLabel
      Left = 184
      Top = 372
      Width = 56
      Height = 13
      Caption = 'lblValidFrom'
    end
    object lblValidTo: TLabel
      Left = 344
      Top = 372
      Width = 56
      Height = 13
      Caption = 'lblValidFrom'
    end
    object Label11: TLabel
      Left = 16
      Top = 392
      Width = 149
      Height = 13
      Caption = #1051#1080#1095#1085#1099#1081' '#1082#1083#1102#1095' '#1076#1077#1081#1089#1090#1074#1080#1090#1077#1083#1077#1085' '#1089
    end
    object lblKeyValidFrom: TLabel
      Left = 184
      Top = 392
      Width = 74
      Height = 13
      Caption = 'lblKeyValidFrom'
    end
    object Label13: TLabel
      Left = 304
      Top = 392
      Width = 12
      Height = 13
      Caption = #1087#1086
    end
    object lblKeyValidTo: TLabel
      Left = 344
      Top = 392
      Width = 74
      Height = 13
      Caption = 'lblKeyValidFrom'
    end
    object imgStatus: TImage
      Left = 12
      Top = 28
      Width = 32
      Height = 32
    end
    object lblStatus: TLabel
      Left = 52
      Top = 32
      Width = 417
      Height = 29
      AutoSize = False
      Caption = 'lblStatus'
      Layout = tlCenter
      WordWrap = True
    end
    object gbOwner: TGroupBox
      Left = 12
      Top = 68
      Width = 465
      Height = 137
      Caption = ' '#1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086' '#1074#1083#1072#1076#1077#1083#1100#1094#1077' '#1089#1077#1088#1090#1080#1092#1080#1082#1072#1090#1072' '
      TabOrder = 0
      object Label3: TLabel
        Left = 12
        Top = 24
        Width = 70
        Height = 13
        Caption = #1054#1088#1075#1072#1085#1080#1079#1072#1094#1080#1103':'
      end
      object Label4: TLabel
        Left = 12
        Top = 52
        Width = 52
        Height = 13
        Caption = #1060#1072#1084#1080#1083#1080#1103':'
      end
      object Label5: TLabel
        Left = 12
        Top = 80
        Width = 53
        Height = 13
        Caption = #1054#1087#1080#1089#1072#1085#1080#1077':'
      end
      object Label6: TLabel
        Left = 12
        Top = 108
        Width = 39
        Height = 13
        Caption = 'Subject:'
      end
      object edOwnerFirm: TEdit
        Left = 88
        Top = 20
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 0
      end
      object edOwnerName: TEdit
        Left = 88
        Top = 48
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 1
      end
      object edOwnerDescr: TEdit
        Left = 88
        Top = 76
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 2
      end
      object edOwnerSubject: TEdit
        Left = 88
        Top = 104
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 3
      end
    end
    object GroupBox1: TGroupBox
      Left = 12
      Top = 220
      Width = 465
      Height = 137
      Caption = ' '#1048#1085#1092#1086#1088#1084#1072#1094#1080#1103' '#1086#1073' '#1080#1079#1076#1072#1090#1077#1083#1077' '#1089#1077#1088#1090#1080#1092#1080#1082#1072#1090#1072' '
      TabOrder = 1
      object Label2: TLabel
        Left = 12
        Top = 24
        Width = 70
        Height = 13
        Caption = #1054#1088#1075#1072#1085#1080#1079#1072#1094#1080#1103':'
      end
      object Label8: TLabel
        Left = 12
        Top = 52
        Width = 52
        Height = 13
        Caption = #1060#1072#1084#1080#1083#1080#1103':'
      end
      object Label9: TLabel
        Left = 12
        Top = 80
        Width = 53
        Height = 13
        Caption = #1054#1087#1080#1089#1072#1085#1080#1077':'
      end
      object Label10: TLabel
        Left = 12
        Top = 108
        Width = 39
        Height = 13
        Caption = 'Subject:'
      end
      object edIssuerFirm: TEdit
        Left = 88
        Top = 20
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 0
      end
      object edIssuerName: TEdit
        Left = 88
        Top = 48
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 1
      end
      object edIssuerDescr: TEdit
        Left = 88
        Top = 76
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 2
      end
      object edIssuerSubject: TEdit
        Left = 88
        Top = 104
        Width = 365
        Height = 21
        ParentColor = True
        ReadOnly = True
        TabOrder = 3
      end
    end
    object btnSave: TButton
      Left = 396
      Top = 420
      Width = 75
      Height = 25
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100'...'
      TabOrder = 2
      OnClick = btnSaveClick
    end
  end
  object SaveDlg: TSaveDialog
    DefaultExt = 'cer'
    Filter = #1060#1072#1081#1083#1099' '#1089#1077#1088#1090#1080#1092#1080#1082#1072#1090#1086#1074' (*.cer)|*.cer'
    Title = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1089#1077#1088#1090#1080#1092#1080#1082#1072#1090
    Left = 24
    Top = 432
  end
end
