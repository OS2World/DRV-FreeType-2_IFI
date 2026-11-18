/* iftest2.c - cross-process test of the driver DLL (PM uses it from several processes).
   The parent loads a font file through the dispatch table and starts a child copy of itself;
   the child opens a font context on the parent's font handle and renders glyph 36.
   Build with test\build_iftest.cmd after adding iftest2 (see that script). */
#define INCL_DOS
#define INCL_GRE_DCS
#define INCL_GRE_DEVSUPPORT
#define INCL_DDIMISC
#define INCL_IFI
#include <os2.h>
#include <pmddi.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include "32pmifi.h"

extern FDHEADER fdhdr;
#define FNAME "C:\\OS2\\MDOS\\WINOS2\\SYSTEM\\ARIAL.TTF"

static void child(HFF hff)
{
    HFC hfc; CONTEXTINFO ci; CHARATTR ca; BITMAPMETRICS bmm; static char buf[32768]; LONG rc;
    static IFIMETRICS im;
    printf("child: using hff %lX\n", (unsigned long) hff);
    rc = fdhdr.pfddisp->FdQueryFaces(hff, &im, sizeof(im), 1, 0);
    printf("child: QueryFaces rc=%ld\n", rc);
    hfc = fdhdr.pfddisp->FdOpenFontContext(hff, 0);
    printf("child: OpenFontContext hfc=%lX\n", (unsigned long) hfc);
    ci.cb = sizeof(ci); ci.fl = 0; ci.sizlPPM.cx = 3622; ci.sizlPPM.cy = 3622;
    ci.pfxSpot.x = 46340; ci.pfxSpot.y = 46340;
    ci.matXform.eM11 = 768; ci.matXform.eM12 = 0; ci.matXform.eM21 = 0; ci.matXform.eM22 = 768;
    rc = fdhdr.pfddisp->FdSetFontContext(hfc, &ci);
    printf("child: SetFontContext rc=%ld\n", rc);
    ca.cb = sizeof(ca); ca.iQuery = FD_QUERY_BITMAPMETRICS | FD_QUERY_CHARIMAGE; ca.gi = 36;
    ca.pBuffer = (PBYTE) buf; ca.cbLen = sizeof(buf);
    rc = fdhdr.pfddisp->FdQueryCharAttr(hfc, &ca, &bmm);
    printf("child: QueryCharAttr bytes=%ld\n", rc);
    rc = fdhdr.pfddisp->FdCloseFontContext(hfc);
    printf("child: CloseFontContext rc=%ld\n", rc);
}

int main(int argc, char **argv)
{
    HFF hff; char cmd[300];
    setbuf(stdout, NULL);
    if (argc > 2 && !strcmp(argv[1], "child")) { child((HFF) strtoul(argv[2], NULL, 16)); return 0; }
    hff = fdhdr.pfddisp->FdLoadFontFile(FNAME);
    printf("parent: HFF=%lX\n", (unsigned long) hff);
    sprintf(cmd, "%s child %lX", argv[0], (unsigned long) hff);
    system(cmd);
    printf("parent: unload rc=%ld\n", (long) fdhdr.pfddisp->FdUnloadFontFile(hff));
    return 0;
}
