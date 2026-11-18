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
- Not yet done: loading the DLL in the PM font engine (install test).

## Documentation

- `README.md` rewritten to the project template; `LICENSE` is the FreeType Project License (`install\FTL.TXT`).
