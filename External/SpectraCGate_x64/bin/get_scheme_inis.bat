@echo off
echo Please wait...

::������ ����� ��� ������������ ������ ���� � ��� ���
set schemeDIR=..\SDK\ini
IF not exist %schemeDIR% (mkdir %schemeDIR%)

::��������� ������
start P2MQRouter64.exe /ini:client_router.ini

::���� ���� �� ����������
timeout /T 2 /NOBREAK > NUL

::������ ����� ���� � �����
for /f %%x in (FORTS_REPL_NAMES.txt) do ( 
schemetool makesrc --output %schemeDIR%\%%x.ini --output-format ini --verbose --key 11111111 --conn "p2tcp://localhost:4001;app_name=stool" --stream %%x --scheme_name %%x -Dget-scheme-string=1)