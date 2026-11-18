/*
 * 32fdstrc.h - data structures exchanged between the Graphics Engine (GRE)
 *              and an OS/2 32-bit font driver (IFI, version 20/21).
 *
 * Clean-room replacement written from the public IFI documentation (IFI32.PDF)
 * and from the way FreeType/2 (ftifi.c) uses these types.
 * Written for the OS2World FreeType/2 project; this file may be used under the
 * same terms as the project (FreeType Project License).
 *
 * Binary layout is part of the ABI shared with the Graphics Engine; do not
 * reorder fields.  Constants not given in the documentation were taken from
 * the observed behaviour of a working driver build.
 */
#ifndef FT2IFI_FDSTRC_H
#define FT2IFI_FDSTRC_H

/* os2.h (LONG, ULONG, FIXED, SIZEL, PBYTE ...) must be included first */

/* ---- handles and glyph indices ------------------------------------------ */

typedef LHANDLE HFF;            /* font file handle (0 invalid, -1 error)   */
typedef LHANDLE HFC;            /* font context handle (0 invalid, -1 error)*/
typedef ULONG   GLYPH;          /* glyph index, 4 bytes in the structures   */
typedef GLYPH  *PGLYPH;

/* ---- FdQueryFaceAttr: iQuery values (compared with ==) ------------------ */
#define FD_QUERY_ABC_WIDTHS       2
#define FD_QUERY_KERNINGPAIRS     3

/* ---- FdQueryCharAttr: iQuery bit flags ---------------------------------- */
#define FD_QUERY_CHARIMAGE        0x00000001UL
#define FD_QUERY_OUTLINE          0x00000002UL
#define FD_QUERY_BITMAPMETRICS    0x00000004UL

/* ---- outline records ---------------------------------------------------- */
#define FD_POLYGON_TYPE           24    /* POLYGONHEADER.iType              */
#define FD_PRIM_LINE              1     /* PRIMLINE.iType                   */
#define FD_PRIM_SPLINE            3     /* PRIMSPLINE.iType (cubic)         */

#pragma pack(push, 4)

/* POINTFX (two FIXED values, x and y) is declared by the Toolkit (pmgpi.h). */

/* 2x2 notional-to-device transform:  x' = x*eM11 + y*eM21,
 *                                    y' = x*eM12 + y*eM22                   */
typedef struct _MAT2 {
    FIXED eM11;
    FIXED eM12;
    FIXED eM21;
    FIXED eM22;
} MAT2, *PMAT2;

/* font context description passed to FdSetFontContext */
typedef struct _CONTEXTINFO {
    ULONG   cb;                 /* length of this structure                 */
    ULONG   fl;                 /* flags, reserved                          */
    SIZEL   sizlPPM;            /* device resolution, pels per metre        */
    POINTFX pfxSpot;            /* spot size in pels                        */
    MAT2    matXform;           /* notional to device transform             */
} CONTEXTINFO, *PCONTEXTINFO;

/* request block of FdQueryCharAttr (24 bytes for a version 21 driver) */
typedef struct _CHARATTR {
    ULONG cb;                   /* length of this structure                 */
    ULONG iQuery;               /* FD_QUERY_* bit flags                     */
    GLYPH gi;                   /* glyph index                              */
    PBYTE pBuffer;              /* bitmap / outline buffer                  */
    ULONG cbLen;                /* size of the buffer in bytes              */
    ULONG fl;                   /* alignment / caching flags (version 21)   */
} CHARATTR, *PCHARATTR;

/* geometry of the bitmap returned by FdQueryCharAttr */
typedef struct _BITMAPMETRICS {
    SIZEL   sizlExtent;         /* real width and height in pels            */
    ULONG   cyAscent;           /* reserved, zero                           */
    POINTFX pfxOrigin;          /* top-left of bitmap relative to origin    */
} BITMAPMETRICS, *PBITMAPMETRICS;

/* FD_QUERY_ABC_WIDTHS output record (notional units) */
typedef struct _ABC_TRIPLETS {
    LONG  lA;
    ULONG ulB;
    LONG  lC;
} ABC_TRIPLETS, *PABC_TRIPLETS;

/* FD_QUERY_KERNINGPAIRS output record (notional units) */
typedef struct _FD_KERNINGPAIRS {
    GLYPH giFirst;
    GLYPH giSecond;
    LONG  eKerningAmount;
} FD_KERNINGPAIRS, *PFD_KERNINGPAIRS;

/* outline output: a POLYGONHEADER followed by PRIMLINE / PRIMSPLINE records */
typedef struct _POLYGONHEADER {
    ULONG cb;                   /* size of the whole polygon record         */
    ULONG iType;                /* FD_POLYGON_TYPE                          */
} POLYGONHEADER, *PPOLYGONHEADER;

typedef struct _PRIMLINE {
    ULONG   iType;              /* FD_PRIM_LINE                             */
    POINTFX pte;                /* starting vertex                          */
} PRIMLINE, *PPRIMLINE;

typedef struct _PRIMSPLINE {
    ULONG   iType;              /* FD_PRIM_SPLINE                           */
    POINTFX pte[3];             /* starting vertex and two control points   */
} PRIMSPLINE, *PPRIMSPLINE;

#pragma pack(pop)

/* Note: the FFDESCS2 record of FdQueryFullFaces is declared by ftifi.c itself. */

#endif /* FT2IFI_FDSTRC_H */
