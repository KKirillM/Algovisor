unit acCertInfo;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, acBase, ComCtrls, StdCtrls, MTEApi, mteTypes, ExtCtrls;

type
  TCertInfoForm = class(TBaseForm)
    btnClose: TButton;
    tcCerts: TTabControl;
    Label1: TLabel;
    Label7: TLabel;
    gbOwner: TGroupBox;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    edOwnerFirm: TEdit;
    edOwnerName: TEdit;
    edOwnerDescr: TEdit;
    edOwnerSubject: TEdit;
    GroupBox1: TGroupBox;
    Label2: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    edIssuerFirm: TEdit;
    edIssuerName: TEdit;
    edIssuerDescr: TEdit;
    edIssuerSubject: TEdit;
    lblValidFrom: TLabel;
    lblValidTo: TLabel;
    btnSave: TButton;
    SaveDlg: TSaveDialog;
    Label11: TLabel;
    lblKeyValidFrom: TLabel;
    Label13: TLabel;
    lblKeyValidTo: TLabel;
    imgStatus: TImage;
    lblStatus: TLabel;
    procedure tcCertsChange(Sender: TObject);
    procedure btnSaveClick(Sender: TObject);
  private
    FClientCert,
    FServerCert,
    FCurrentCert: PMTEConnCertificate;
    FConnStats: PMTEConnStats;
    procedure ShowCertificate(Cert: PMTEConnCertificate);
  public
    procedure SetCertificates(AClientCert, AServerCert: PMTEConnCertificate; AConnStats: PMTEConnStats);
  end;

implementation

{$R *.dfm}

{ TCertInfoForm }

procedure TCertInfoForm.SetCertificates(AClientCert,
  AServerCert: PMTEConnCertificate; AConnStats: PMTEConnStats);
begin
  FClientCert := AClientCert;
  FServerCert := AServerCert;
  FConnStats := AConnStats;
  tcCertsChange(tcCerts);
end;

procedure TCertInfoForm.ShowCertificate(Cert: PMTEConnCertificate);
const
  IconIndexes1: array [Boolean] of Integer = (OIC_ERROR, OIC_INFORMATION);
  IconIndexes2: array [Boolean] of Integer = (OIC_WARNING, OIC_ERROR);
  SCertTypes: array [CERTTYPE_VALIDATA..CERTTYPE_OPENSSL] of string =
    ('Валидата', 'OpenSSL');
  SNoCertErrors: array [Boolean] of string =
    ('Шифрование и ЭЦП не используется, сертификат неизвестен',
     'ОШИБКА: Шифрование или ЭЦП включены, но сертификат получить не удается!');
var
  S: string;
  CertOk, SecureChannel: Boolean;
  IconIndex: Integer;
begin
  if Cert.CertType <> CERTTYPE_UNKNOWN then
  begin
    if Cert.CertRevoked <> 0 then
      S := 'Сертификат отозван'
    else
      S := '';

    if Cert.CertExpired <> 0 then
    begin
      if S <> '' then S := S + ', ';
      S := S + 'Сертификат или личный ключ просрочен';
    end;
    CertOk := S = '';
    if CertOk then S := 'Сертификат и личный ключ действительны';
    lblStatus.Caption := 'Тип сертификата: ' + SCertTypes[Cert.CertType] + #13#10 + S;
    IconIndex := IconIndexes1[CertOk];

    edOwnerFirm.Text := Cert.Owner.OrganizationName;
    edOwnerName.Text := Cert.Owner.Surname;
    edOwnerDescr.Text := Cert.Owner.Description;
    edOwnerSubject.Text := Cert.Owner.Subject;

    edIssuerFirm.Text := Cert.Issuer.OrganizationName;
    edIssuerName.Text := Cert.Issuer.Surname;
    edIssuerDescr.Text := Cert.Issuer.Description;
    edIssuerSubject.Text := Cert.Issuer.Subject;

    lblValidFrom.Caption := FormatDateTime('dddddd', SystemTimeToDateTime(Cert.ValidFrom));
    lblValidTo.Caption := FormatDateTime('dddddd', SystemTimeToDateTime(Cert.ValidTo));
    lblKeyValidFrom.Caption := FormatDateTime('dddddd', SystemTimeToDateTime(Cert.PrivateValidFrom));
    lblKeyValidTo.Caption := FormatDateTime('dddddd', SystemTimeToDateTime(Cert.PrivateValidTo));
  end
  else
  begin
    SecureChannel := FConnStats.Properties and (FLAG_ENCRYPTED or FLAG_SIGNING_ON) <> 0;
    IconIndex := IconIndexes2[SecureChannel];
    lblStatus.Caption := SNoCertErrors[SecureChannel];
    edOwnerFirm.Text := '';
    edOwnerName.Text := '';
    edOwnerDescr.Text := '';
    edOwnerSubject.Text := '';

    edIssuerFirm.Text := '';
    edIssuerName.Text := '';
    edIssuerDescr.Text := '';
    edIssuerSubject.Text := '';

    lblValidFrom.Caption := 'N/A';
    lblValidTo.Caption := 'N/A';
    lblKeyValidFrom.Caption := 'N/A';
    lblKeyValidTo.Caption := 'N/A';
  end;

  imgStatus.Picture.Icon.Handle := LoadImage(0, PChar(IconIndex), IMAGE_ICON, 32, 32, LR_SHARED);
  btnSave.Enabled := Assigned(Cert) and (Cert.CertEncoded.len > 0);
end;

procedure TCertInfoForm.tcCertsChange(Sender: TObject);
begin
  inherited;
  case tcCerts.TabIndex of
    0: FCurrentCert := FServerCert;
    else FCurrentCert := FClientCert;
  end;
  ShowCertificate(FCurrentCert);
end;

procedure TCertInfoForm.btnSaveClick(Sender: TObject);
begin
  if SaveDlg.Execute then
    with TFileStream.Create(SaveDlg.FileName, fmCreate) do
    try
      WriteBuffer(FCurrentCert.CertEncoded.buf^, FCurrentCert.CertEncoded.len);
    finally
      Free;
    end;
end;

end.
