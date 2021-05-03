#ifndef ST_STEROIDS_OPTS_H
#define ST_STEROIDS_OPTS_H

#include "steroids/module.h"
#include "steroids/types/modules/opts.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif



#undef ST_VISIBILITY

#endif
