#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/render.h"

st_render_funcs_t st_render_opengl_funcs = {
    .render_init                 = st_render_init,
    .render_quit                 = st_render_quit,
    .render_put_sprite           = st_render_put_sprite,
    .render_put_sprite_rdangled  = st_render_put_sprite_rdangled,
    .render_put_sprite_dgangled  = st_render_put_sprite_dgangled,
    .render_put_sprite_rhsheared = st_render_put_sprite_rhsheared,
    .render_put_sprite_dhsheared = st_render_put_sprite_dhsheared,
    .render_put_sprite_rvsheared = st_render_put_sprite_rvsheared,
    .render_put_sprite_dvsheared = st_render_put_sprite_dvsheared,
    .render_process              = st_render_process,
};

st_modfuncentry_t st_module_render_opengl_funcs[] = {
    {"init",                 st_render_init},
    {"quit",                 st_render_quit},
    {"put_sprite",           st_render_put_sprite},
    {"put_sprite_rdangled",  st_render_put_sprite_rdangled},
    {"put_sprite_dgangled",  st_render_put_sprite_dgangled},
    {"put_sprite_rhsheared", st_render_put_sprite_rhsheared},
    {"put_sprite_dhsheared", st_render_put_sprite_dhsheared},
    {"put_sprite_rvsheared", st_render_put_sprite_rvsheared},
    {"put_sprite_dvsheared", st_render_put_sprite_dvsheared},
    {"process",              st_render_process},
    {NULL, NULL},
};
