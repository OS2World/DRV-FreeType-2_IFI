@echo off
rem compile_wat.cmd - build FreeType.dll with Open Watcom 2 (run from the project root)
rem   compile_wat.cmd         build, log in compile.log
rem   compile_wat.cmd clean   remove bin\ and compile.log
set WATCOM=C:\WATCOM
set PATH=%WATCOM%\BINP;%WATCOM%\BINW;%PATH%
set EDPATH=%WATCOM%\EDDAT

if not exist "%WATCOM%\BINP\WMAKE.EXE" goto missing_wmake
if "%1"=="clean" goto clean

echo Building FreeType/2 ...
"%WATCOM%\BINP\WMAKE.EXE" -a -z -f Makefile.wat > compile.log 2>&1
if errorlevel 1 goto build_failed
if not exist bin\FreeType.dll goto missing_output
echo OK: bin\FreeType.dll
dir bin\FreeType.dll
echo OK: bin\libttf.lib
dir bin\libttf.lib
echo Build log: compile.log
exit 0

:clean
"%WATCOM%\BINP\WMAKE.EXE" -f Makefile.wat clean
if exist compile.log del compile.log
echo OK: cleaned.
exit 0

:missing_wmake
echo FAILED: Open Watcom wmake was not found at %WATCOM%\BINP\WMAKE.EXE.
exit 1

:build_failed
type compile.log
echo FAILED: Open Watcom build failed. See compile.log.
exit 1

:missing_output
echo FAILED: bin\FreeType.dll was not produced. See compile.log.
exit 1
