#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/bitmap.h"

static st_bitmapctx_funcs_t st_bitmap_simple_funcs = {
    .quit    = st_bitmap_quit,
    .load    = st_bitmap_load,
    .memload = st_bitmap_memload,
    .import  = st_bitmap_import,
};

static st_modfuncentry_t st_module_bitmap_simple_funcs[] = {
    {"init"            , st_bitmap_init},
    {"quit"            , st_bitmap_quit},
    {"load"            , st_bitmap_load},
    {"memload"         , st_bitmap_memload},
    {"save"            , st_bitmap_save},
    {"memsave"         , st_bitmap_memsave},
    {"import"          , st_bitmap_import},
    {"destroy"         , st_bitmap_destroy},
    {"get_data"        , st_bitmap_get_data},
    {"get_width"       , st_bitmap_get_width},
    {"get_height"      , st_bitmap_get_height},
    {"get_pixel_format", st_bitmap_get_pixel_format},
    {NULL, NULL},
};
