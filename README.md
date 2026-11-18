# FreeType/2 (FreeType.dll)

FreeType/2 is the OS/2 Presentation Manager installable font interface (IFI) driver that renders
TrueType fonts through the FreeType library; it replaces IBM's `TRUETYPE.DLL`. It is a user-mode DLL
(ring 3), not a `.SYS` driver.

| Field | Details |
|---|---|
| Version | 1.3.6r2 (BLDLEVEL `@#Necasek/Seo/KO/Taylor et al:1.3.6r2#@`) |
| Status | Working: builds with Open Watcom 2 and, on the ArcaOS Dev VM, behaves exactly like the shipped `FREETYPE.DLL` (112 outline fonts in PM; metrics, widths, kerning, bitmaps and outlines of 185 font files identical; soak and parallel runs clean; installed with `UPDATE.CMD` and rolled back). **Not yet tested on real hardware.** |
| Tested on | ArcaOS 5.1.1 Dev VM (VirtualBox), 2026-11-18, side-by-side copy, then installed with `UPDATE.CMD` and rolled back. No real hardware. |
| License | FreeType Project License, see [LICENSE](LICENSE). The four IFI headers `src\ifi\32*.h` are clean-room replacements (see `doc/CHANGES.md`); the old IBM/Microsoft versions remain in the git history (they were already in the original repository, tag `upstream-original`). `doc\IFI32.PDF` and the other files in `doc\` come from the original package. |
| Authors | Michal Necasek, Seo, KO Myung-Hun, Alex Taylor and others (see `src\ifi\ftifi.c`); FreeType 1.3.1 by David Turner, Robert Wilhelm, Werner Lemberg |
| Original source | https://github.com/altsan/os2-freetype-ifi (`legacy/`, tag v1.3.6); git tag `upstream-original` = the earlier 1.3.4 snapshot |
| Build tools | Open Watcom 2.0 + OS/2 Toolkit 4.5 (headers, `os2386.lib`, `libconv.lib`, `rc`, `ipfc`); build with `compile_wat.cmd` |

## Build

```text
compile_wat.cmd
```

Prints `OK: bin\FreeType.dll` (and the other outputs) with sizes, or `FAILED:`; writes `compile.log`.
`compile_wat.cmd clean` removes `bin\` and the log.

## Install

ArcaOS already has FreeType/2 1.3.6 installed, so this release is an update. In the release zip `FREETYPE.DLL` and the scripts are in one folder:

1. Back up the current driver: `copy C:\OS2\DLL\FREETYPE.DLL` to a safe folder, and make an OS/2 desktop/INI archive if you can.
2. Run `UPDATE.CMD` from that folder. It replaces the loaded `FREETYPE.DLL` with `repmod` (the ArcaOS copy is in `C:\sys\apps\ftifi`; put that folder in the PATH if `repmod` is not found). Reboot.
3. `QUERY.CMD` shows the registered TrueType driver; `ftconfig.exe` is the settings program.
4. To go back: run `repmod <backup>\FREETYPE.DLL C:\os2\dll\FREETYPE.DLL` and reboot.

A first installation on a system without FreeType/2 uses `INSTALL.CMD` instead (copies the DLL, creates the settings object, registers the driver in `OS2.INI`); it was not tested with this build. If PM does not start after a bad driver: Alt+F1 at boot, F2 (command line), rename the DLL, reboot, and restore the `PM_Font_Drivers` / `TRUETYPE` key.
See also `readme.txt` and `FAQ` (the original package documentation).

## Parameters and boot messages

None: this is a user-mode DLL, it prints nothing at boot. Settings are made with `ftconfig.exe` (stored in `OS2.INI`).

## Sample / test

- `test\ifidump.c` (build: `test\build_ifidump.cmd <dllname> <exename>`): driver-level comparison without PM. Calls the IFI dispatch table of a
  DLL for every TrueType file in a list and writes a report (metrics, full faces, ABC widths, kerning, bitmaps, outlines);
  `test\dump_reference.txt` is the report of the shipped 1.3.6 DLL and of this build (identical). `soak <n>` repeats everything n times and
  prints the free memory before and after. `iftest.c`, `iftest2.c`, `iftest3.c`: small single-/two-process and DosLoadModule tests.
- `test\fontrender.c`: renders every outline font through PM into a bitmap and prints metrics and checksums (`render_ship.txt` = shipped DLL,
  `render_ow.txt` = this build, identical). `fontlist.c` lists the fonts.
- `src\ifi\test.c` and `testconv.c` are the original test programs.
- To test a DLL next to the installed one: its internal module name must equal its file name (`wmake MODNAME=NAME`), register it under
  `PM_Font_Drivers` / `TRUETYPE` in `OS2.INI`, reboot.

## Known issues

- Not tested on real hardware, and `INSTALL.CMD`, `UNINST.CMD`, saving settings in `ftconfig.exe` and the help files were not tested.
- Only TrueType, as in the shipped driver. OpenType/CFF would need the FreeType 2 based driver (`ft2ifi` in the upstream repository), not ported.
- The code page range 2 charset checks are disabled (as in the VisualAge build, by accident of an upstream overflow); the intended
  values are noted in `src\ifi\ftifi.h`.
- The driver leaks memory in the shared engine at the same rate as the shipped one (about 70 KB per round of loading all 185 test fonts).
- `ftconfig.exe` must not be bound with `rc -x` (it then fails to start on ArcaOS).

## Links

- https://github.com/OS2World/DRV-FreeType-2_IFI
- https://github.com/altsan/os2-freetype-ifi

## Changelog

Newest first; details in [doc/CHANGES.md](doc/CHANGES.md).

- 1.3.6r2 (2026-11-18): upstream 1.3.6 built with Open Watcom 2 (`compile_wat.cmd`); clean-room IFI headers; output identical to the shipped driver; BLDLEVEL 1.3.6r2.
- 1.3.4: original state.
