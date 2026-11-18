/* fontrender.c - render a text with every outline font into a memory bitmap and print, per font and
   size, the text box, font metrics, the number of set pixels and a checksum of the bitmap.
   Run it once per driver and compare the two outputs. usage: fontrender <outfile> [dump]
   With "dump" it also writes an ASCII picture of the bitmap of each font at the large size. */
#define INCL_GPI
#define INCL_DEV
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BW 640
#define BH 80

int main(int argc, char **argv)
{
    HAB hab = WinInitialize(0);
    HMQ hmq = WinCreateMsgQueue(hab, 0);
    HPS hscr = WinGetScreenPS(HWND_DESKTOP);
    LONG n = 0, i;
    PFONTMETRICS pfm;
    FILE *f;
    int dump = argc > 2;
    static const int sizes[] = { 8, 12, 24, 40 };
    static const char *text = "Hamburgefonstiv Quick Fox 0123 AVATAR";
    HDC hdc; HPS hps; SIZEL szl = { BW, BH };
    BITMAPINFOHEADER2 bih; HBITMAP hbm;
    unsigned char *bits = malloc(BW * 4 * BH);
    PBITMAPINFO2 pbmi = calloc(1, sizeof(BITMAPINFO2) + 16 * sizeof(RGB2));
    DEVOPENSTRUC dop = { 0 };

    if (argc < 2) return 2;
    f = fopen(argv[1], "w");
    if (!f) return 1;
    n = GpiQueryFonts(hscr, QF_PUBLIC, NULL, &n, (LONG) sizeof(FONTMETRICS), NULL);
    pfm = calloc(n, sizeof(FONTMETRICS));
    GpiQueryFonts(hscr, QF_PUBLIC, NULL, &n, (LONG) sizeof(FONTMETRICS), pfm);

    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, NULL, NULLHANDLE);
    hps = GpiCreatePS(hab, hdc, &szl, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
    memset(&bih, 0, sizeof(bih));
    bih.cbFix = sizeof(bih); bih.cx = BW; bih.cy = BH; bih.cPlanes = 1; bih.cBitCount = 24;
    GpiCreateLogColorTable(hps, 0L, LCOLF_RGB, 0L, 0L, NULL);
    hbm = GpiCreateBitmap(hps, &bih, 0L, NULL, NULL);
    if (hbm == GPI_ERROR) { fprintf(f, "bitmap create failed\n"); fclose(f); return 3; }

    for (i = 0; i < n; i++) {
        int s;
        if (!(pfm[i].fsDefn & FM_DEFN_OUTLINE)) continue;
        fprintf(f, "FONT %s\n", pfm[i].szFacename);
        for (s = 0; s < 4; s++) {
            FATTRS fat; SIZEF sz; POINTL ptl[TXTBOX_COUNT], at; FONTMETRICS fm; RECTL rcl = { 0, 0, BW, BH };
            unsigned long sum = 5381; long set = 0; LONG y; int x, pt = sizes[s];
            BITMAPINFOHEADER2 *pb = (BITMAPINFOHEADER2 *) pbmi;
            GpiSetBitmap(hps, hbm);
            memset(&fat, 0, sizeof(fat));
            fat.usRecordLength = sizeof(fat);
            fat.fsFontUse = FATTR_FONTUSE_OUTLINE | FATTR_FONTUSE_TRANSFORMABLE;
            fat.usCodePage = 850;
            strcpy(fat.szFacename, pfm[i].szFacename);
            if (GpiCreateLogFont(hps, NULL, 1L, &fat) == GPI_ERROR) { fprintf(f, "  %d pt: createfont failed\n", pt); continue; }
            GpiSetCharSet(hps, 1L);
            sz.cx = sz.cy = MAKEFIXED(pt * 4 / 3, 0);
            GpiSetCharBox(hps, &sz);
            GpiQueryFontMetrics(hps, sizeof(fm), &fm);
            GpiQueryTextBox(hps, strlen(text), (PCH) text, TXTBOX_COUNT, ptl);
            GpiSetColor(hps, 0x00FFFFFFL);
            { POINTL p0 = { 0, 0 }, p1 = { BW - 1, BH - 1 }; GpiMove(hps, &p0); GpiSetPattern(hps, PATSYM_SOLID); GpiBox(hps, DRO_FILL, &p1, 0, 0); }
            GpiSetColor(hps, 0x00000000L);
            at.x = 4; at.y = BH / 3;
            GpiCharStringAt(hps, &at, strlen(text), (PCH) text);
            GpiSetCharSet(hps, LCID_DEFAULT);
            GpiDeleteSetId(hps, 1L);
            memset(pb, 0, sizeof(BITMAPINFOHEADER2));
            pb->cbFix = sizeof(BITMAPINFOHEADER2); pb->cx = BW; pb->cy = BH; pb->cPlanes = 1; pb->cBitCount = 24;
            memset(bits, 0, BW * 4 * BH);
            GpiQueryBitmapBits(hps, 0L, BH, bits, pbmi);
            for (y = 0; y < BH; y++) for (x = 0; x < BW; x++) {
                unsigned char *px = bits + y * ((BW * 3 + 3) / 4 * 4) + x * 3;
                if (px[0] != 255 || px[1] != 255 || px[2] != 255) set++;
                sum = sum * 33 + px[0] + 7 * px[1] + 13 * px[2];
            }
            fprintf(f, "  %2d pt: box=%ld,%ld..%ld,%ld ave=%ld maxasc=%ld maxdesc=%ld xh=%ld pixels=%ld sum=%08lx\n", pt,
                    ptl[TXTBOX_BOTTOMLEFT].x, ptl[TXTBOX_BOTTOMLEFT].y, ptl[TXTBOX_TOPRIGHT].x, ptl[TXTBOX_TOPRIGHT].y,
                    fm.lAveCharWidth, fm.lMaxAscender, fm.lMaxDescender, fm.lXHeight, set, sum);
            fflush(f);
            if (dump && pt == 24) {
                for (y = BH - 1; y >= 0; y -= 2) {
                    for (x = 0; x < 300; x += 2) fputc(bits[y * ((BW * 3 + 3) / 4 * 4) + x * 3] < 128 ? '#' : ' ', f);
                    fputc(10, f);
                }
            }
            GpiSetBitmap(hps, NULLHANDLE);
        }
    }
    GpiDeleteBitmap(hbm); GpiDestroyPS(hps); DevCloseDC(hdc);
    fclose(f);
    WinReleasePS(hscr); WinDestroyMsgQueue(hmq); WinTerminate(hab);
    return 0;
}
