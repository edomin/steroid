#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/texture.h"

static st_texturectx_funcs_t st_texture_opengl_funcs = {
    .texture_init    = st_texture_init,
    .texture_quit    = st_texture_quit,
    .texture_load    = st_texture_load,
    .texture_memload = st_texture_memload,
};

static st_modfuncentry_t st_module_texture_opengl_funcs[] = {
    {"init",       st_texture_init},
    {"quit",       st_texture_quit},
    {"load",       st_texture_load},
    {"memload",    st_texture_memload},
    {"destroy",    st_texture_destroy},
    {"bind",       st_texture_bind},
    {"get_width",  st_texture_get_width},
    {"get_height", st_texture_get_height},
    {NULL, NULL},
};
