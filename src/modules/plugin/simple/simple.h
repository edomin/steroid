#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/plugin.h"

static st_modfuncentry_t st_module_plugin_simple_funcs[] = {
    {"init"   , st_plugin_init},
    {"quit"   , st_plugin_quit},
    {"load"   , st_plugin_load},
    {"memload", st_plugin_memload},
    {NULL, NULL},
};
