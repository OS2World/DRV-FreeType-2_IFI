# FreeType in this tree

`freetype-2.15.0` is FreeType 2.15.0 (tag VER-2-15-0, https://gitlab.freedesktop.org/freetype/freetype) reduced to what the driver uses:
`include\`, `src\{base,autofit,truetype,cff,psaux,psnames,pshinter,raster,sfnt}`, the licence files (`FTL.TXT`, `GPLv2.TXT`, `LICENSE.TXT`) and `README`.
FreeType is available under the FreeType Project License or the GPL v2.

Files added for the OS/2 IFI driver (not part of FreeType):

- `builds\os2\ifisystem.c`: replaces `ftsystem.c`: memory from `SSAllocMem` (shared by all PM processes), files from the `Dos*` calls, no C run-time.
  Ported from the 2.5.0.1 version of the upstream FT2IFI project; the changes are the 2.15 header names, the size of the memory object
  (`sizeof(*memory)`, it was the size of a pointer) and a safer `ft_realloc`.
- `builds\os2\ifioption.h`: `include\freetype\config\ftoption.h` with these switched off: `FT_CONFIG_OPTION_ENVIRONMENT_PROPERTIES`, `USE_LZW`, `USE_ZLIB`, `MAC_FONTS`,
  `GUESSING_EMBEDDED_RFORK`, `INCREMENTAL`, `SVG`, `TT_CONFIG_OPTION_GX_VAR_SUPPORT`, `TT_CONFIG_OPTION_VARC`, `TT_CONFIG_OPTION_BDF`.
- `builds\os2\ifimodule.h`: the modules compiled in (autofit, truetype, cff, psaux, psnames, pshinter, sfnt, raster1; 1-bit rendering only).
- `ifi\ft2build.h`: selects the two headers above, then includes `freetype\config\ftheader.h`; `ifi` must come before `include` in the include path.

## Updating to a newer FreeType

1. Replace `include\` and the `src\` modules above with the new release; keep `builds\os2`.
2. Regenerate `ifioption.h` from the new `ftoption.h` with the switches listed above (check for new options that need the C library or a file system).
3. Compare `ifimodule.h` with the new `ftmodule.h` and `ifisystem.c` with the new `builds\unix\ftsystem.c` (stream and memory interface).
4. `compile_wat.cmd`, then run the comparison tests in `..\..\test` against an isolated copy of the DLL (see `..\README.md`).
