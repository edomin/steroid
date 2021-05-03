#ifndef ST_STDLIB_STRING_H
#define ST_STDLIB_STRING_H

#include <string.h>
#include "steroids/config.h"

#ifndef ST_HAVE_STRLCPY
    #ifndef _SYS_CDEFS_H_
        #undef __P
        #undef __CONCAT
        #undef __always_inline
        #undef __nonnull
    #endif
    #include <bsd/string.h>
#endif

#endif
