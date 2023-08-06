#ifndef ST_MODULES_BITMAP_SIMPLE_TYPES_H
#define ST_MODULES_BITMAP_SIMPLE_TYPES_H

#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_bitmap_simple_logger_t;

typedef struct {
    st_bitmap_simple_logger_t logger;
} st_bitmap_simple_t;

typedef struct {
    st_bitmap_simple_t *module;
    unsigned            width;
    unsigned            height;
    int                 pixel_format;
    char                data[];
} st_bitmap_t;

#define ST_BITMAP_T_DEFINED

#endif /* ST_MODULES_BITMAP_SIMPLE_TYPES_H */
