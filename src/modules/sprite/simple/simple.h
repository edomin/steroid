#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/sprite.h"

st_sprite_funcs_t st_sprite_simple_funcs = {
    .sprite_init         = st_sprite_init,
    .sprite_quit         = st_sprite_quit,
    .sprite_create       = st_sprite_create,
    .sprite_from_texture = st_sprite_from_texture,
    .sprite_destroy      = st_sprite_destroy,
    .sprite_get_texture  = st_sprite_get_texture,
    .sprite_get_width    = st_sprite_get_width,
    .sprite_get_height   = st_sprite_get_height,
    .sprite_export_uv    = st_sprite_export_uv,
};

st_modfuncentry_t st_module_sprite_simple_funcs[] = {
    {"init",         st_sprite_init},
    {"quit",         st_sprite_quit},
    {"create",       st_sprite_create},
    {"from_texture", st_sprite_from_texture},
    {"destroy",      st_sprite_destroy},
    {"get_texture",  st_sprite_get_texture},
    {"get_width",    st_sprite_get_width},
    {"get_height",   st_sprite_get_height},
    {"export_uv",    st_sprite_export_uv},
    {NULL, NULL},
};
