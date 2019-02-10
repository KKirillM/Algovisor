unit acBase;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;

type
  TBaseForm = class(TForm)
  private
    { Private declarations }
  public
    constructor CreateNew(AOwner: TComponent; Dummy: Integer = 0); override;
  end;

function IniFileName: string;
function LogonIniFileName: string;

implementation

{$R *.DFM}

function IniFileName: string;
begin
  Result := ChangeFileExt(ParamStr(0), '.ini');
end;

function LogonIniFileName: string;
begin
  Result := ChangeFileExt(ParamStr(0), '.ini');
  Insert('Logon', Result, Length(Result) - 3);
end;

{ TBaseForm }

constructor TBaseForm.CreateNew(AOwner: TComponent; Dummy: Integer);
var
  NonClientMetrics: TNonClientMetrics;
begin
  inherited CreateNew(AOwner);
  NonClientMetrics.cbSize := SizeOf(NonClientMetrics);
  if SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, @NonClientMetrics, 0) then
    Font.Handle := CreateFontIndirect(NonClientMetrics.lfMessageFont);
end;

end.
 