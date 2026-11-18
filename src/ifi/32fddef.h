/*
 * 32fddef.h - font driver header, dispatch table and entry point types of the
 *             OS/2 32-bit Installable Font Interface (IFI).
 *
 * Clean-room replacement written from the public IFI documentation (IFI32.PDF)
 * and from the way FreeType/2 (ftifi.c) uses these types.
 * Written for the OS2World FreeType/2 project; this file may be used under the
 * same terms as the project (FreeType Project License).
 *
 * A font driver DLL exports a single entry, FONT_DRIVER_DISPATCH_TABLE, which
 * is the address of an FDHEADER.  Entry points use the _System (_syscall)
 * calling convention.
 */
#ifndef FT2IFI_FDDEF_H
#define FT2IFI_FDDEF_H

#include "32fdstrc.h"
#include "32ifimet.h"

#define IFI_VERSION20   20      /* first 32-bit IFI version                 */
#define IFI_VERSION21   21      /* adds bitmap fonts, new glyphlist names   */
#define IFI_CAPS_PANOSE 1       /* ufDeviceCaps: IFIMETRICS carries the PANOSE table (IFI32_Errata.txt) */

/* FdConvertFontFile return code: success (error is -1).  Defined here because
 * ftifi.c returns it by this name. */
#ifndef OK
#define OK 0
#endif

/* calling convention of all font driver entry points */
#ifndef FT2IFI_CALL
#define FT2IFI_CALL _System
#endif

typedef struct _FDDISPATCH *PFDDISPATCH;

/* FdLoadFontFile: returns an HFF, or -1 */
typedef HFF  (FT2IFI_CALL *PFDLFF)(PSZ pszFileName);
/* FdQueryFaces: returns number of faces, or -1 */
typedef LONG (FT2IFI_CALL *PFDQF)(HFF hff, PIFIMETRICS pifiMetrics,
                                  ULONG cMetricLen, ULONG cFontCount,
                                  ULONG cStart);
/* FdUnloadFontFile */
typedef LONG (FT2IFI_CALL *PFDUFF)(HFF hff);
/* FdOpenFontContext: returns an HFC, or -1 */
typedef HFC  (FT2IFI_CALL *PFDOFC)(HFF hff, ULONG ulFont);
/* FdSetFontContext */
typedef LONG (FT2IFI_CALL *PFDSFC)(HFC hfc, PCONTEXTINFO pci);
/* FdCloseFontContext */
typedef LONG (FT2IFI_CALL *PFDCFC)(HFC hfc);
/* FdQueryFaceAttr: ABC widths or kerning pairs */
typedef LONG (FT2IFI_CALL *PFDQFA)(HFC hfc, ULONG iQuery, PBYTE pBuffer,
                                   ULONG cb, PGLYPH pagi, GLYPH giStart);
/* FdQueryCharAttr: glyph bitmap, outline and bitmap metrics */
typedef LONG (FT2IFI_CALL *PFDQCA)(HFC hfc, PCHARATTR pCharAttr,
                                   PBITMAPMETRICS pbmm);
/* FdClaimFontFile: slot unused by FreeType/2 (NULL); prototype unverified */
typedef LONG (FT2IFI_CALL *PFDCLF)(PSZ pszFileName);
/* FdConvertFontFile: install (or remove, both strings NULL) a font file */
typedef LONG (FT2IFI_CALL *PFDCFF)(PSZ pszSrc, PSZ pszDestDir, PSZ pszNewName);
/* FdQueryFullFaces: family/face name records */
typedef LONG (FT2IFI_CALL *PFDQFF)(HFF hff, PVOID pBuffer, PULONG pcBufLen,
                                   PULONG pcFontCount, ULONG cStart);

#pragma pack(push, 4)

/* table of entry points, in this order (11 slots, 44 bytes) */
typedef struct _FDDISPATCH {
    PFDLFF FdLoadFontFile;
    PFDQF  FdQueryFaces;
    PFDUFF FdUnloadFontFile;
    PFDOFC FdOpenFontContext;
    PFDSFC FdSetFontContext;
    PFDCFC FdCloseFontContext;
    PFDQFA FdQueryFaceAttr;
    PFDQCA FdQueryCharAttr;
    PFDCLF FdClaimFontFile;
    PFDCFF FdConvertFontFile;
    PFDQFF FdQueryFullFaces;
} FDDISPATCH;

/* the object exported as FONT_DRIVER_DISPATCH_TABLE (72 bytes) */
typedef struct _FDHEADER {
    ULONG       cbLength;           /* sizeof(FDHEADER)                     */
    UCHAR       strId[16];          /* "OS/2 FONT DRIVER", no terminator    */
    UCHAR       szTechnology[40];   /* name of the font technology          */
    ULONG       ulVersion;          /* IFI_VERSION20 / IFI_VERSION21        */
    ULONG       ufDeviceCaps;       /* flags; 0 or 1 (IFIMETRICS extension) */
    PFDDISPATCH pfddisp;            /* entry point table                    */
} FDHEADER, *PFDHEADER;

#pragma pack(pop)

typedef char ft2ifi_chk_fdhdr_size [(sizeof(FDHEADER)   == 72) ? 1 : -1];
typedef char ft2ifi_chk_fddisp_size[(sizeof(FDDISPATCH) == 44) ? 1 : -1];

#endif /* FT2IFI_FDDEF_H */
