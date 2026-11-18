# Open Watcom 2.0 Makefile for FT2IFI (FT2IFI.DLL, the FreeType 2 based PM font driver)
# Run from the ft2ifi directory (compile_wat.cmd does this).
#
# FreeType 2 (lib\freetype-2.15.0, with the OS/2 IFI additions in builds\os2) is compiled into
# the DLL together with the driver. There is no C run-time start-up code: the DLL entry point
# is _DLL_InitTerm, memory comes from SSAllocMem and files from the Dos* calls (ifisystem.c).
# Needs the OS/2 Toolkit 4.5 headers and import libraries (TK) in addition to Open Watcom.

WATCOM = C:\WATCOM
TK = C:\os2tk45
CC = $(WATCOM)\binp\wcc386.exe
LINK = $(WATCOM)\binp\wlink.exe

FT = lib\freetype-2.15.0
BIN = bin
OBJ = bin\obj
DLLNAME = FT2IFI
DLL = $(BIN)\$(DLLNAME).dll
# the tool that sets the "single data" module flag is shared with the version 1 build
LXDATA = $(BIN)\lxdata.exe

# FreeType: -zp4 as the VisualAge default packing, no stack checks, DLL code, no C run-time calls beyond Watcom's clib
FTFLAGS = -zq -4r -Otexn -zp4 -bd -s -w1 -DFT2_BUILD_LIBRARY -I$(FT)\ifi -I$(FT)\builds\os2 -I$(FT)\include -I$(FT)\src -I$(WATCOM)\h -I$(TK)\h
IFIFLAGS = -zq -4r -zp4 -bd -s -w1 -D_ULS_CALLCONV_ -DCALLCONV=_System -I$(FT)\ifi -I$(FT)\builds\os2 -I$(FT)\include -Iifi -I$(WATCOM)\h -I$(TK)\h $(IFIEXTRA)

FTOBJS = $(OBJ)\ifisystem.obj $(OBJ)\ftinit.obj $(OBJ)\ftbase.obj $(OBJ)\ftbbox.obj $(OBJ)\ftbitmap.obj $(OBJ)\ftglyph.obj $(OBJ)\ftdebug.obj &
         $(OBJ)\autofit.obj $(OBJ)\truetype.obj $(OBJ)\cff.obj $(OBJ)\psnames.obj $(OBJ)\pshinter.obj $(OBJ)\raster.obj $(OBJ)\sfnt.obj $(OBJ)\psaux.obj

.c: $(FT)\builds\os2;$(FT)\src\base;$(FT)\src\autofit;$(FT)\src\truetype;$(FT)\src\cff;$(FT)\src\psnames;$(FT)\src\pshinter;$(FT)\src\raster;$(FT)\src\sfnt;$(FT)\src\psaux

all: $(DLL)

$(OBJ)\ft2ifi.obj: ifi\ft2ifi.c ifi\ft2ifi.h ifi\ft2kern.h ifi\ft2mem.h ifi\ft2debug.h
	@if not exist $(BIN) mkdir $(BIN)
	@if not exist $(OBJ) mkdir $(OBJ)
	$(CC) $(IFIFLAGS) -fo=$@ ifi\ft2ifi.c

$(OBJ)\ft2kern.obj: ifi\ft2kern.c ifi\ft2kern.h
	@if not exist $(OBJ) mkdir $(OBJ)
	$(CC) $(IFIFLAGS) -fo=$@ ifi\ft2kern.c

.c.obj:
	@if not exist $(BIN) mkdir $(BIN)
	@if not exist $(OBJ) mkdir $(OBJ)
	$(CC) $(FTFLAGS) -fo=$(OBJ)\$^. $[@

$(OBJ)\ifisystem.obj: $(FT)\builds\os2\ifisystem.c
	@if not exist $(BIN) mkdir $(BIN)
	@if not exist $(OBJ) mkdir $(OBJ)
	$(CC) $(FTFLAGS) -fo=$@ $(FT)\builds\os2\ifisystem.c

$(LXDATA): ..\tools\lxdata.c
	@if not exist $(BIN) mkdir $(BIN)
	@if not exist $(OBJ) mkdir $(OBJ)
	$(CC) -zq -4r -bt=os2 -I$(WATCOM)\h -fo=$(OBJ)\lxdata.obj ..\tools\lxdata.c
	$(LINK) name $@ system os2v2 option quiet file $(OBJ)\lxdata.obj

$(DLL): $(OBJ)\ft2ifi.obj $(OBJ)\ft2kern.obj $(FTOBJS) $(LXDATA)
	$(LINK) @<<
name $(DLL)
system os2v2_dll initinstance terminstance
option quiet,caseexact,map=$(BIN)\$(DLLNAME).map
option start=_DLL_InitTerm
option modname=$(DLLNAME)
segment type data shared
option description '@#Alexander Taylor:2.0.0r1#@FT2IFI - OpenType PM font driver based on FreeType 2'
file $(OBJ)\ft2ifi.obj
file $(OBJ)\ft2kern.obj
file { $(FTOBJS) }
library $(WATCOM)\lib386\os2\clib3r.lib
library $(TK)\lib\os2386.lib
library $(TK)\lib\libconv.lib
export FONT_DRIVER_DISPATCH_TABLE.1=_fdhdr
<<
	$(LXDATA) $(DLL)

clean: .SYMBOLIC
	@if exist $(BIN) rmdir /s /q $(BIN)
