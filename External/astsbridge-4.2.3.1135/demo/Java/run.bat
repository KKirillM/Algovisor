@echo off
setlocal
setlocal enabledelayedexpansion

set MODE=embedded
rem set MODE=mtesrl

if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    set "PATH=%PATH%;..\..\%MODE%\win64;.\lib\win64"
) else (
    set "PATH=%PATH%;..\..\%MODE%\win32;.\lib\win32"
)

set CP="."
for %%F in ("./lib/"*.jar) do set CP=!CP!;"./lib/%%~nxF"

@echo on
javac -classpath %CP% Demo.java

java -classpath %CP% Demo

pause
