#ifndef ST_MODULES_SPRITE_SIMPLE_SIMPLE_H
#define ST_MODULES_SPRITE_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/sprite.h"

st_sprite_funcs_t st_sprite_simple_funcs = {
    .sprite_init        = st_sprite_init,
    .sprite_quit        = st_sprite_quit,
    .sprite_create      = st_sprite_create,
    .sprite_destroy     = st_sprite_destroy,
    .sprite_get_texture = st_sprite_get_texture,
    .sprite_export_uv   = st_sprite_export_uv,
};

st_modfuncentry_t st_module_sprite_simple_funcs[] = {
    {"init",        st_sprite_init},
    {"quit",        st_sprite_quit},
    {"create",      st_sprite_create},
    {"destroy",     st_sprite_destroy},
    {"get_texture", st_sprite_get_texture},
    {"export_uv",   st_sprite_export_uv},
    {NULL, NULL},
};

#endif /* ST_MODULES_SPRITE_SIMPLE_SIMPLE_H */
