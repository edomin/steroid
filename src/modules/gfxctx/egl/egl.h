#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/gfxctx.h"

static st_gfxctxctx_funcs_t st_gfxctx_egl_funcs = {
    .quit          = st_gfxctx_quit,
    .create        = st_gfxctx_create,
    .create_shared = st_gfxctx_create_shared,
};

static st_modfuncentry_t st_module_gfxctx_egl_funcs[] = {
    {"init",             st_gfxctx_init},
    {"quit",             st_gfxctx_quit},
    {"create",           st_gfxctx_create},
    {"create_shared",    st_gfxctx_create_shared},
    {"make_current",     st_gfxctx_make_current},
    {"swap_buffers",     st_gfxctx_swap_buffers},
    {"get_window",       st_gfxctx_get_window},
    {"get_api",          st_gfxctx_get_api},
    {"get_shared_index", st_gfxctx_get_shared_index},
    {"destroy",          st_gfxctx_destroy},
    {"debug_enabled",    st_gfxctx_debug_enabled},
    {"set_userdata",     st_gfxctx_set_userdata},
    {"get_userdata",     st_gfxctx_get_userdata},
    {NULL, NULL},
};
