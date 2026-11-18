# Changes from the original FreeType/2

This file describes version 1 (FreeType 1.3.1 based, release 1.3.6r2). The FreeType 2.15.0 based driver (release 2.15.0a1, branch `ft2ifi`) has its own notes:
`ft2ifi\README.md` and `ft2ifi\lib\RESYNC.md` in the `ft2ifi\` directory.

Summary of version 1.3.6r2 (released 2026-11-18, git tag `1.3.6r2`) against FreeType/2 1.3.6 as shipped with ArcaOS:

- Same source (upstream `legacy/` 1.3.6), built with Open Watcom 2 instead of IBM VisualAge (`compile_wat.cmd`, `Makefile.wat`).
- The four IBM/Microsoft IFI headers (`src\ifi\32*.h`) are clean-room replacements.
- Source changes: compiler guard and dispatch table casts in `ftifi.c`; the code page range 2 masks in `ftifi.h` are 0 as with the
  VisualAge build (see "Verification" below); `ftconfig.exe` is bound with `rc -n`.
- Link settings that matter: entry point `_DLL_InitTerm`, data object shared plus the "single data" module flag (`..\tools\lxdata.c`),
  and the internal module name equal to the file name (`MODNAME`).
- Tested on the ArcaOS Dev VM: output identical to the shipped DLL (112 outline fonts in PM, 185 font files at driver level), soak and
  parallel runs clean, installed with `UPDATE.CMD` and rolled back. Not tested: real hardware, `INSTALL.CMD`, `UNINST.CMD`, saving
  settings and the help files of `ftconfig.exe`.

Original state: tag `upstream-original` (FreeType/2 1.3.4 snapshot). The tree was then re-based on the
upstream project, https://github.com/altsan/os2-freetype-ifi (`legacy/` at tag v1.3.6, 2020-03-23), imported
unchanged into the standard layout (commit "Import FreeType/2 1.3.6 from upstream").

## Build

- `Makefile.wat`, `compile_wat.cmd`, `.gitignore`: build with Open Watcom 2 (`wcc386` + `wlink` + Toolkit `rc`
  and `ipfc`). Outputs in `bin\`: `FreeType.dll`, `libttf.lib`, `ftconfig.exe`, `ftmem.exe`, `ftconfig.hlp`,
  `ftconfde.hlp`, `ftconfes.hlp`. Log `compile.log`.
- `src\ifi\ftifi.c`: accepts Open Watcom besides VisualAge; the dispatch table entries get casts (Watcom reads the
  IBM `_syscall` function-pointer types differently); the `#pragma export` is VisualAge only, the export is done in
  `Makefile.wat` (`FONT_DRIVER_DISPATCH_TABLE.1=_fdhdr`). No logic change.
- Flags: `-4r -zp1 -bd -zc -s`, `-D_ULS_CALLCONV_ -DCALLCONV=_System`. The build still needs the OS/2 Toolkit 4.5
  headers and libraries (`os2386.lib` for SSAllocMem, `libconv.lib` for Uni*).
- BLDLEVEL `@#Necasek/Seo/KO/Taylor et al:1.3.6r2#@FreeType/2 installable font interface driver`.
- Result on the Dev VM: all outputs built, 0 compile errors; `FreeType.dll` about 101 KB (the shipped ArcaOS DLL
  is 82,402 bytes, built with VisualAge and exepack). Same imports (DOSCALLS, uconv, PMSHAPI, NLS, PMGRE) and the
  same single export (ordinal 1, `FONT_DRIVER_DISPATCH_TABLE`).
- Install test: see the section below.

## Documentation

- `README.md` rewritten to the project template; `LICENSE` is the FreeType Project License (`install\FTL.TXT`).

## Clean-room replacement of the IBM IFI headers

- `src\ifi\32pmifi.h`, `32fdstrc.h`, `32ifimet.h`, `32fddef.h` (IBM/Microsoft copyright) are rewritten from scratch by an
  agent that did not read the originals. Sources: `doc\IFI32.PDF`, the way `ftifi.c` uses the identifiers, and the
  layout facts (offsets, sizes, flag values) read from the metrics dumps `arial_*.hex` and from immediates of a built
  `FreeType.dll`. Where the documentation is wrong or silent the header says so.
- Layout checked: IFIMETRICS 254 bytes (compile-time asserts), FDHEADER 72, FDDISPATCH 11 slots. `FreeType.dll` is
  built with the new headers: same size as with the old ones, 0 errors.
- After the rewrite the maintainer compared the doubtful values with the old headers (verification only): one value was
  wrong (`IFIMETRICS_MBCS` is 0x18) and was corrected; `panose` is 10 bytes plus 2 reserved bytes, same layout as the
  original 12-byte field.
- Remaining doubts: `IFIMETRICS_ITALIC`, `_UDC`, `_ANTI_ALIAS`, `_NO_CACHE`, `_ATOM_NAMES` and `FD_CHARATTR_*` are not
  defined because nothing uses them. The old headers stay in the git history (tag `upstream-original`).

## Install test (ArcaOS Dev VM, 2026-11-17 / 18)

Method: the DLL is copied under a test name next to the shipped one, `PM_Font_Drivers` / `TRUETYPE` in `OS2.INI` is pointed at
it, the VM is rebooted, and `test\fontrender.exe` renders a text with every outline font into a memory bitmap and prints
metrics, pixel counts and a checksum per font and size (`test\render_ship.txt` = shipped `FREETYPE.DLL`, `test\render_ow.txt` = this build).

Result: **identical to the shipped DLL**: 112 outline fonts, same text boxes, metrics, pixel counts and checksums for all fonts at
8, 12, 24 and 40 pt (`diff` of the two files is empty). Not tested: real hardware, OpenType/CFF, other code pages, the install scripts.

Problems found on the way (all fixed):

- Open Watcom's DLL start-up code was the entry point, so `_DLL_InitTerm` (it starts the FreeType engine) never ran and no TrueType
  font loaded (21 outline fonts, all Type 1). Fixed: `option start=_DLL_InitTerm`.
- **The internal module name of the DLL must equal the file name it is installed under.** Test copies named `FTOW.DLL`, `FTOWT.DLL`
  etc. that kept the internal name `FreeType` made every PM process that used a TrueType font trap in `List_Find`; even a copy of the
  shipped `FREETYPE.DLL` under the name `FTS.DLL` did the same. A build with `option modname=FTOWX` installed as `FTOWX.DLL`
  works. `Makefile.wat` has `MODNAME = FREETYPE` (override with `wmake MODNAME=name` for test copies). The earlier theories (data not
  shared between processes) were wrong; the data-sharing settings below are kept because the original DLL has `DATA SINGLE SHARED`,
  but it was not tested whether they are needed.
- `segment type data shared` and the module flag "single data" set after linking by `..\tools\lxdata.c` (the Watcom linker has no switch
  for it in LX files) reproduce the original `DATA SINGLE SHARED`.
- Handy: `test\iftest.c`, `iftest2.c`, `iftest3.c` test the driver without PM (one process, two processes, DosLoadModule);
  `IFIEXTRA="-DFTIFI_TRACE -za99"` builds a trace version (`\FTIFI.LOG`, `\FTIFIP.LOG` per process) which can hang the Workplace Shell.
- If PM hangs after a bad driver: reset the VM, press Alt+F1 right after the reset, F2 (command line), rename the DLL registered under
  `TRUETYPE`, reboot, restore the key.

## Install test with the package scripts (ArcaOS Dev VM, 2026-11-18)

- Backed up the shipped `C:\OS2\DLL\FREETYPE.DLL` (82,402 bytes, 1.3.6 from 2020-03-23) to `C:\FTBACKUP`, staged our `FREETYPE.DLL`
  (built with the default `MODNAME = FREETYPE`) with the `install\*.CMD` scripts, and ran the package's own `UPDATE.CMD`
  (it calls `repmod`, which replaces the locked DLL; the ArcaOS copy of `repmod.exe` is in `C:\sys\apps\ftifi`).
- After the reboot: `C:\OS2\DLL\FREETYPE.DLL` is our 101,469 byte build, `QUERY.CMD` reports `\OS2\DLL\FREETYPE.DLL` as the TrueType
  driver, PM starts normally, and `test\fontrender.exe` gives output byte-identical to the shipped DLL (112 outline fonts).
- Rollback: `repmod FREETYPE.DLL C:\os2\dll\FREETYPE.DLL` from `C:\FTBACKUP`, reboot: the shipped DLL is back and renders as before.
- `INSTALL.CMD` (first install: copies the DLL, creates the settings object, writes the `TRUETYPE` key) was not run: the
  key already points to `FREETYPE.DLL` on ArcaOS, and `copy` cannot overwrite the loaded DLL. `ftconfig.exe` was started
  from an ssh session but no window could be seen, so the configuration tool is untested. `UNINST.CMD` not run.

## Verification of version 1.3.6r2 (2026-11-18)

Version string: BLDLEVEL `@#Necasek/Seo/KO/Taylor et al:1.3.6r2#@FreeType/2 installable font interface driver` (`bldlevel` shows revision
`1.03.6r2`).

Driver-level comparison without PM, shipped `FREETYPE.DLL` against this build, same machine, same 185 TrueType files
(`ftlist.txt`: every `.TTF`/`.TTC` on the Dev VM, 194 faces, 7 TTC faces). `test\ifidump.c` (build with `test\build_ifidump.cmd <dll> <exe>`) calls
the IFI dispatch table of each DLL and writes a report; the two reports are **identical** (7,787 lines, `test\dump_reference.txt`):
- IFIMETRICS (all 254 bytes, checksum) of every face, QueryFullFaces (184 files), ABC widths of 256 glyphs, kerning pairs (54 faces),
- bitmaps (2,790) and outlines (2,796) of 8 sample glyphs at two sizes for every face.
Found and fixed by this comparison: 57 faces differed in one byte, `fsCapabilities` bit 0x20 (PC charset). Upstream wrote the code page range 2
masks as `(1 << 48)` ... `(1 << 63)` (`ftifi.h`), which overflows a 32 bit int: IBM VisualAge evaluates them to 0, Open Watcom wraps the shift
count and made them match. They are 0 now (with a comment), which reproduces the shipped behaviour; the intended values are `(1 << (n - 32))`
(a possible later change that alters font charset flags). This also removes the eight W135 compiler warnings.

Soak: `ifidump <list> <out> soak <n>` loads every font, opens contexts, queries and renders, and unloads, n times. 300 rounds (55,500 file loads):
both DLLs finish and both lose the same amount of free memory (about 20 MB, available memory counter), so this build leaks no more than the
shipped one (the growth is in the shared engine or in the test program, not in the Open Watcom port). Three processes at once, 100 rounds each:
no trap, no entry in `POPUPLOG.OS2`.

Configuration tool: **`ftconfig.exe` bound with `rc -x` does not start on ArcaOS (SYS2070, `DRGTXT39`, error 193)**; without `-x` it starts and shows the
Driver/Settings/About notebook. `Makefile.wat` binds with `rc -n` now (the exe is 5 KB larger). `ftmem.exe` starts (it reports that FreeType/2 is not running,
which is right: memory tracking is not compiled in). Settings saving and the help files were not exercised.
