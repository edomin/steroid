#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/spcpaths.h"

static st_modfuncentry_t st_module_spcpaths_cfgpath_funcs[] = {
    {"init"           , st_spcpaths_init},
    {"quit"           , st_spcpaths_quit},
    {"get_config_path", st_spcpaths_get_config_path},
    {"get_data_path"  , st_spcpaths_get_data_path},
    {"get_cache_path" , st_spcpaths_get_cache_path},
    {NULL, NULL},
};
