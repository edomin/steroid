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
    .window_get_monitor = st_window_get_monitor,
    .window_get_handle  = st_window_get_handle,
};

#define FUNCS_COUNT 7
st_modfuncstbl_t st_module_window_xlib_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init"       , st_window_init},
        {"quit"       , st_window_quit},
        {"create"     , st_window_create},
        {"destroy"    , st_window_destroy},
        {"process"    , st_window_process},
        {"get_monitor", st_window_get_monitor},
        {"get_handle" , st_window_get_handle},
    }
};

#endif /* ST_MODULES_WINDOW_XLIB_XLIB_H */
