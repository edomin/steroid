#ifndef ST_STRING_H
#define ST_STRING_H

#include <string.h>
#include "steroids/config.h"

#ifndef ST_HAVE_STRLCPY
size_t strlcpy(char * __restrict dst, const char * __restrict src,
 size_t dsize);
#endif

#endif
