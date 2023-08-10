#ifndef ST_MODULES_PNG_LIBPNG_TYPES_H
#define ST_MODULES_PNG_LIBPNG_TYPES_H

#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t                 *ctx;
    st_bitmap_import_t           import;
    st_bitmap_get_data_t         get_data;
    st_bitmap_get_width_t        get_width;
    st_bitmap_get_height_t       get_height;
    st_bitmap_get_pixel_format_t get_pixel_format;
} st_png_libpng_bitmap_t;

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_warning_t warning;
    st_logger_error_t   error;
} st_png_libpng_logger_t;

typedef struct {
    st_png_libpng_bitmap_t bitmap;
    st_png_libpng_logger_t logger;
} st_png_libpng_t;

#endif /* ST_MODULES_PNG_LIBPNG_TYPES_H */
