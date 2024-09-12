#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/fnv1a.h"

static st_modfuncentry_t st_module_fnv1a_simple_funcs[] = {
    {"init"                 , st_fnv1a_init},
    {"quit"                 , st_fnv1a_quit},
    {"u32hash_string"       , st_fnv1a_u32hash_string},
    {"u32hash_bytes"        , st_fnv1a_u32hash_bytes},
    {"get_u32hashstr_func"  , st_fnv1a_get_u32hashstr_func},
    {"get_u32hashbytes_func", st_fnv1a_get_u32hashbytes_func},
    {NULL, NULL},
};
