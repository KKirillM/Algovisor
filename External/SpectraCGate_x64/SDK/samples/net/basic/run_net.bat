set PATH=%CGATE_HOME%\p2bin;%PATH%

xcopy "%CGATE_HOME%\p2bin\cgate64.dll" "%~dp0" /Y 
xcopy "%CGATE_HOME%\p2bin\cgate_net64.dll" "%~dp0" /Y

nsend64
rem naggrspy64 259726 50 asout.txt
rem nordbook64 259726 50 obout.txt
rem np2sys64 FORTS_HB00_yurasov1 12345
rem nrepl64 0
rem nsend64
rem nsend_mt64
