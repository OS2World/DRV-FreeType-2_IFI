@echo off
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
wcc386 -zq -4r -bt=os2 -I%WATCOM%\h -IC:\os2tk45\h -fo=test\iftest3.obj test\iftest3.c > test\build_iftest3.log 2>&1
if errorlevel 1 goto failed
wlink name test\iftest3.exe system os2v2 option quiet file test\iftest3.obj >> test\build_iftest3.log 2>&1
if errorlevel 1 goto failed
echo OK: test\iftest3.exe
exit 0
:failed
type test\build_iftest3.log
echo FAILED: iftest3
exit 1
