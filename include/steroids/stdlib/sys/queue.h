#ifndef ST_STDLIB_SYS_QUEUE_H
#define ST_STDLIB_SYS_QUEUE_H

#include "steroids/config.h"

#ifndef ST_HAVE_QUEUE_H
    #ifndef _SYS_CDEFS_H_
        #undef __P
        #undef __CONCAT
        #undef __always_inline
        #undef __nonnull
    #endif
    #include <bsd/sys/queue.h>
#endif

#endif
