@echo off
rem builds test\iftest.exe, a stand-alone test of the driver DLL (no PM needed): it imports the
rem dispatch table from the DLL named in %1 (default FTT, i.e. C:\OS2\DLL\FTT.DLL) and calls it.
rem run from the project root; log in test\build_iftest.log
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
set EDPATH=%WATCOM%\EDDAT
set DLLNAME=%1
if "%DLLNAME%"=="" set DLLNAME=FTT
wcc386 -zq -4r -zp1 -bt=os2 -D_ULS_CALLCONV_ -DCALLCONV=_System -I%WATCOM%\h -IC:\os2tk45\h -Ifreetype1\src\ifi -fo=test\iftest.obj test\iftest.c > test\build_iftest.log 2>&1
if errorlevel 1 goto failed
wlink name test\iftest.exe system os2v2 option quiet file test\iftest.obj import _fdhdr %DLLNAME%.FONT_DRIVER_DISPATCH_TABLE >> test\build_iftest.log 2>&1
if errorlevel 1 goto failed
echo OK: test\iftest.exe
exit 0
:failed
type test\build_iftest.log
echo FAILED: iftest
exit 1
