#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/keyboard.h"

st_keyboard_funcs_t st_keyboard_simple_funcs = {
    .keyboard_init      = st_keyboard_init,
    .keyboard_quit      = st_keyboard_quit,
    .keyboard_process   = st_keyboard_process,
    .keyboard_press     = st_keyboard_press,
    .keyboard_release   = st_keyboard_release,
    .keyboard_pressed   = st_keyboard_pressed,
    .keyboard_get_input = st_keyboard_get_input,
};

st_modfuncentry_t st_module_keyboard_simple_funcs[] = {
    {"init",      st_keyboard_init},
    {"quit",      st_keyboard_quit},
    {"process",   st_keyboard_process},
    {"press",     st_keyboard_press},
    {"release",   st_keyboard_release},
    {"pressed",   st_keyboard_pressed},
    {"get_input", st_keyboard_get_input},
    {NULL, NULL},
};
