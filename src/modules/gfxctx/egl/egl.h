#ifndef ST_MODULES_GFXCTX_EGL_SIMPLE_H
#define ST_MODULES_GFXCTX_EGL_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/gfxctx.h"

st_gfxctx_funcs_t st_gfxctx_egl_funcs = {
    .gfxctx_init          = st_gfxctx_init,
    .gfxctx_quit          = st_gfxctx_quit,
    .gfxctx_create        = st_gfxctx_create,
    .gfxctx_create_shared = st_gfxctx_create_shared,
    .gfxctx_make_current  = st_gfxctx_make_current,
    .gfxctx_swap_buffers  = st_gfxctx_swap_buffers,
    .gfxctx_get_api       = st_gfxctx_get_api,
    .gfxctx_destroy       = st_gfxctx_destroy,
};

#define FUNCS_COUNT 8
st_modfuncstbl_t st_module_gfxctx_egl_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init"         , st_gfxctx_init},
        {"quit"         , st_gfxctx_quit},
        {"create"       , st_gfxctx_create},
        {"create_shared", st_gfxctx_create_shared},
        {"make_current" , st_gfxctx_make_current},
        {"swap_buffers" , st_gfxctx_swap_buffers},
        {"get_api"      , st_gfxctx_get_api},
        {"destroy"      , st_gfxctx_destroy},
    }
};

#endif /* ST_MODULES_GFXCTX_EGL_SIMPLE_H */
