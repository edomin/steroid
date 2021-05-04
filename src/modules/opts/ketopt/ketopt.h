#ifndef ST_MODULES_OPTS_KETOPT_KETOPT_H
#define ST_MODULES_OPTS_KETOPT_KETOPT_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <ketopt.h>
#pragma GCC diagnostic pop

#include "config.h"
#include "steroids/opts.h"
#include "steroids/types/modules/logger.h"

#define ST_OPTS_OPTS_MAX 128

typedef struct {
    char         short_opt;
    int          longopt_index;
    st_opt_arg_t arg;
    char        *arg_fmt;
    char        *opt_descr;
    // char *arg;
} st_opt_data_t;

typedef struct {
    st_modctx_t          *logger_ctx;
    st_logger_debug_t     logger_debug;
    st_logger_info_t      logger_info;
    st_logger_notice_t    logger_notice;
    st_logger_warning_t   logger_warning;
    st_logger_error_t     logger_error;
    st_logger_critical_t  logger_critical;
    st_logger_alert_t     logger_alert;
    st_logger_emergency_t logger_emergency;
    int                   argc;
    char                **argv;
    ko_longopt_t          longopts[ST_OPTS_OPTS_MAX];
    st_opt_data_t         opts_data[ST_OPTS_OPTS_MAX];
    unsigned              longopts_count;
    unsigned              opts_data_count;
} st_opts_ketopt_t;

st_modfuncstbl_t st_module_opts_ketopt_funcs_table = {
    .funcs_count = 4,
    .entries = {
        {"st_opts_init", st_opts_init},
        {"st_opts_quit", st_opts_quit},
        {"st_opts_add_option", st_opts_add_option},
        {"st_opts_get_str", st_opts_get_str},
    }
};

#endif
