@echo off
rem builds test\fontrender.exe with Open Watcom 2 (run from the project root), log in test\build_fontrender.log
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
set EDPATH=%WATCOM%\EDDAT
wcc386 -zq -4r -bt=os2 -I%WATCOM%\h -IC:\os2tk45\h -fo=test\fontrender.obj test\fontrender.c > test\build_fontrender.log 2>&1
if errorlevel 1 goto failed
wlink name test\fontrender.exe system os2v2_pm option quiet file test\fontrender.obj library C:\os2tk45\lib\os2386.lib >> test\build_fontrender.log 2>&1
if errorlevel 1 goto failed
echo OK: test\fontrender.exe
exit 0
:failed
type test\build_fontrender.log
echo FAILED: fontrender
exit 1
