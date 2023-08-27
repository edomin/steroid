#ifndef ST_MODULES_RENDER_OPENGL_RENDER_H
#define ST_MODULES_RENDER_OPENGL_RENDER_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/render.h"

st_render_funcs_t st_render_opengl_funcs = {
    .render_init       = st_render_init,
    .render_quit       = st_render_quit,
    .render_put_sprite = st_render_put_sprite,
    .render_process    = st_render_process,
};

st_modfuncentry_t st_module_render_opengl_funcs[] = {
    {"init",       st_render_init},
    {"quit",       st_render_quit},
    {"put_sprite", st_render_put_sprite},
    {"process",    st_render_process},
    {NULL, NULL},
};

#endif /* ST_MODULES_RENDER_OPENGL_RENDER_H */
