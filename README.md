# FreeType/2 for OS/2 and ArcaOS (OS2World)

FreeType/2 is the Presentation Manager TrueType font driver (an installable font interface driver, a user-mode DLL) that replaces IBM's `TRUETYPE.DLL`.
This repository has two drivers:

| | Newest: **2.15.0a1** (alpha) | Older: **1.3.6r2** |
|---|---|---|
| Directory | [`ft2ifi\`](ft2ifi) | [`freetype1\`](freetype1) |
| Based on | FreeType **2.15.0** | FreeType 1.3.1 |
| Fonts | TrueType and OpenType/CFF | TrueType |
| DLL | `FT2IFI.DLL` | `FREETYPE.DLL` (the driver ArcaOS ships as 1.3.6) |
| Download | [release 2.15.0a1](https://github.com/OS2World/DRV-FreeType-2_IFI/releases/tag/2.15.0a1) | source and git tag `1.3.6r2` |
| Notes | [`ft2ifi\README.md`](ft2ifi/README.md) | [`freetype1\README.md`](freetype1/README.md) |

Both are built with Open Watcom 2 (`compile_wat.cmd` in each directory) and installed side by side: the new driver does not replace `FREETYPE.DLL`, and its `UNINST.CMD` goes back.
Read the install and recovery steps in the README of the driver before installing; back up first.

Shared by both drivers: [`test\`](test) (programs that compare a driver with the shipped one without PM, render every outline font through PM, soak the driver; reference reports of the
shipped 1.3.6 driver), [`tools\lxdata.c`](tools) (sets the "single data" module flag the PM font engine needs, which the Watcom linker cannot do) and [`LICENSE`](LICENSE).

## License

FreeType Project License, see [LICENSE](LICENSE) (FreeType 2 itself is also available under the GPL v2, see `ft2ifi\lib\freetype-2.15.0\`). The IFI headers (`src\ifi\32*.h`, `ft2ifi\ifi\32*.h`)
are clean-room replacements of the IBM/Microsoft ones. Authors: Alexander Taylor, Michal Necasek, Seo Hyun-Tae, KO Myung-Hun and others (FreeType/2); the FreeType Project; OS2World (Open Watcom builds,
bug fixes, tests).

## History

- 2.15.0a1 (2026-11-18): driver based on FreeType 2.15.0, OpenType/CFF, fixes of four bugs of the upstream 2.0 beta.
- 1.3.6r2 (2026-11-18): upstream 1.3.6 built with Open Watcom 2.
- 1.3.4: original state of this repository (git tag `upstream-original`).

Upstream: https://github.com/altsan/os2-freetype-ifi. FreeType: https://gitlab.freedesktop.org/freetype/freetype.
