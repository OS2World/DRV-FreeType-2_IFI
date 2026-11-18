/*
 * 32ifimet.h - IFIMETRICS, the per-face metrics record of the OS/2 IFI.
 *
 * Clean-room replacement written from the public IFI documentation (IFI32.PDF)
 * and from the way FreeType/2 (ftifi.c) uses these types.
 * Written for the OS2World FreeType/2 project; this file may be used under the
 * same terms as the project (FreeType Project License).
 *
 * The structure is byte packed (offsets are not naturally aligned) and its
 * layout is an ABI shared with the Graphics Engine.  Differences between the
 * documentation text and the layout used here (derived from a metrics dump of
 * a working driver, src/ifi/arial_tt.hex, and from the built driver):
 *   - szGlyphlistName is 16 bytes, not FACESIZE;
 *   - there is no leading length field; the length is the ulMetricsLength
 *     field near the end (the first 64 bytes are the two names);
 *   - the atom fields of the documentation are not present in the 254-byte
 *     record; ulReserved occupies the final two bytes.
 */
#ifndef FT2IFI_IFIMET_H
#define FT2IFI_IFIMET_H

#include "32fdstrc.h"           /* GLYPH */

/* ---- fsType --------------------------------------------------------------*/
#define IFIMETRICS_FIXED          0x0001  /* fixed pitch                    */
#define IFIMETRICS_LICENSED       0x0002  /* subject of a licence agreement */
#define IFIMETRICS_KERNING        0x0004  /* has kerning data               */
#define IFIMETRICS_MBCS           0x0018  /* DBCS plus single byte; the DBCS bit is 0x0010 */
#define IFIMETRICS_DBCS           0x0010  /* double byte charset            */
#define IFIMETRICS_UNICODE        0x0040  /* font has the Unicode charset   */
#define IFIMETRICS_FACETRUNC      0x1000  /* szFacename truncated           */
#define IFIMETRICS_FAMTRUNC       0x2000  /* szFamilyname truncated         */

/* ---- fsDefn --------------------------------------------------------------*/
#define IFIMETRICS_OUTLINE        0x0001  /* outline font (else bitmap)     */

/* ---- fsSelection ---------------------------------------------------------*/
#define IFIMETRICS_OVERSTRUCK     0x2000  /* strike-out                     */
#define IFIMETRICS_UNDERSCORE     0x4000  /* underscore                     */

/* The charset bits stored in fsCapabilities (IFIMETRICS_CHARSET_*,
 * IFIMETRICS_DBCS_*) are defined in ftifi.h. */

#pragma pack(push, 1)

typedef struct _IFIMETRICS {
    CHAR    szFamilyname[32];       /* family name, e.g. "Times New Roman"  */
    CHAR    szFacename[32];         /* face name, e.g. "Helvetica Bold"     */
    CHAR    szGlyphlistName[16];    /* "PM383", "UNICODE", "PMJPN", ... or "" */
    USHORT  idRegistry;             /* registration number, 0 if none       */
    LONG    lCapEmHeight;           /* notional units from here on          */
    LONG    lXHeight;
    LONG    lMaxAscender;
    LONG    lMaxDescender;
    LONG    lLowerCaseAscent;
    LONG    lLowerCaseDescent;
    LONG    lInternalLeading;
    LONG    lExternalLeading;
    LONG    lAveCharWidth;
    LONG    lMaxCharInc;
    LONG    lEmInc;
    LONG    lMaxBaselineExt;
    FIXED   fxCharSlope;
    FIXED   fxInlineDir;
    FIXED   fxCharRot;
    USHORT  usWeightClass;          /* 1 (ultra-light) .. 9 (ultra-bold)    */
    USHORT  usWidthClass;           /* 1 (ultra-condensed) .. 9 (ultra-expanded) */
    LONG    lEmSquareSizeX;
    LONG    lEmSquareSizeY;
    GLYPH   giFirstChar;
    GLYPH   giLastChar;
    GLYPH   giDefaultChar;
    GLYPH   giBreakChar;
    USHORT  usNominalPointSize;     /* decipoints                           */
    USHORT  usMinimumPointSize;
    USHORT  usMaximumPointSize;
    USHORT  fsType;                 /* IFIMETRICS_FIXED, _LICENSED, ...     */
    USHORT  fsDefn;                 /* IFIMETRICS_OUTLINE                   */
    USHORT  fsSelection;
    USHORT  fsCapabilities;         /* charset bits (see ftifi.h)           */
    LONG    lSubscriptXSize;
    LONG    lSubscriptYSize;
    LONG    lSubscriptXOffset;
    LONG    lSubscriptYOffset;
    LONG    lSuperscriptXSize;
    LONG    lSuperscriptYSize;
    LONG    lSuperscriptXOffset;
    LONG    lSuperscriptYOffset;
    LONG    lUnderscoreSize;
    LONG    lUnderscorePosition;
    LONG    lStrikeoutSize;
    LONG    lStrikeoutPosition;
    SHORT   cKerningPairs;
    ULONG   ulFontClass;            /* IBM font classification              */
    ULONG   ulMetricsLength;        /* sizeof(IFIMETRICS), set by the driver */
    UCHAR   panose[10];             /* PANOSE classification                */
    USHORT  ulReserved;             /* last two bytes (type unverified)     */
} IFIMETRICS, *PIFIMETRICS;

#pragma pack(pop)

/* Layout checks: a wrong size or offset gives a negative array size. */
#define FT2IFI_OFS(t, m) ((unsigned long)&(((t *)0)->m))
typedef char ft2ifi_chk_ifi_size   [(sizeof(IFIMETRICS) == 254)               ? 1 : -1];
typedef char ft2ifi_chk_ifi_reg    [(FT2IFI_OFS(IFIMETRICS, idRegistry)      ==  80) ? 1 : -1];
typedef char ft2ifi_chk_ifi_gifirst[(FT2IFI_OFS(IFIMETRICS, giFirstChar)     == 154) ? 1 : -1];
typedef char ft2ifi_chk_ifi_fstype [(FT2IFI_OFS(IFIMETRICS, fsType)          == 176) ? 1 : -1];
typedef char ft2ifi_chk_ifi_kern   [(FT2IFI_OFS(IFIMETRICS, cKerningPairs)   == 232) ? 1 : -1];
typedef char ft2ifi_chk_ifi_len    [(FT2IFI_OFS(IFIMETRICS, ulMetricsLength) == 238) ? 1 : -1];
typedef char ft2ifi_chk_ifi_panose [(FT2IFI_OFS(IFIMETRICS, panose)          == 242) ? 1 : -1];

#endif /* FT2IFI_IFIMET_H */
