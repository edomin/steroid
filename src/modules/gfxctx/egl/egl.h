#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/gfxctx.h"

st_gfxctx_funcs_t st_gfxctx_egl_funcs = {
    .gfxctx_init             = st_gfxctx_init,
    .gfxctx_quit             = st_gfxctx_quit,
    .gfxctx_create           = st_gfxctx_create,
    .gfxctx_create_shared    = st_gfxctx_create_shared,
    .gfxctx_make_current     = st_gfxctx_make_current,
    .gfxctx_swap_buffers     = st_gfxctx_swap_buffers,
    .gfxctx_get_ctx          = st_gfxctx_get_ctx,
    .gfxctx_get_window       = st_gfxctx_get_window,
    .gfxctx_get_api          = st_gfxctx_get_api,
    .gfxctx_get_shared_index = st_gfxctx_get_shared_index,
    .gfxctx_destroy          = st_gfxctx_destroy,
};

st_modfuncentry_t st_module_gfxctx_egl_funcs[] = {
    {"init",             st_gfxctx_init},
    {"quit",             st_gfxctx_quit},
    {"create",           st_gfxctx_create},
    {"create_shared",    st_gfxctx_create_shared},
    {"make_current",     st_gfxctx_make_current},
    {"swap_buffers",     st_gfxctx_swap_buffers},
    {"get_ctx",          st_gfxctx_get_ctx},
    {"get_window",       st_gfxctx_get_window},
    {"get_api",          st_gfxctx_get_api},
    {"get_shared_index", st_gfxctx_get_shared_index},
    {"destroy",          st_gfxctx_destroy},
    {NULL, NULL},
};
