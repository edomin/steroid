#ifndef ST_MODULES_ATLAS_SIMPLE_SIMPLE_H
#define ST_MODULES_ATLAS_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/atlas.h"

st_atlas_funcs_t st_atlas_simple_funcs = {
    .atlas_init            = st_atlas_init,
    .atlas_quit            = st_atlas_quit,
    .atlas_create          = st_atlas_create,
    .atlas_destroy         = st_atlas_destroy,
    .atlas_set_clip        = st_atlas_set_clip,
    .atlas_get_texture     = st_atlas_get_texture,
    .atlas_get_clips_count = st_atlas_get_clips_count,
    .atlas_get_clip_x      = st_atlas_get_clip_x,
    .atlas_get_clip_y      = st_atlas_get_clip_y,
    .atlas_get_clip_width  = st_atlas_get_clip_width,
    .atlas_get_clip_height = st_atlas_get_clip_height,
};

st_modfuncentry_t st_module_atlas_simple_funcs[] = {
    {"init",            st_atlas_init},
    {"quit",            st_atlas_quit},
    {"create",          st_atlas_create},
    {"destroy",         st_atlas_destroy},
    {"set_clip",        st_atlas_set_clip},
    {"get_texture",     st_atlas_get_texture},
    {"get_clips_count", st_atlas_get_clips_count},
    {"get_clip_x",      st_atlas_get_clip_x},
    {"get_clip_y",      st_atlas_get_clip_y},
    {"get_clip_width",  st_atlas_get_clip_width},
    {"get_clip_height", st_atlas_get_clip_height},
    {NULL, NULL},
};

#endif /* ST_MODULES_ATLAS_SIMPLE_SIMPLE_H */
