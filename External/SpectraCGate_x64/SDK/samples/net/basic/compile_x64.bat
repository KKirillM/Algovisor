@echo off
if exist "%CGATE_HOME%\bin\cgate64.dll" goto cgate_exist

echo ERROR: cgate64.dll not found. Terminating.
goto finish

:cgate_exist

if exist "%CGATE_HOME%\bin\cgate_net64.dll" goto cgate_net_exist

echo ERROR: cgate_net64.dll not found. Terminating.
goto finish

:cgate_net_exist

if exist "%VS110COMNTOOLS%../../VC/bin/amd64/vcvars64.bat" goto use_vs110
if exist "%VS90COMNTOOLS%../../VC/bin/amd64/vcvarsamd64.bat" goto use_vs90

echo ERROR: Neither MSVS2005 nor MSVS2008 found. Terminating
goto finish

:use_vs110
call "%%VS110COMNTOOLS%%../../VC/bin/amd64/vcvars64.bat"
goto build

:use_vs90
call "%%VS90COMNTOOLS%%../../VC/bin/amd64/vcvarsamd64.bat"
goto build

:build

xcopy "%CGATE_HOME%\bin\cgate64.dll" "%~dp0" /Y 
xcopy "%CGATE_HOME%\bin\cgate_net64.dll" "%~dp0" /Y

csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:nrepl64.exe /platform:x64 repl.cs 
csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:nsend64.exe /platform:x64 send.cs
csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:np2sys64.exe /platform:x64 p2sys.cs
csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:nsend_mt64.exe /platform:x64 send_mt.cs
csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:nordbook64.exe /platform:x64 orderbook.cs
csc /r:"%CGATE_HOME%\bin\cgate_net64.dll" /target:exe /out:naggrspy64.exe /platform:x64 aggrspy.cs

:finish

