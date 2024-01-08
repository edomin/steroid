#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/mouse.h"

st_mouse_funcs_t st_mouse_simple_funcs = {
    .mouse_init               = st_mouse_init,
    .mouse_quit               = st_mouse_quit,
    .mouse_process            = st_mouse_process,
    .mouse_press              = st_mouse_press,
    .mouse_release            = st_mouse_release,
    .mouse_pressed            = st_mouse_pressed,
    .mouse_get_wheel_relative = st_mouse_get_wheel_relative,
    .mouse_moved              = st_mouse_moved,
    .mouse_entered            = st_mouse_entered,
    .mouse_leaved             = st_mouse_leaved,
    .mouse_get_x              = st_mouse_get_x,
    .mouse_get_y              = st_mouse_get_y,
    .mouse_get_window         = st_mouse_get_window,
};

st_modfuncentry_t st_module_mouse_simple_funcs[] = {
    {"init",               st_mouse_init},
    {"quit",               st_mouse_quit},
    {"process",            st_mouse_process},
    {"press",              st_mouse_press},
    {"release",            st_mouse_release},
    {"pressed",            st_mouse_pressed},
    {"get_wheel_relative", st_mouse_get_wheel_relative},
    {"moved",              st_mouse_moved},
    {"entered",            st_mouse_entered},
    {"leaved",             st_mouse_leaved},
    {"get_x",              st_mouse_get_x},
    {"get_y",              st_mouse_get_y},
    {"get_window",         st_mouse_get_window},
    {NULL, NULL},
};
