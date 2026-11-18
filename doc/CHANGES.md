# Changes from the original FreeType/2

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
- BLDLEVEL `@#Necasek/Seo/KO/Taylor et al:1.3.6#@FreeType/2 installable font interface driver`.
- Result on the Dev VM: all outputs built, 0 compile errors; `FreeType.dll` 101,469 bytes (the shipped ArcaOS DLL
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
  built with the new headers: same size (101,469 bytes), 0 errors.
- After the rewrite the maintainer compared the doubtful values with the old headers (verification only): one value was
  wrong (`IFIMETRICS_MBCS` is 0x18) and was corrected; `panose` is 10 bytes plus 2 reserved bytes, same layout as the
  original 12-byte field.
- Remaining doubts: `IFIMETRICS_ITALIC`, `_UDC`, `_ANTI_ALIAS`, `_NO_CACHE`, `_ATOM_NAMES` and `FD_CHARATTR_*` are not
  defined because nothing uses them. The old headers stay in the git history (tag `upstream-original`).

## Install test (ArcaOS Dev VM, 2026-11-17): NOT working yet

What was found, in order:

- First test: PM started and listed fonts, but only 21 outline fonts (the Type 1 ones) against 112 with the shipped
  `FREETYPE.DLL`: the driver loaded **no** TrueType font. Cause: Open Watcom's DLL startup code was the entry point, so
  `_DLL_InitTerm` (which starts the FreeType engine) never ran. Fixed with `option start=_DLL_InitTerm` in `Makefile.wat`.
- After that all fonts load (198 listed), but any process that uses a TrueType font through GPI traps in `List_Find`
  (read of the driver's own data, which sits at an address of another process): the driver data is not shared between the
  PM processes. The original DLL has `DATA SINGLE SHARED`. Done so far: data object marked shared (`segment type data
  shared`) and the module flag "single data" set after linking by `tools\lxdata.c` (the Watcom linker has no switch for it
  in LX files). A trace (`IFIEXTRA=-DFTIFI_TRACE`, `\FTIFIP.LOG`) shows that each PM process still gets its own copy of the
  data (different `&liveFiles` per process), so PM still crashes; with the trace build the Workplace Shell does not even start.
- Stand-alone tests that do not need PM pass with the current DLL: `test\iftest.c` (one process: load font, query faces,
  open context, render a glyph through the dispatch table) and `test\iftest2.c` (two processes: the child uses the font
  handle of the parent, so the shared data works there). Build them with `test\build_iftest.cmd` / `build_iftest2.cmd`.
- Recovery used when PM hangs: boot, press Alt+F1 right after reset, F2 (command line), rename the DLL registered under
  `PM_Font_Drivers` / `TRUETYPE`, reboot, restore the key with `FTSETK.CMD` (helper kept outside the repo).

Open: why PM loads the DLL data per process; next idea is to make the object layout of the DLL identical to the original
(four objects, constant data in a separate shared read-only object) or to find the loader rule. The Open Watcom build is
**not usable as a replacement for the shipped driver yet**.
