#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/bmcodec.h"

static st_bmcodec_funcs_t st_bmcodec_stb_image_funcs = {
    .bmcodec_init         = st_bmcodec_init,
    .bmcodec_quit         = st_bmcodec_quit,
    .bmcodec_get_priority = st_bmcodec_get_priority,
    .bmcodec_load         = st_bmcodec_load,
    .bmcodec_memload      = st_bmcodec_memload,
    .bmcodec_save         = st_bmcodec_save,
    .bmcodec_memsave      = st_bmcodec_memsave,
};

static st_modfuncentry_t st_module_bmcodec_stb_image_funcs[] = {
    {"init",         st_bmcodec_init},
    {"quit",         st_bmcodec_quit},
    {"get_priority", st_bmcodec_get_priority},
    {"load",         st_bmcodec_load},
    {"memload",      st_bmcodec_memload},
    {"save",         st_bmcodec_save},
    {"memsave",      st_bmcodec_memsave},
    {NULL, NULL},
};
