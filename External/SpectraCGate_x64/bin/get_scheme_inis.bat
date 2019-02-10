@echo off
echo Please wait...

::создаём папку для заголовочных файлов если её еще нет
set schemeDIR=..\SDK\ini
IF not exist %schemeDIR% (mkdir %schemeDIR%)

::запускаем роутер
start P2MQRouter64.exe /ini:client_router.ini

::ждем пока он запустится
timeout /T 2 /NOBREAK > NUL

::создаём файлы схем в цикле
for /f %%x in (FORTS_REPL_NAMES.txt) do ( 
schemetool makesrc --output %schemeDIR%\%%x.ini --output-format ini --verbose --key 11111111 --conn "p2tcp://localhost:4001;app_name=stool" --stream %%x --scheme_name %%x -Dget-scheme-string=1)