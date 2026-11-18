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
