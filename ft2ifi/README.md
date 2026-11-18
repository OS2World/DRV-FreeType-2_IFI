# FT2IFI: FreeType 2 based driver, Open Watcom build (alpha)

Branch `ft2ifi`. This directory is the upstream `ft2ifi/` tree (https://github.com/altsan/os2-freetype-ifi, tag `v2.0.0-beta.1`: FreeType/2 IFI 2.0.0 public beta 1),
**updated to FreeType 2.15.0** (release 2.15.0a1), with fixes and an Open Watcom 2 build. The version 1 driver (FreeType 1.3.1, release 1.3.6r2) is in the top-level directory and is not touched.
`lib\freetype-2.15.0` is FreeType 2.15.0 reduced to the needed modules plus the OS/2 additions; see `lib\RESYNC.md`.

## Release 2.15.0a1

Download: https://github.com/OS2World/DRV-FreeType-2_IFI/releases/tag/2.15.0a1 (`FT2IFI_2-15-0a1.zip`, a GitHub pre-release). Source: this branch, tag `2.15.0a1`.

| Field | Details |
|---|---|
| Version | 2.15.0a1: alpha 1 of the FreeType/2 IFI driver built on FreeType 2.15.0 (BLDLEVEL `@#Alexander Taylor:2.15.0a1#@`). The upstream project's own version of this code is 2.0.0 beta 1. |
| Status | Alpha. Works on the ArcaOS 5.1.1 Dev VM and on a real PC with an SMP kernel: loads as the PM TrueType driver, renders TrueType and OpenType/CFF fonts, widths equal to the shipped FreeType/2 1.3.6. Not tested on other hardware, with the configuration program (not included) or with CFF2/variable fonts. |
| License | FreeType Project License (`lib\freetype-2.15.0\FTL.TXT`; FreeType itself is also available under the GPL v2), see `lib\freetype-2.15.0\LICENSE.TXT`. The driver code is by Alexander Taylor and others, see `ifi\ft2ifi.c`. The IFI headers are clean-room replacements. |
| Authors | Alexander Taylor (FT2IFI 2.0 beta), Michal Necasek, Seo Hyun-Tae, KO Myung-Hun (FreeType/2 1.x); the FreeType Project (FreeType 2.15.0); OS2World (Open Watcom build, bug fixes, FreeType 2.15 update, tests) |
| Original source | https://github.com/altsan/os2-freetype-ifi (`ft2ifi/` at tag `v2.0.0-beta.1`), https://gitlab.freedesktop.org/freetype/freetype (tag VER-2-15-0) |
| Build tools | Open Watcom 2.0, OS/2 Toolkit 4.5 (headers and import libraries); `compile_wat.cmd` |

### Install (release zip)

FT2IFI is installed next to the FreeType/2 1.3.6 that ArcaOS ships; it does not replace `FREETYPE.DLL`.

1. Make an OS/2 desktop/INI archive if you can, and know the recovery steps below.
2. From the folder with `FT2IFI.DLL` run `INSTALL.CMD` and answer `yes`: it copies `FT2IFI.DLL` to `\OS2\DLL` and registers it as the TrueType driver (`PM_Font_Drivers` / `TRUETYPE` in `OS2.INI`). Reboot.
3. To add OpenType fonts, install them like TrueType fonts (the driver accepts `.OTF`); on a test system you can add them to `PM_Fonts` by hand.
4. To go back: run `UNINST.CMD` (it restores `FREETYPE.DLL`, or `TRUETYPE.DLL` if there is no FreeType/2) and reboot.
5. If PM does not start after a bad driver: press Alt+F1 right after the boot logo, F2 (command line), rename `C:\OS2\DLL\FT2IFI.DLL`, reboot, then put the key `PM_Font_Drivers` / `TRUETYPE` back to `\OS2\DLL\FREETYPE.DLL`.

`INSTALL.CMD` and `UNINST.CMD` are the upstream scripts; they were not run in the tests below, the same registration was done with a small REXX program.

## Build

```text
cd ft2ifi
compile_wat.cmd
```

Builds `bin\FT2IFI.dll` (about 400 KB, module name FT2IFI): FreeType 2.15.0 (base, autofit, truetype, cff, psaux, psnames, pshinter, raster, sfnt, and the OS/2 `ifisystem.c`) plus `ifi\ft2ifi.c` and
`ft2kern.c`. Same recipe as the version 1 DLL: entry point `_DLL_InitTerm`, shared data object, "single data" module flag set by `..\tools\lxdata.c`, module name equal to the file name
(`wmake DLLNAME=NAME` builds a test copy under another name). The IFI headers are the clean-room ones of the parent directory (`IFI_CAPS_PANOSE` = 1 added from `dev-docs\IFI32_Errata.txt`).
Needs the OS/2 Toolkit 4.5 and Open Watcom 2.

## Changes to the upstream driver source

- `ifi\ft2ifi.c`: compiler guard and dispatch table casts for Open Watcom (as in version 1).
- Bugs of the upstream beta found and fixed (all found by the comparison with the 1.3.6 driver):
  1. `FdQueryFullFaces`, DBCS fonts: a size-only query (output count 0) measured `pszFaceName`, which is only set when the entry is returned: an uninitialized
     pointer, a trap on `DroidSansCombined.ttf` (the debug build happened to survive). The face name length is taken from the value computed earlier.
  2. `FdQueryFaces`, the `@` vertical faces of DBCS fonts: `strncpy(szFamilyname + 1, ..., FACESIZE)` and the same for `szFacename` copied one byte too many, and the zero padding
     of the second one cleared `szGlyphlistName[0]`, so GPI could not use these faces and silently fell back to another font. Now `FACESIZE - 1` (as in version 1).
  3. `FREE()` passed the address of the pointer to `SSFreeMem`, so **nothing was ever freed** (kerning directories, width and kerning index tables, file records): about 1.4 MB
     of shared memory per round of loading 220 fonts. Now it frees the block and clears the pointer.
  4. Kerning: `KERN_TABLE_SUPPORT` was commented out; it is enabled, and the query had its test inverted (`if (!directory || nPairs) return 0`), so it always returned 0 pairs.
  5. `FT_New_Memory` (library side, `ifisystem.c`) allocated the size of a pointer for a structure.
- For FreeType 2.15: `FT_Stream_Open` is declared with a plain `extern` (the `FT_BASE_DEF` form no longer compiles), and the TrueType interpreter is set to **version 35** at start
  (`FT_Property_Set`). FreeType 2.7+ defaults to version 40, which does not hint horizontally; with it the glyphs differed from the 1.3.6 and 2.5 output by up to a pixel in width.

## Results on the ArcaOS Dev VM (2026-11-18)

Test fonts: the 185 TrueType files of the VM plus 35 OpenType/CFF fonts (`C:\usr\share\fonts\urw-base35\*.otf`).

- **PM**: registered as `FT2IFI.DLL` (`PM_Font_Drivers` / `TRUETYPE`), with three OpenType fonts added to `PM_Fonts`: PM starts, the font engine lists the 112 outline fonts of the shipped driver plus the
  3 OpenType fonts, which render (`test\render_ft2ifi.txt`). Against the shipped 1.3.6 driver (`render_ship.txt`): text width equal for all 448 samples (the `@` faces included), 104 samples byte-identical, the others
  differ in hinting only (pixel count -9% / +18%, mean +1%). Against the same driver built with FreeType 2.5.0.1: 211 of 460 identical, no width difference, pixel counts -18% / +3%.
- **Driver level** (isolated copy of the DLL under another name, `test\dump_ft2ifi.txt` against `test\dump_reference.txt` of 1.3.6):
  all 220 files (185 TrueType, 35 OpenType) load and render; 182 of 185 TrueType files return the same full-face names; kerning is returned for 52 of the 54 faces that have it in 1.3.6 and 41 have identical pairs
  (FreeType 2 maps more glyphs to PM indexes than the 1.3.6 lookup, which gives 0 beyond a point). Against the FreeType 2.5.0.1 build: the 8 sample glyphs per face at two sizes (6,176 bitmaps and outlines),
  the metrics, widths, kerning and full-face names are identical; over 94 glyphs per face about a third of the bitmaps differ in a few pixels (new rasterizer and CFF engine; most of the OpenType fonts).
- **OpenType/CFF**: all 35 fonts load and render, no kerning is reported (they carry GPOS, not a `kern` table).
- **Memory**: 100 rounds of load/open/render/unload of 220 fonts lose 0.66 MB of free memory (the 1.3.6 driver loses about 70 KB per round with 185 fonts).
- **Concurrency**: three independent processes calling the driver at once crash it (the engine has no locking: in PM the GRE serializes the calls). Such a test is not valid for this driver.

Not done: replacing the installed driver the way a user would (`repmod`), real hardware, CFF2 and variable fonts (GX variations are switched off), vertical text with real CJK glyphs, a release.

## Real hardware, SMP (ArcaMiix, 2026-11-18)

ArcaOS 5.1 with the SMP kernel (IBM 14.204_SMP, ACPI.PSD), the shipped FreeType/2 1.3.6 as the reference. The DLLs were copied next to the shipped one under other names
(`FT2IFI.DLL`; `FT2U.DLL` and `FTS.DLL` for the isolated driver-level tests); the shipped `FREETYPE.DLL` was never touched. After the tests the PC was put back on the shipped driver.

- **PM, new driver registered** (`TRUETYPE` = `FT2IFI.DLL`, reboot): PM starts normally, `FT2IFI.DLL` is loaded (locked), the font engine lists 112 outline fonts, no popup, no trap
  (`test\hw_render_new.txt`). Against the shipped driver on the same machine (`hw_render_ship.txt`): text width equal for all 448 samples, 104 samples byte-identical, pixel counts -9% / +18%, mean +1%:
  the same numbers as on the Dev VM.
- **OpenType fonts in PM**: with three URW OpenType fonts added to `PM_Fonts` (`C059-Roman`, `C059-Bold`, `NimbusSans-Regular`), the engine lists 115 outline fonts and they render (`hw_render_new2.txt`).
- **Driver level** (129 TrueType/OpenType files on the PC, both drivers through the dispatch table, no PM): the shipped driver cannot load 35 of them (the OpenType/CFF ones), the new one loads all 129;
  kerning is returned for the same 46 faces; 94 of 129 files return the same full-face names.
- **Soak**: 100 rounds over the 129 files, both drivers finish; the new driver loses 0.56 MB of free memory (the shipped one 1.27 MB) and takes 134 s against 40 s for the shipped driver (it loads 35 more files, including large CFF fonts,
  and FreeType 2 is slower than FreeType 1).

## Known issues

- Alpha: see the status above. The configuration program of upstream (`config\`) is not built, so options (fake bold, alias names ...) can only be changed in `OS2.INI` by hand.
- Blank glyphs of OpenType fonts (the notdef glyph and unmapped codes) come back as a 1x1 empty bitmap in later test runs and as a 0x0 bitmap in earlier ones, with the same driver code; the cause is not found
  (it depends on the state of the test machine, not on the build). The result is a blank glyph either way.
- The engine has no locking; it relies on the PM GRE to serialize calls. Do not call the driver from several processes at once.
- FreeType's GX variations, SVG, Mac font formats, zlib/LZW/BDF/PCF are switched off (see `lib\RESYNC.md`).

## Cautions when testing

- A test program that links the registered DLL shares PM's instance of the driver (single shared data). Test an isolated copy: `wmake DLLNAME=FT2T`, copy `bin\FT2T.dll` to
  `\OS2\DLL\FT2T.DLL`. A leak or crash in a test then cannot disturb PM. (A kernel trap and an out-of-memory failure of the compiler happened on the VM while an earlier build was
  leaking shared memory and tests were running on PM's own instance.)
- A DLL's internal module name must equal its file name (the build does this from `DLLNAME`).

Not done: install via `UPDATE.CMD`-style replacement, real hardware, fonts with CFF2/variable data, vertical text with real CJK glyphs, the rest of FreeType 2.5 modules.

Next step (not started): update the bundled FreeType from 2.5.0.1 to 2.15.0.

## Links

- Release 2.15.0a1: https://github.com/OS2World/DRV-FreeType-2_IFI/releases/tag/2.15.0a1
- This repository, branch `ft2ifi`: https://github.com/OS2World/DRV-FreeType-2_IFI/tree/ft2ifi (the version 1 driver, release 1.3.6r2, is on `master`)
- Upstream FT2IFI: https://github.com/altsan/os2-freetype-ifi
- FreeType: https://gitlab.freedesktop.org/freetype/freetype
