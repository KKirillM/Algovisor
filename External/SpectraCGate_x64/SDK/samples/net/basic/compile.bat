@echo off

if exist "%CGATE_HOME%\bin\cgate.dll" goto cgate_exist

echo ERROR: cgate.dll not found. Terminating.
goto finish

:cgate_exist

if exist "%CGATE_HOME%\bin\cgate_net.dll" goto cgate_net_exist

echo ERROR: cgate_net.dll not found. Terminating.
goto finish

:cgate_net_exist

if exist "%VS110COMNTOOLS%vsvars32.bat" goto use_vs110
if exist "%VS90COMNTOOLS%vsvars32.bat" goto use_vs90

echo ERROR: Neither MSVS2005 nor MSVS2008 found. Terminating
goto finish

:use_vs110
call "%VS110COMNTOOLS%vsvars32.bat"
goto build

:use_vs90
call "%VS90COMNTOOLS%vsvars32.bat"
goto build

:build

xcopy "%CGATE_HOME%\bin\cgate.dll" "%~dp0" /Y 
xcopy "%CGATE_HOME%\bin\cgate_net.dll" "%~dp0" /Y

csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:nrepl.exe /platform:x86 repl.cs 
csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:nsend.exe /platform:x86 send.cs
csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:np2sys.exe /platform:x86 p2sys.cs
csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:nsend_mt.exe /platform:x86 send_mt.cs
csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:nordbook.exe /platform:x86 orderbook.cs
csc /r:"%CGATE_HOME%\bin\cgate_net.dll" /target:exe /out:naggrspy.exe /platform:x86 aggrspy.cs

:finish

