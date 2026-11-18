/* fontlist.c - list the public fonts the PM font engine offers, mark outline (TrueType) fonts and
   measure a text string with each of the first few outline fonts.
   Build: see test\build_fontlist.cmd. Output: fontlist.out in the current directory. */
#define INCL_GPI
#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    HAB hab = WinInitialize(0);
    HMQ hmq = WinCreateMsgQueue(hab, 0);
    HPS hps = WinGetScreenPS(HWND_DESKTOP);
    LONG n = 0, i, outline = 0, measured = 0;
    PFONTMETRICS pfm;
    FILE *f = fopen("fontlist.out", "w");

    if (!f) return 1;
    n = GpiQueryFonts(hps, QF_PUBLIC, NULL, &n, (LONG) sizeof(FONTMETRICS), NULL);
    pfm = (PFONTMETRICS) calloc(n, sizeof(FONTMETRICS));
    GpiQueryFonts(hps, QF_PUBLIC, NULL, &n, (LONG) sizeof(FONTMETRICS), pfm);
    fprintf(f, "fonts: %ld\n", n);
    for (i = 0; i < n; i++) {
        int isout = (pfm[i].fsDefn & FM_DEFN_OUTLINE) != 0;
        if (isout) outline++;
        fprintf(f, "%3ld %c %-32s %-32s type=%04x sel=%04x\n", i, isout ? 'O' : ' ',
                pfm[i].szFamilyname, pfm[i].szFacename, pfm[i].fsType, pfm[i].fsSelection);
    }
    fprintf(f, "outline fonts: %ld\n", outline);
    for (i = 0; i < n && measured < 12; i++) {
        FATTRS fat; POINTL ptl[TXTBOX_COUNT]; SIZEF sz; LONG ok;
        if (!(pfm[i].fsDefn & FM_DEFN_OUTLINE)) continue;
        memset(&fat, 0, sizeof(fat));
        fat.usRecordLength = sizeof(fat);
        fat.fsFontUse = FATTR_FONTUSE_OUTLINE;
        fat.usCodePage = 850;
        strcpy(fat.szFacename, pfm[i].szFacename);
        ok = GpiCreateLogFont(hps, NULL, 1L, &fat);
        GpiSetCharSet(hps, 1L);
        sz.cx = MAKEFIXED(40, 0); sz.cy = MAKEFIXED(40, 0);
        GpiSetCharBox(hps, &sz);
        GpiQueryTextBox(hps, 11L, "Hello World", TXTBOX_COUNT, ptl);
        fprintf(f, "measure %-32s create=%ld width=%ld\n", pfm[i].szFacename, ok,
                ptl[TXTBOX_TOPRIGHT].x - ptl[TXTBOX_BOTTOMLEFT].x);
        GpiSetCharSet(hps, LCID_DEFAULT);
        GpiDeleteSetId(hps, 1L);
        measured++;
    }
    fclose(f);
    WinReleasePS(hps);
    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
    return 0;
}
