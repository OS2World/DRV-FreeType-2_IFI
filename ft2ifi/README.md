# FT2IFI: FreeType 2 based driver, Open Watcom build (work in progress)

Branch `ft2ifi`. This directory is the upstream `ft2ifi/` tree (https://github.com/altsan/os2-freetype-ifi, tag `v2.0.0-beta.1`:
FreeType/2 IFI 2.0.0 public beta 1, based on FreeType 2.5.0.1 with the OS/2 subsystem additions in
`lib\freetype-2.5.0.1\builds\os2`), plus an Open Watcom 2 build. The version 1 driver (FreeType 1.3.1, release 1.3.6r2) is in the
top-level directory and is not touched.

## Build

```text
cd ft2ifi
compile_wat.cmd
```

Builds `bin\FT2IFI.dll` (about 326 KB, module name FT2IFI): FreeType 2.5.0.1 (base, autofit, truetype, cff, psnames, pshinter,
raster, sfnt, psaux, and the OS/2 `ifisystem.c`) plus `ifi\ft2ifi.c` and `ft2kern.c`. Same recipe as the version 1 DLL: entry point
`_DLL_InitTerm`, shared data object, "single data" module flag set by `..\tools\lxdata.c`, module name equal to the file name. The IFI headers are
the clean-room ones of the parent directory (`IFI_CAPS_PANOSE` = 1 added from `dev-docs\IFI32_Errata.txt`). Needs the OS/2 Toolkit 4.5.

## Changes to the upstream source

- `ifi\ft2ifi.c`: compiler guard and dispatch table casts for Open Watcom (as in version 1).
- `ifi\ft2ifi.c`, `FdQueryFullFaces`: a **bug of the upstream beta** is fixed. For DBCS fonts it measured `pszFaceName`, which is only set when the
  entry is actually returned, so a size-only query (output count 0) read an uninitialized pointer and trapped. The release build crashed on
  `DroidSansCombined.ttf`, the debug build happened to survive. The face name length is taken from the value computed earlier instead.

## Results on the ArcaOS Dev VM (2026-11-18)

- Loads and runs as the PM TrueType driver (registered as `FT2IFI.DLL` under `PM_Font_Drivers` / `TRUETYPE`): PM starts, the font engine lists the
  same **112 outline fonts** as the shipped driver, no trap.
- PM rendering (`test\fontrender`, `render_ft2ifi.txt` against `render_ship.txt`): 106 of the 448 samples are byte-identical; the others differ in
  hinting/rounding only (pixel counts within -32% / +22%, mean +1%, text width equal for 109 of 112 fonts at 40 pt).
- Different from the shipped driver (all three are properties of this beta, not yet investigated):
  - the three `@` vertical-writing faces (`@Droid Sans Combined`, `@Times New Roman WT J`, `@Monotype Sans Duospace WT J`) render the same text box
    (1174 pels at 40 pt) and the same pixel count: the vertical variant is not rendered like the FreeType 1 driver does;
  - no kerning pairs are reported (`KERN_TABLE_SUPPORT` is commented out in `ft2ifi.c`; the FreeType 1 driver reports them for 54 faces);
  - driver-level comparison (`test\dump_ft2ifi.txt` against `test\dump_reference.txt`): bitmaps, outlines, widths and metrics differ in most faces, as expected
    from a different rasterizer and hinting; all 185 files and 194 faces load and render, with no trap.
- Not done: OpenType/CFF fonts (none on the VM), soak and parallel runs, install via `UPDATE.CMD`, real hardware.

Next steps (not started): review the differences above, then update the bundled FreeType to 2.15.0.
