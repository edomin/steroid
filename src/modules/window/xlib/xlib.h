#ifndef ST_MODULES_WINDOW_XLIB_XLIB_H
#define ST_MODULES_WINDOW_XLIB_XLIB_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/window.h"

st_window_funcs_t st_window_xlib_funcs = {
    .window_init        = st_window_init,
    .window_quit        = st_window_quit,
    .window_create      = st_window_create,
    .window_destroy     = st_window_destroy,
    .window_process     = st_window_process,
    .window_xed         = st_window_xed,
    .window_get_monitor = st_window_get_monitor,
    .window_get_handle  = st_window_get_handle,
    .window_get_width   = st_window_get_width,
    .window_get_height  = st_window_get_height,
};

st_modfuncentry_t st_module_window_xlib_funcs[] = {
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

#endif /* ST_MODULES_WINDOW_XLIB_XLIB_H */
