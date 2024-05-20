#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/png.h"

static st_png_funcs_t st_png_libpng_funcs = {
    .png_init    = st_png_init,
    .png_quit    = st_png_quit,
    .png_load    = st_png_load,
    .png_memload = st_png_memload,
    .png_save    = st_png_save,
    .png_memsave = st_png_memsave,
};

static st_modfuncentry_t st_module_png_libpng_funcs[] = {
    {"init",    st_png_init},
    {"quit",    st_png_quit},
    {"load",    st_png_load},
    {"memload", st_png_memload},
    {"save",    st_png_save},
    {"memsave", st_png_memsave},
    {NULL, NULL},
};
