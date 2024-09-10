#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/window.h"

static st_windowctx_funcs_t st_window_xlib_funcs = {
    .window_init    = st_window_init,
    .window_quit    = st_window_quit,
    .window_create  = st_window_create,
    .window_process = st_window_process,
};

static st_modfuncentry_t st_module_window_xlib_funcs[] = {
    {"init",        st_window_init},
    {"quit",        st_window_quit},
    {"create",      st_window_create},
    {"destroy",     st_window_destroy},
    {"process",     st_window_process},
    {"xed",         st_window_xed},
    {"get_monitor", st_window_get_monitor},
    {"get_handle",  st_window_get_handle},
    {"get_width",   st_window_get_width},
    {"get_height",  st_window_get_height},
    {NULL, NULL},
};
