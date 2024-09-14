#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/opts.h"

static st_modfuncentry_t st_module_opts_ketopt_funcs[] = {
    {"init"      , st_opts_init},
    {"quit"      , st_opts_quit},
    {"add_option", st_opts_add_option},
    {"get_str"   , st_opts_get_str},
    {"get_help"  , st_opts_get_help},
    {NULL, NULL},
};
