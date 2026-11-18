# Open Watcom 2.0 Makefile for FreeType/2 (FreeType.dll, the PM TrueType font driver)
# Run from the project root (compile_wat.cmd does this).
#
#   bin\libttf.lib   the FreeType 1.3.1 library (single object + extensions)
#   bin\FreeType.dll the installable font interface (IFI) driver
#
# Needs the OS/2 Toolkit 4.5 headers and import libraries (TK = C:\os2tk45; os2.h, pmddi.h,
# uconv.h, os2386.lib with the PM driver interface (SSAllocMem ...) and libconv.lib) in addition
# to Open Watcom. CALLCONV is defined here because the Toolkit header only defines it for IBM compilers.

WATCOM = C:\WATCOM
TK = C:\os2tk45
CC = $(WATCOM)\binp\wcc386.exe
LINK = $(WATCOM)\binp\wlink.exe
LIBR = $(WATCOM)\binp\wlib.exe

PROJ_ROOT = .
LIB_DIR = $(PROJ_ROOT)\src\lib
IFI_DIR = $(PROJ_ROOT)\src\ifi
BIN_DIR = $(PROJ_ROOT)\bin
OBJ_DIR = $(BIN_DIR)\obj

# -4r 486 register calling, -zp1 pack structures on byte boundaries (important), -bd DLL code,
# -zc strings in the code segment, -s no stack checks (the original was built without run-time: -Rn)
LIBFLAGS = -zq -4r -Otexan -zp1 -bd -zc -s -w1 -I$(LIB_DIR)\arch\os2 -I$(LIB_DIR) -I$(LIB_DIR)\extend -I$(WATCOM)\h -I$(TK)\h
IFIFLAGS = -zq -4r -zp1 -bd -zc -s -w1 $(IFIEXTRA) -D_ULS_CALLCONV_ -DCALLCONV=_System -I$(LIB_DIR) -I$(LIB_DIR)\extend -I$(LIB_DIR)\arch\os2 -I$(IFI_DIR) -I$(WATCOM)\h -I$(TK)\h

EXTOBJS = $(OBJ_DIR)\ftxgasp.obj $(OBJ_DIR)\ftxkern.obj $(OBJ_DIR)\ftxpost.obj $(OBJ_DIR)\ftxcmap.obj $(OBJ_DIR)\ftxwidth.obj $(OBJ_DIR)\ftxsbit.obj
LIBOBJ = $(OBJ_DIR)\freetype.obj
TTF = $(BIN_DIR)\libttf.lib
DLL = $(BIN_DIR)\FreeType.dll

all: $(DLL) $(BIN_DIR)\ftconfig.exe $(BIN_DIR)\ftmem.exe $(BIN_DIR)\ftconfig.hlp $(BIN_DIR)\ftconfde.hlp $(BIN_DIR)\ftconfes.hlp

$(DLL): $(OBJ_DIR)\ftifi.obj $(TTF) $(IFI_DIR)\FreeType.def $(BIN_DIR)\lxdata.exe
	$(LINK) @<<
name $(DLL)
system os2v2_dll initinstance terminstance
option quiet,caseexact,map=$(BIN_DIR)\FreeType.map
option start=_DLL_InitTerm
segment type data shared
option description '@#Necasek/Seo/KO/Taylor et al:1.3.6#@FreeType/2 installable font interface driver'
file $(OBJ_DIR)\ftifi.obj
library $(TTF)
library $(WATCOM)\lib386\os2\clib3r.lib
library $(TK)\lib\os2386.lib
library $(TK)\lib\libconv.lib
export FONT_DRIVER_DISPATCH_TABLE.1=_fdhdr
<<
	$(BIN_DIR)\lxdata.exe $(DLL)

# the linker has no DATA SINGLE for LX files: a tiny tool sets the module flag after linking
$(BIN_DIR)\lxdata.exe: tools\lxdata.c
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) -zq -4r -bt=os2 -I$(WATCOM)\h -fo=$(OBJ_DIR)\lxdata.obj tools\lxdata.c
	$(LINK) name $@ system os2v2 option quiet file $(OBJ_DIR)\lxdata.obj

$(TTF): $(LIBOBJ) $(EXTOBJS)
	@if exist $(TTF) del $(TTF)
	$(LIBR) -q -n -c $(TTF) +$(LIBOBJ) +$(OBJ_DIR)\ftxgasp.obj +$(OBJ_DIR)\ftxkern.obj +$(OBJ_DIR)\ftxpost.obj +$(OBJ_DIR)\ftxcmap.obj +$(OBJ_DIR)\ftxwidth.obj +$(OBJ_DIR)\ftxsbit.obj

$(OBJ_DIR)\ftifi.obj: $(IFI_DIR)\ftifi.c $(IFI_DIR)\ftifi.h
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(IFIFLAGS) -fo=$@ $(IFI_DIR)\ftifi.c

$(OBJ_DIR)\freetype.obj: $(LIB_DIR)\arch\os2\freetype.c
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\arch\os2\freetype.c

$(OBJ_DIR)\ftxgasp.obj: $(LIB_DIR)\extend\ftxgasp.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxgasp.c

$(OBJ_DIR)\ftxkern.obj: $(LIB_DIR)\extend\ftxkern.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxkern.c

$(OBJ_DIR)\ftxpost.obj: $(LIB_DIR)\extend\ftxpost.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxpost.c

$(OBJ_DIR)\ftxcmap.obj: $(LIB_DIR)\extend\ftxcmap.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxcmap.c

$(OBJ_DIR)\ftxwidth.obj: $(LIB_DIR)\extend\ftxwidth.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxwidth.c

$(OBJ_DIR)\ftxsbit.obj: $(LIB_DIR)\extend\ftxsbit.c
	$(CC) $(LIBFLAGS) -fo=$@ $(LIB_DIR)\extend\ftxsbit.c


# ---- configuration tool, memory viewer and help file ------------------------------------------
CFG_DIR = $(PROJ_ROOT)\src\config
RC = $(TK)\bin\rc.exe
IPFC = $(TK)\bin\ipfc.exe
APPFLAGS = -zq -4r -zp1 -s -w1 -I$(CFG_DIR) -I$(IFI_DIR) -I$(WATCOM)\h -I$(TK)\h

$(BIN_DIR)\ftconfig.exe: $(OBJ_DIR)\ft2conf.obj $(OBJ_DIR)\resource.res
	$(LINK) @<<
name $(BIN_DIR)\ftconfig.exe
system os2v2_pm
option quiet,caseexact,map=$(BIN_DIR)\ftconfig.map
file $(OBJ_DIR)\ft2conf.obj
library $(TK)\lib\os2386.lib
<<
	$(RC) -x -n $(OBJ_DIR)\resource.res $(BIN_DIR)\ftconfig.exe

$(OBJ_DIR)\ft2conf.obj: $(CFG_DIR)\ft2conf.c $(CFG_DIR)\ft2conf.h $(CFG_DIR)\resource.h
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(APPFLAGS) -fo=$@ $(CFG_DIR)\ft2conf.c

$(OBJ_DIR)\resource.res: $(CFG_DIR)\resource.rc $(CFG_DIR)\resource.dlg $(CFG_DIR)\resource.h $(CFG_DIR)\ft2gui.ICO $(CFG_DIR)\ftifi.bmp
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(RC) -r -n -i $(CFG_DIR) -i $(TK)\h $(CFG_DIR)\resource.rc $@

$(BIN_DIR)\ftconfig.hlp: $(CFG_DIR)\ftconfig_en.ipf
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(IPFC) $(CFG_DIR)\ftconfig_en.ipf $@

$(BIN_DIR)\ftconfde.hlp: $(CFG_DIR)\ftconfig_de.ipf
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(IPFC) $(CFG_DIR)\ftconfig_de.ipf $@

$(BIN_DIR)\ftconfes.hlp: $(CFG_DIR)\ftconfig_es.ipf
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(IPFC) $(CFG_DIR)\ftconfig_es.ipf $@

$(BIN_DIR)\ftmem.exe: $(OBJ_DIR)\ftmem.obj
	$(LINK) @<<
name $(BIN_DIR)\ftmem.exe
system os2v2_pm
option quiet,caseexact,stack=32k
file $(OBJ_DIR)\ftmem.obj
library $(TK)\lib\os2386.lib
<<

$(OBJ_DIR)\ftmem.obj: $(IFI_DIR)\ftmem.c
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) $(APPFLAGS) -fo=$@ $(IFI_DIR)\ftmem.c

clean: .SYMBOLIC
	@if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
