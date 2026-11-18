# FreeType/2 (FreeType.dll)

FreeType/2 is the OS/2 Presentation Manager installable font interface (IFI) driver that renders
TrueType fonts through the FreeType library; it replaces IBM's `TRUETYPE.DLL`. It is a user-mode DLL
(ring 3), not a `.SYS` driver.

| Field | Details |
|---|---|
| Version | 1.3.6 (BLDLEVEL `@#Necasek/Seo/KO/Taylor et al:1.3.6#@`) |
| Status | Builds with Open Watcom 2 (all outputs) and works in stand-alone tests, but **does not work yet as the PM font driver**: PM processes trap because the driver data is not shared between them (see doc/CHANGES.md). Use the shipped `FREETYPE.DLL`. |
| Tested on | ArcaOS 5.1.1 Dev VM (VirtualBox), 2026-11-17. No real hardware. |
| License | FreeType Project License, see [LICENSE](LICENSE). The four IFI headers `src\ifi\32*.h` are clean-room replacements (see `doc/CHANGES.md`). |
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

Original scripts in `install\` (`INSTALL.CMD`, `UPDATE.CMD`, `UNINST.CMD`, `QUERY.CMD`; see `install\readme.txt`
and `install\FAQ`). They copy the DLL to `\OS2\DLL` and register it in `OS2.INI`. Back up `OS2.INI` and the existing
`FREETYPE.DLL` first; ArcaOS already ships FreeType/2 1.3.6.

## Parameters and boot messages

None: this is a user-mode DLL, it prints nothing at boot. Settings are made with `ftconfig.exe` (stored in `OS2.INI`).

## Sample / test

`test\iftest.c`, `iftest2.c` test the driver without PM (build with `test\build_iftest.cmd`); `test\fontlist.c` and `fontrender.c` list and render fonts through PM (used to compare with the shipped DLL). `src\ifi\test.c` and `testconv.c` are the original test programs.

## Known issues

- Does not work inside PM yet: the DLL's data is not shared between processes (original: `DATA SINGLE SHARED`).
- Only TrueType (and what FreeType 1.3.1 supports); OpenType/CFF needs the FreeType 2 based driver
  (`ft2ifi` in the upstream repository), planned as a second phase.

## Links

- https://github.com/OS2World/DRV-FreeType-2_IFI
- https://github.com/altsan/os2-freetype-ifi

## Changelog

Newest first; details in [doc/CHANGES.md](doc/CHANGES.md).

- 1.3.6 (re-based on upstream) built with Open Watcom 2 (`compile_wat.cmd`).
- 1.3.4: original state.
