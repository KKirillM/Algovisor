program TEClient;

uses
  Forms,
  AcMain in 'AcMain.pas' {MainForm},
  AcLogon in 'AcLogon.pas' {LogonForm},
  AcTblFrm in 'AcTblFrm.pas' {TableForm},
  AcStruc in 'AcStruc.pas',
  acBase in 'acBase.pas' {BaseForm},
  acExport in 'acExport.pas',
  acFilter in 'acFilter.pas' {FilterForm},
  acCertInfo in 'acCertInfo.pas' {CertInfoForm},
  acInput in 'acInput.pas' {InputForm};

{$R *.RES}

begin
  Application.Initialize;
  Application.HelpFile := '';
  Application.Title := 'TE Client 2';
  Application.CreateForm(TMainForm, MainForm);
  Application.Run;
end.
