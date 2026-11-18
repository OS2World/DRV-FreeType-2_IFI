/* ft2build.h for the OS/2 IFI driver: selects the IFI option and module headers (builds/os2), then
 * continues as the stock ft2build.h does. Put the directory of this file in front of include/ in the include path. */
#ifndef FT2BUILD_H_
#define FT2BUILD_H_

#define FT_CONFIG_OPTIONS_H  <ifioption.h>
#define FT_CONFIG_MODULES_H  <ifimodule.h>

#include <freetype/config/ftheader.h>

#endif /* FT2BUILD_H_ */
