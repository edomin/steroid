#pragma once

#include "config.h" // IWYU pragma: keep
#include "steroids/spcpaths.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_spcpaths_cfgpath_logger_t;

typedef struct {
    st_spcpaths_cfgpath_logger_t logger;
} st_spcpaths_cfgpath_t;

st_spcpaths_funcs_t st_spcpaths_cfgpath_funcs = {
    .spcpaths_init            = st_spcpaths_init,
    .spcpaths_quit            = st_spcpaths_quit,
    .spcpaths_get_config_path = st_spcpaths_get_config_path,
    .spcpaths_get_data_path   = st_spcpaths_get_data_path,
    .spcpaths_get_cache_path  = st_spcpaths_get_cache_path,
};

st_modfuncentry_t st_module_spcpaths_cfgpath_funcs[] = {
    {"init"           , st_spcpaths_init},
    {"quit"           , st_spcpaths_quit},
    {"get_config_path", st_spcpaths_get_config_path},
    {"get_data_path"  , st_spcpaths_get_data_path},
    {"get_cache_path" , st_spcpaths_get_cache_path},
    {NULL, NULL},
};
