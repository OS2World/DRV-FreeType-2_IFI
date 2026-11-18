/* ifidump.c - compare two builds of the font driver DLL without PM.
   Runs through the IFI dispatch table of the DLL it is linked to (see build_ifidump.cmd) and writes a
   text report: for every TrueType file in the list file, the face metrics, QueryFullFaces, ABC widths,
   kerning pairs, bitmaps and outlines of a sample of glyphs at two sizes. Diff the reports of two DLLs.
   usage: ifidump <listfile> <outfile>            report
          ifidump <listfile> <outfile> soak <n>   n rounds of load/open/render/unload, memory before/after */
#define INCL_DOS
#define INCL_GRE_DCS
#define INCL_GRE_DEVSUPPORT
#define INCL_DDIMISC
#define INCL_IFI
#include <os2.h>
#include <pmddi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "32pmifi.h"

extern FDHEADER fdhdr;
#define D (fdhdr.pfddisp)

static unsigned long hash(const unsigned char *p, long n)
{
    unsigned long h = 5381;
    while (n-- > 0) h = h * 33 + *p++;
    return h;
}

static unsigned char buf[65536];
static unsigned char abc[256 * 12];
static unsigned char kern[4000 * 8];

static int hexmode = 0;

static int process(FILE *f, const char *file, int quick)
{
    HFF hff; LONG nf, j; static IFIMETRICS im;
    hff = D->FdLoadFontFile((PSZ) file);
    if (!quick) fprintf(f, "FILE %s\n", file);
    if (hff == (HFF) -1) { if (!quick) fprintf(f, "  load failed\n"); return 0; }
    nf = D->FdQueryFaces(hff, NULL, 0, (ULONG) -1, 0);
    if (!quick) fprintf(f, "  faces=%ld\n", nf);
    for (j = 0; j < nf && j < 4; j++) {
        LONG rc = D->FdQueryFaces(hff, &im, sizeof(im), 1, j);
        HFC hfc; int s;
        if (!quick)
            fprintf(f, "  face %ld rc=%ld fam='%.32s' face='%.32s' gl='%.16s' len=%lu sum=%08lx kern=%d first=%lu last=%lu def=%lu\n",
                    j, rc, im.szFamilyname, im.szFacename, im.szGlyphlistName, (unsigned long) im.ulMetricsLength,
                    hash((unsigned char *) &im, sizeof(im)), (int) im.cKerningPairs,
                    (unsigned long) im.giFirstChar, (unsigned long) im.giLastChar, (unsigned long) im.giDefaultChar);
        if (hexmode) { int k; fprintf(f, "HEX %s %ld ", file, j); for (k = 0; k < (int) sizeof(im); k++) fprintf(f, "%02x", ((unsigned char *) &im)[k]); fputc(10, f); continue; }
        hfc = D->FdOpenFontContext(hff, j);
        if (hfc == (HFC) -1) { if (!quick) fprintf(f, "  open context failed\n"); continue; }
        for (s = 0; s < 2; s++) {
            CONTEXTINFO ci; static const int gis[] = { 0, 3, 36, 37, 68, 100, 150, 400 };
            int g; LONG r;
            ci.cb = sizeof(ci); ci.fl = 0; ci.sizlPPM.cx = 3622; ci.sizlPPM.cy = 3622;
            ci.pfxSpot.x = 46340; ci.pfxSpot.y = 46340;
            ci.matXform.eM11 = s ? 1536 : 768; ci.matXform.eM12 = 0; ci.matXform.eM21 = 0; ci.matXform.eM22 = s ? 1536 : 768;
            r = D->FdSetFontContext(hfc, &ci);
            if (!quick) fprintf(f, "  size %d set=%ld\n", s, r);
            if (r == -1) continue;
            memset(abc, 0, sizeof(abc));
            r = D->FdQueryFaceAttr(hfc, FD_QUERY_ABC_WIDTHS, abc, sizeof(abc), NULL, 0);
            if (!quick) fprintf(f, "   abc rc=%ld sum=%08lx\n", r, hash(abc, sizeof(abc)));
            if (s == 0 && im.cKerningPairs > 0) {
                int cnt = im.cKerningPairs > 4000 ? 4000 : im.cKerningPairs;
                memset(kern, 0, sizeof(kern));
                r = D->FdQueryFaceAttr(hfc, FD_QUERY_KERNINGPAIRS, kern, cnt * sizeof(FD_KERNINGPAIRS), NULL, 0);
                if (!quick) fprintf(f, "   kern rc=%ld n=%d sum=%08lx\n", r, cnt, hash(kern, cnt * sizeof(FD_KERNINGPAIRS)));
            }
            for (g = 0; g < 8; g++) {
                CHARATTR ca; BITMAPMETRICS bmm; int mode;
                for (mode = 0; mode < 2; mode++) {
                    memset(buf, 0, sizeof(buf)); memset(&bmm, 0, sizeof(bmm));
                    ca.cb = sizeof(ca); ca.fl = 0; ca.gi = gis[g]; ca.pBuffer = buf; ca.cbLen = sizeof(buf);
                    ca.iQuery = mode ? FD_QUERY_OUTLINE : (FD_QUERY_BITMAPMETRICS | FD_QUERY_CHARIMAGE);
                    r = D->FdQueryCharAttr(hfc, &ca, &bmm);
                    if (!quick) {
                        if (mode)
                            fprintf(f, "   g%d outline rc=%ld sum=%08lx\n", gis[g], r, r > 0 && r <= (LONG) sizeof(buf) ? hash(buf, r) : 0UL);
                        else
                            fprintf(f, "   g%d bitmap rc=%ld ext=%ld,%ld sum=%08lx bm=%08lx\n", gis[g], r,
                                    bmm.sizlExtent.cx, bmm.sizlExtent.cy, r > 0 && r <= (LONG) sizeof(buf) ? hash(buf, r) : 0UL,
                                    hash((unsigned char *) &bmm, sizeof(bmm)));
                    }
                }
            }
        }
        D->FdCloseFontContext(hfc);
    }
    if (!quick) {
        static unsigned char ffb[8192]; ULONG len = sizeof(ffb), cnt = 0; LONG r;
        memset(ffb, 0, sizeof(ffb));
        r = D->FdQueryFullFaces(hff, ffb, &len, &cnt, 0);
        fprintf(f, "  fullfaces rc=%ld len=%lu cnt=%lu sum=%08lx\n", r, len, cnt, hash(ffb, len > sizeof(ffb) ? sizeof(ffb) : len));
    }
    D->FdUnloadFontFile(hff);
    return 1;
}

int main(int argc, char **argv)
{
    FILE *lst, *f; char line[300]; static char names[400][260]; int n = 0, i, soak = 0;
    setbuf(stdout, NULL);
    if (argc < 3) return 2;
    lst = fopen(argv[1], "r");
    if (!lst) return 1;
    while (n < 400 && fgets(line, sizeof(line), lst)) {
        char *e = line + strlen(line);
        while (e > line && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ')) *--e = 0;
        if (line[0]) strcpy(names[n++], line);
    }
    fclose(lst);
    f = fopen(argv[2], "w");
    if (!f) return 1;
    if (getenv("IFIDUMP_UNBUF")) setvbuf(f, NULL, _IONBF, 0);
    if (argc > 3 && !strcmp(argv[3], "hex")) hexmode = 1;
    if (argc > 4 && !strcmp(argv[3], "soak")) soak = atoi(argv[4]);
    if (!soak) {
        for (i = 0; i < n; i++) process(f, names[i], 0);
        fprintf(f, "END %d files\n", n);
    } else {
        ULONG m0[2], m1[2]; int r; ULONG t0, t1;
        DosQuerySysInfo(QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, &m0[0], 4);
        DosQuerySysInfo(QSV_MAXSHMEM, QSV_MAXSHMEM, &m0[1], 4);
        DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &t0, 4);
        for (r = 0; r < soak; r++) {
            for (i = 0; i < n; i++) process(f, names[i], 1);
            if (r % 10 == 0) { fprintf(f, "round %d\n", r); fflush(f); }
        }
        DosQuerySysInfo(QSV_TOTAVAILMEM, QSV_TOTAVAILMEM, &m1[0], 4);
        DosQuerySysInfo(QSV_MAXSHMEM, QSV_MAXSHMEM, &m1[1], 4);
        DosQuerySysInfo(QSV_MS_COUNT, QSV_MS_COUNT, &t1, 4);
        fprintf(f, "SOAK rounds=%d files=%d ms=%lu\n", soak, n, t1 - t0);
        fprintf(f, "availmem before=%lu after=%lu delta=%ld\n", m0[0], m1[0], (long) m1[0] - (long) m0[0]);
        fprintf(f, "maxshmem before=%lu after=%lu delta=%ld\n", m0[1], m1[1], (long) m1[1] - (long) m0[1]);
    }
    fclose(f);
    return 0;
}
