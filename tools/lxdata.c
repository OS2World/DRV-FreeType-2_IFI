/* lxdata.c - set the "single automatic data segment" flag (DATA SINGLE) in the LX module flags
   of a DLL. The Open Watcom linker has no switch for this in LX files, while the PM font engine
   needs it for a font driver (the IBM and the original FreeType/2 DLLs have it).
   usage: lxdata <dll> */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE *f;
    unsigned char h[4];
    unsigned long lx, flags;
    if (argc < 2) { printf("usage: lxdata <dll>\n"); return 2; }
    f = fopen(argv[1], "r+b");
    if (!f) { printf("lxdata: cannot open %s\n", argv[1]); return 1; }
    fseek(f, 0x3c, SEEK_SET);
    fread(&lx, 4, 1, f);
    fseek(f, lx, SEEK_SET);
    fread(h, 2, 1, f);
    if (h[0] != 'L' || h[1] != 'X') { printf("lxdata: %s is not an LX file\n", argv[1]); fclose(f); return 1; }
    fseek(f, lx + 0x10, SEEK_SET);
    fread(&flags, 4, 1, f);
    flags |= 1UL;
    fseek(f, lx + 0x10, SEEK_SET);
    fwrite(&flags, 4, 1, f);
    fclose(f);
    printf("lxdata: %s module flags now %08lX\n", argv[1], flags);
    return 0;
}
