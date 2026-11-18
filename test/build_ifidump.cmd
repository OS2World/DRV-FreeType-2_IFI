@echo off
rem build_ifidump.cmd <dllname> <exename>: builds test\<exename>.exe linked to <dllname>.DLL (run from the project root)
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
wcc386 -zq -4r -zp1 -bt=os2 -D_ULS_CALLCONV_ -DCALLCONV=_System -I%WATCOM%\h -IC:\os2tk45\h -Ifreetype1\src\ifi -fo=test\%2.obj test\ifidump.c > test\%2.log 2>&1
if errorlevel 1 goto failed
wlink name test\%2.exe system os2v2 option quiet file test\%2.obj import _fdhdr %1.FONT_DRIVER_DISPATCH_TABLE >> test\%2.log 2>&1
if errorlevel 1 goto failed
echo OK: test\%2.exe
exit 0
:failed
type test\%2.log
echo FAILED: %2
exit 1
