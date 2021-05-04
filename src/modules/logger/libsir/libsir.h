#ifndef ST_MODULES_LOGGER_LIBSIR_LIBSIR_H
#define ST_MODULES_LOGGER_LIBSIR_LIBSIR_H

#include "config.h"
#include "steroids/logger.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    bool                          use_fallback_module;
    st_modctx_t                  *logger_fallback_ctx;
    st_logger_init_t              logger_fallback_init;
    st_logger_quit_t              logger_fallback_quit;
    st_logger_set_stdout_levels_t logger_fallback_set_stdout_levels;
    st_logger_set_stderr_levels_t logger_fallback_set_stderr_levels;
    st_logger_set_syslog_levels_t logger_fallback_set_syslog_levels;
    st_logger_set_log_file_t      logger_fallback_set_log_file;
    st_logger_debug_t             logger_fallback_debug;
    st_logger_info_t              logger_fallback_info;
    st_logger_notice_t            logger_fallback_notice;
    st_logger_warning_t           logger_fallback_warning;
    st_logger_error_t             logger_fallback_error;
    st_logger_critical_t          logger_fallback_critical;
    st_logger_alert_t             logger_fallback_alert;
    st_logger_emergency_t         logger_fallback_emergency;
} st_logger_libsir_t;

st_logger_funcs_t st_logger_libsir_funcs = {
    .logger_init = st_logger_init,
    .logger_quit = st_logger_quit,
    .logger_set_stdout_levels = st_logger_set_stdout_levels,
    .logger_set_stderr_levels = st_logger_set_stderr_levels,
    .logger_set_syslog_levels = st_logger_set_syslog_levels,
    .logger_set_log_file = st_logger_set_log_file,
    .logger_debug = st_logger_debug,
    .logger_info = st_logger_info,
    .logger_notice = st_logger_notice,
    .logger_warning = st_logger_warning,
    .logger_error = st_logger_error,
    .logger_critical = st_logger_critical,
    .logger_alert = st_logger_alert,
    .logger_emergency = st_logger_emergency,
};

st_modfuncstbl_t st_module_logger_libsir_funcs_table = {
    .funcs_count = 14,
    .entries = {
        {"st_logger_init", st_logger_init},
        {"st_logger_quit", st_logger_quit},
        {"st_logger_set_stdout_levels", st_logger_set_stdout_levels},
        {"st_logger_set_stderr_levels", st_logger_set_stderr_levels},
        {"st_logger_set_syslog_levels", st_logger_set_syslog_levels},
        {"st_logger_set_log_file", st_logger_set_log_file},
        {"st_logger_debug", st_logger_debug},
        {"st_logger_info", st_logger_info},
        {"st_logger_notice", st_logger_notice},
        {"st_logger_warning", st_logger_warning},
        {"st_logger_error", st_logger_error},
        {"st_logger_critical", st_logger_critical},
        {"st_logger_alert", st_logger_alert},
        {"st_logger_emergency", st_logger_emergency},
    }
};

#endif
