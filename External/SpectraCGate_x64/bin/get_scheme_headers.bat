@echo off
echo Please wait...

::создаём папку для заголовочных файлов если её еще нет
set schemeDIR=..\SDK\include\schemes
IF not exist %schemeDIR% (mkdir %schemeDIR%)

::запускаем роутер
rem start P2MQRouter64.exe /ini:plaza2_router.ini

::ждем пока он запустится
timeout /T 2 /NOBREAK > NUL

::создаём файлы схем в цикле
for /f %%x in (FORTS_REPL_NAMES.txt) do ( 
echo %%x
schemetool makesrc --output %schemeDIR%\%%x.h --output-format c --verbose --key 11111111 --conn "p2tcp://127.0.0.1:4001;app_name=stool" --stream %%x --scheme_name %%x -Dget-scheme-string=1 -Dgen-namespaces=1)
:завершаем процесс роутера
rem taskkill /IM "P2MQRouter64.exe"

rem localhost:4001 p2tcp://85.118.179.71:20001