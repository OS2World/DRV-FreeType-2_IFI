/*
 * ifimodule.h - modules compiled into the OS/2 IFI font driver (FT2IFI).
 * TrueType and OpenType/CFF fonts, 1-bit rendering only (the IFI interface wants monochrome bitmaps and outlines).
 * Replaces include/freetype/config/ftmodule.h, see ifi/ft2build.h.
 */
FT_USE_MODULE( FT_Module_Class, autofit_module_class )
FT_USE_MODULE( FT_Driver_ClassRec, tt_driver_class )
FT_USE_MODULE( FT_Driver_ClassRec, cff_driver_class )
FT_USE_MODULE( FT_Module_Class, psaux_module_class )
FT_USE_MODULE( FT_Module_Class, psnames_module_class )
FT_USE_MODULE( FT_Module_Class, pshinter_module_class )
FT_USE_MODULE( FT_Module_Class, sfnt_module_class )
FT_USE_MODULE( FT_Renderer_Class, ft_raster1_renderer_class )
/* EOF */
