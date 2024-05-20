#pragma once

#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t                 *ctx;
    st_bitmap_import_t           import;
    st_bitmap_get_data_t         get_data;
    st_bitmap_get_width_t        get_width;
    st_bitmap_get_height_t       get_height;
    st_bitmap_get_pixel_format_t get_pixel_format;
} st_bmcodec_stb_image_bitmap_t;

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_error_t   error;
} st_bmcodec_stb_image_logger_t;

typedef struct {
    st_bmcodec_stb_image_bitmap_t bitmap;
    st_bmcodec_stb_image_logger_t logger;
} st_bmcodec_stb_image_t;
