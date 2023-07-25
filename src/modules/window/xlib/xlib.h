#ifndef ST_MODULES_WINDOW_XLIB_XLIB_H
#define ST_MODULES_WINDOW_XLIB_XLIB_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/window.h"

st_window_funcs_t st_window_xlib_funcs = {
    .window_init    = st_window_init,
    .window_quit    = st_window_quit,
    .window_create  = st_window_create,
    .window_destroy = st_window_destroy,
};

#define FUNCS_COUNT 5
st_modfuncstbl_t st_module_window_xlib_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init"   , st_window_init},
        {"quit"   , st_window_quit},
        {"create" , st_window_create},
        {"destroy", st_window_destroy},
        {"process", st_window_process},
    }
};

#endif /* ST_MODULES_WINDOW_XLIB_XLIB_H */
