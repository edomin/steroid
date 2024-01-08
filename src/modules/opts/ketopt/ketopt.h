#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <klib/ketopt.h>
#pragma GCC diagnostic pop

#include "config.h" // IWYU pragma: keep
#include "steroids/opts.h"
#include "steroids/types/modules/logger.h"

#define ST_OPTS_OPTS_MAX 128

typedef struct {
    char        *longopt;
    int          longopt_index;
    char         shortopt;
    st_opt_arg_t arg;
    char        *arg_fmt;
    char        *opt_descr;
} st_opt_t;

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_warning_t warning;
    st_logger_error_t   error;
} st_opts_ketopt_logger_t;

typedef struct {
    st_opts_ketopt_logger_t logger;
    int                     argc;
    char                  **argv;
    st_opt_t                opts[ST_OPTS_OPTS_MAX];
    unsigned                opts_count;
} st_opts_ketopt_t;

st_opts_funcs_t st_opts_ketopt_funcs = {
    .opts_init       = st_opts_init,
    .opts_quit       = st_opts_quit,
    .opts_add_option = st_opts_add_option,
    .opts_get_str    = st_opts_get_str,
    .opts_get_help   = st_opts_get_help,
};

st_modfuncentry_t st_module_opts_ketopt_funcs[] = {
    {"init"      , st_opts_init},
    {"quit"      , st_opts_quit},
    {"add_option", st_opts_add_option},
    {"get_str"   , st_opts_get_str},
    {"get_help"  , st_opts_get_help},
    {NULL, NULL},
};
