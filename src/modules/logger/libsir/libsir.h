#ifndef ST_MODULES_LOGGER_LIBSIR_LIBSIR_H
#define ST_MODULES_LOGGER_LIBSIR_LIBSIR_H

#include <stdbool.h>

#include "config.h" // IWYU pragma: keep
#include "steroids/logger.h"

#define ST_LOGGER_CALLBACKS_MAX 16

typedef struct {
    st_logcbk_t func;
    void       *userdata;
    st_loglvl_t log_levels;
} st_logger_libsir_callback_t;

typedef struct {
    bool                          use_fallback_module;
    bool                          logger_fallback_active;
    st_modctx_t                  *logger_fallback_ctx;
    st_logger_init_t              logger_fallback_init;
    st_logger_quit_t              logger_fallback_quit;
    st_logger_set_stdout_levels_t logger_fallback_set_stdout_levels;
    st_logger_set_stderr_levels_t logger_fallback_set_stderr_levels;
    st_logger_set_syslog_levels_t logger_fallback_set_syslog_levels;
    st_logger_set_log_file_t      logger_fallback_set_log_file;
    st_logger_set_callback_t      logger_fallback_set_callback;
    st_logger_debug_t             logger_fallback_debug;
    st_logger_info_t              logger_fallback_info;
    st_logger_notice_t            logger_fallback_notice;
    st_logger_warning_t           logger_fallback_warning;
    st_logger_error_t             logger_fallback_error;
    st_logger_critical_t          logger_fallback_critical;
    st_logger_alert_t             logger_fallback_alert;
    st_logger_emergency_t         logger_fallback_emergency;
    st_logger_libsir_callback_t   callbacks[ST_LOGGER_CALLBACKS_MAX];
    unsigned                      callbacks_count;
} st_logger_libsir_t;

st_logger_funcs_t st_logger_libsir_funcs = {
    .logger_init = st_logger_init,
    .logger_quit = st_logger_quit,
    .logger_set_stdout_levels = st_logger_set_stdout_levels,
    .logger_set_stderr_levels = st_logger_set_stderr_levels,
    .logger_set_syslog_levels = st_logger_set_syslog_levels,
    .logger_set_log_file = st_logger_set_log_file,
    .logger_set_callback = st_logger_set_callback,
    .logger_debug = st_logger_debug,
    .logger_info = st_logger_info,
    .logger_notice = st_logger_notice,
    .logger_warning = st_logger_warning,
    .logger_error = st_logger_error,
    .logger_critical = st_logger_critical,
    .logger_alert = st_logger_alert,
    .logger_emergency = st_logger_emergency,
};

#define FUNCS_COUNT 14
st_modfuncstbl_t st_module_logger_libsir_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init",              st_logger_init},
        {"quit",              st_logger_quit},
        {"set_stdout_levels", st_logger_set_stdout_levels},
        {"set_stderr_levels", st_logger_set_stderr_levels},
        {"set_syslog_levels", st_logger_set_syslog_levels},
        {"set_log_file",      st_logger_set_log_file},
        {"set_callback",      st_logger_set_callback},
        {"debug",             st_logger_debug},
        {"info",              st_logger_info},
        {"notice",            st_logger_notice},
        {"warning",           st_logger_warning},
        {"error",             st_logger_error},
        {"critical",          st_logger_critical},
        {"alert",             st_logger_alert},
        {"emergency",         st_logger_emergency},
    }
};

#endif
