@echo off
rem builds test\fontlist.exe with Open Watcom 2 (run from the project root), log in test\build_fontlist.log
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
set EDPATH=%WATCOM%\EDDAT
wcc386 -zq -4r -bt=os2 -I%WATCOM%\h -IC:\os2tk45\h -fo=test\fontlist.obj test\fontlist.c > test\build_fontlist.log 2>&1
if errorlevel 1 goto failed
wlink name test\fontlist.exe system os2v2_pm option quiet file test\fontlist.obj library C:\os2tk45\lib\os2386.lib >> test\build_fontlist.log 2>&1
if errorlevel 1 goto failed
echo OK: test\fontlist.exe
exit 0
:failed
type test\build_fontlist.log
echo FAILED: fontlist
exit 1
