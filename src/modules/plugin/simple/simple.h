#ifndef ST_MODULES_PLUGIN_SIMPLE_SIMPLE_H
#define ST_MODULES_PLUGIN_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/plugin.h"

st_plugin_funcs_t st_plugin_simple_funcs = {
    .plugin_init    = st_plugin_init,
    .plugin_quit    = st_plugin_quit,
    .plugin_load    = st_plugin_load,
    .plugin_memload = st_plugin_memload,
};

st_modfuncentry_t st_module_plugin_simple_funcs[] = {
    {"init"   , st_plugin_init},
    {"quit"   , st_plugin_quit},
    {"load"   , st_plugin_load},
    {"memload", st_plugin_memload},
    {NULL, NULL},
};

#endif /* ST_MODULES_PLUGIN_SIMPLE_SIMPLE_H */
