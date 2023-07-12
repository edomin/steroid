#ifndef ST_MODULES_RUNNER_SIMPLE_TYPES_H
#define ST_MODULES_RUNNER_SIMPLE_TYPES_H

#include "steroids/types/modules/ini.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/opts.h"
#include "steroids/types/modules/plugin.h"

typedef struct {
    st_modctx_t      *ctx;
    st_ini_load_t     load;
    st_ini_destroy_t  destroy;
    st_ini_fill_str_t fill_str;
} st_runner_simple_ini_t;

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_warning_t warning;
    st_logger_error_t   error;
} st_runner_simple_logger_t;

typedef struct {
    st_modctx_t         *ctx;
    st_opts_add_option_t add_option;
    st_opts_get_str_t    get_str;
} st_runner_simple_opts_t;

typedef struct {
    st_modctx_t         *ctx;
    st_plugin_load_t load;
} st_runner_simple_plugin_t;

typedef struct {
    st_runner_simple_ini_t    ini;
    st_runner_simple_logger_t logger;
    st_runner_simple_opts_t   opts;
    st_runner_simple_plugin_t plugin;
} st_runner_simple_t;

#endif /* ST_MODULES_RUNNER_SIMPLE_TYPES_H */