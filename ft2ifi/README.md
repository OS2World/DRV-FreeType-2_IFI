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
- Bugs of the upstream beta found and fixed (all found by the comparison with the 1.3.6 driver):
  1. `FdQueryFullFaces`, DBCS fonts: a size-only query (output count 0) measured `pszFaceName`, which is only set when the entry is returned: an uninitialized
     pointer, a trap on `DroidSansCombined.ttf` (the debug build happened to survive). The face name length is taken from the value computed earlier.
  2. `FdQueryFaces`, the `@` vertical faces of DBCS fonts: `strncpy(szFamilyname + 1, ..., FACESIZE)` and the same for `szFacename` copied one byte too many, and the zero padding
     of the second one cleared `szGlyphlistName[0]`, so GPI could not use these faces and silently fell back to another font. Now `FACESIZE - 1` (as in version 1).
  3. `FREE()` passed the address of the pointer to `SSFreeMem`, so **nothing was ever freed** (kerning directories, width and kerning index tables, file records): about 1.4 MB
     of shared memory per round of loading 220 fonts. Now it frees the block and clears the pointer.
  4. Kerning: `KERN_TABLE_SUPPORT` was commented out; it is enabled, and the query had its test inverted (`if (!directory || nPairs) return 0`), so it always returned 0 pairs.

## Results on the ArcaOS Dev VM (2026-11-18)

Test fonts: the 185 TrueType files of the VM, plus 35 OpenType/CFF fonts (`C:\usr\share\fonts\urw-base35\*.otf`, URW base 35).

- **PM**: registered as `FT2IFI.DLL` (`PM_Font_Drivers` / `TRUETYPE`) with three OpenType fonts added to `PM_Fonts`: PM starts, the font engine lists the 112 outline fonts of
  the shipped driver plus the 3 OpenType fonts, which render (`render_ft2ifi.txt`). Against the shipped driver (`render_ship.txt`): text width equal for all 448 samples
  (the `@` faces included), 107 samples byte-identical, the others differ in hinting only (pixel count -9% / +18%, mean +0.9%).
- **OpenType/CFF**: all 35 fonts load and render at driver level (`test\art_otf.txt` has the letter A of each), no kerning reported (they carry GPOS, not a `kern` table).
- **Driver level** (isolated copy of the DLL under another name, so the test does not share PM's instance; `test\dump_ft2ifi.txt` against `test\dump_reference.txt`):
  all 220 files load and render; 182 of 185 TrueType files return the same full-face names; kerning is returned for 52 of the 54 faces that have it in 1.3.6 and 41 of those
  have identical pairs (the others differ because FreeType 2 maps more glyphs to PM indexes than the 1.3.6 reverse lookup, which returns 0 beyond a point); bitmaps, outlines and
  widths differ in most faces as expected from a different rasterizer.
- **Memory**: 100 rounds of load/open/render/unload of the 220 fonts lose 0.4 MB of free memory (the 1.3.6 driver loses about 70 KB per round of 185 fonts), after the `FREE` fix.
- **Concurrency**: three independent processes calling the driver at once crash it (the engine has no locking: in PM the GRE serializes the calls). A test of this kind is not valid for this driver.

## Cautions when testing

- A test program that links the registered DLL shares PM's instance of the driver (single shared data). Test an isolated copy: `wmake DLLNAME=FT2T`, copy `bin\FT2T.dll` to
  `\OS2\DLL\FT2T.DLL`. A leak or crash in a test then cannot disturb PM. (A kernel trap and an out-of-memory failure of the compiler happened on the VM while an earlier build was
  leaking shared memory and tests were running on PM's own instance.)
- A DLL's internal module name must equal its file name (the build does this from `DLLNAME`).

Not done: install via `UPDATE.CMD`-style replacement, real hardware, fonts with CFF2/variable data, vertical text with real CJK glyphs, the rest of FreeType 2.5 modules.

Next step (not started): update the bundled FreeType from 2.5.0.1 to 2.15.0.
