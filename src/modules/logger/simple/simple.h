#ifndef ST_SRC_MODULES_LOGGER_SIMPLE_SIMPLE_H
#define ST_SRC_MODULES_LOGGER_SIMPLE_SIMPLE_H

#include <limits.h>
#include <stdio.h>

#include "config.h" // IWYU pragma: keep
#include "steroids/logger.h"

// version

#define ST_LOGGER_LOG_FILES_MAX 16
#define ST_LOGGER_CALLBACKS_MAX 16

typedef struct {
    FILE       *file;
    char        filename[PATH_MAX];
    st_loglvl_t log_levels;
} st_logger_simple_log_file_t;

typedef struct {
    st_logcbk_t func;
    void       *userdata;
    st_loglvl_t log_levels;
} st_logger_simple_callback_t;

typedef struct {
    st_loglvl_t                 stdout_levels;
    st_loglvl_t                 stderr_levels;
    st_loglvl_t                 syslog_levels;
    st_logger_simple_log_file_t log_files[ST_LOGGER_LOG_FILES_MAX];
    unsigned                    log_files_count;
    st_logger_simple_callback_t callbacks[ST_LOGGER_CALLBACKS_MAX];
    unsigned                    callbacks_count;
} st_logger_simple_t;

st_logger_funcs_t st_logger_simple_funcs = {
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
st_modfuncstbl_t st_module_logger_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_logger_init", st_logger_init},
        {"st_logger_quit", st_logger_quit},
        {"st_logger_set_stdout_levels", st_logger_set_stdout_levels},
        {"st_logger_set_stderr_levels", st_logger_set_stderr_levels},
        {"st_logger_set_syslog_levels", st_logger_set_syslog_levels},
        {"st_logger_set_log_file", st_logger_set_log_file},
        {"st_logger_set_callback", st_logger_set_callback},
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
