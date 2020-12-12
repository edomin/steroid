#ifndef ST_MODULES_LOGGER_LIBSIR_LIBSIR_H
#define ST_MODULES_LOGGER_LIBSIR_LIBSIR_H

#include "config.h"
#include "steroids/logger.h"

// version

typedef struct {
    char unused;
} st_logger_libsir_t;

st_modfuncstbl_t st_module_logger_libsir_funcs_table = {
    .funcs_count = 13,
    .entries = {
        {"st_logger_init", st_logger_init},
        {"st_logger_quit", st_logger_quit},
        {"st_logger_set_stdout_levels", st_logger_set_stdout_levels},
        {"st_logger_set_stderr_levels", st_logger_set_stderr_levels},
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
