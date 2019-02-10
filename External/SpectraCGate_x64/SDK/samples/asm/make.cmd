@echo off

set FileName=repl

del .\exe\%FileName%.exe > nul
C:\masm32\bin\ml /c /coff %FileName%.asm
C:\masm32\bin\Link /SUBSYSTEM:CONSOLE %FileName%.obj
copy %FileName%.exe .\exe\%FileName%.exe > nul
if exist %FileName%.obj del %FileName%.obj > nul
if exist %FileName%.exe del %FileName%.exe > nul

set FileName=send

del .\exe\%FileName%.exe > nul
C:\masm32\bin\ml /c /coff %FileName%.asm
C:\masm32\bin\Link /SUBSYSTEM:CONSOLE %FileName%.obj
copy %FileName%.exe .\exe\%FileName%.exe > nul
if exist %FileName%.obj del %FileName%.obj > nul
if exist %FileName%.exe del %FileName%.exe > nul
