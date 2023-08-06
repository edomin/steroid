#ifndef ST_MODULES_BITMAP_SIMPLE_SIMPLE_H
#define ST_MODULES_BITMAP_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/bitmap.h"

st_bitmap_funcs_t st_bitmap_simple_funcs = {
    .bitmap_init             = st_bitmap_init,
    .bitmap_quit             = st_bitmap_quit,
    .bitmap_import           = st_bitmap_import,
    .bitmap_destroy          = st_bitmap_destroy,
    .bitmap_get_data         = st_bitmap_get_data,
    .bitmap_get_width        = st_bitmap_get_width,
    .bitmap_get_height       = st_bitmap_get_height,
    .bitmap_get_pixel_format = st_bitmap_get_pixel_format,
};

st_modfuncentry_t st_module_bitmap_simple_funcs[] = {
    {"init"            , st_bitmap_init},
    {"quit"            , st_bitmap_quit},
    {"import"          , st_bitmap_import},
    {"destroy"         , st_bitmap_destroy},
    {"get_data"        , st_bitmap_get_data},
    {"get_width"       , st_bitmap_get_width},
    {"get_height"      , st_bitmap_get_height},
    {"get_pixel_format", st_bitmap_get_pixel_format},
    {NULL, NULL},
};

#endif /* ST_MODULES_BITMAP_SIMPLE_SIMPLE_H */
