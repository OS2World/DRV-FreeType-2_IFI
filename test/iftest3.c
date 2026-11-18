/* iftest3.c - does DosLoadModule of the driver DLL in two processes give the same data address?
   usage: iftest3 [dllname]  (default C:\OS2\DLL\FTT.DLL); the parent starts a child copy of itself. */
#define INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

static void show(const char *who, const char *name)
{
    char err[64]; HMODULE h; PFN pfn = NULL; APIRET rc;
    rc = DosLoadModule(err, sizeof(err), (PSZ) name, &h);
    if (rc) { printf("%s: DosLoadModule rc=%lu (%s)\n", who, rc, err); return; }
    rc = DosQueryProcAddr(h, 1, NULL, &pfn);
    printf("%s: hmod=%lX ordinal1 rc=%lu addr=%lX\n", who, (unsigned long) h, rc, (unsigned long) pfn);
}

int main(int argc, char **argv)
{
    const char *dll = (argc > 2) ? argv[2] : "FTT";
    char cmd[300];
    setbuf(stdout, NULL);
    if (argc > 1 && !strcmp(argv[1], "child")) { show("child ", dll); return 0; }
    show("parent", dll);
    sprintf(cmd, "%s child %s", argv[0], dll);
    system(cmd);
    return 0;
}
