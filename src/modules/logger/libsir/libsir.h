#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/logger.h"

static st_modfuncentry_t st_module_logger_libsir_funcs[] = {
    {"init",               st_logger_init},
    {"quit",               st_logger_quit},
    {"enable_events",      st_logger_enable_events},
    {"set_stdout_levels",  st_logger_set_stdout_levels},
    {"set_stderr_levels",  st_logger_set_stderr_levels},
    {"set_log_file",       st_logger_set_log_file},
    {"set_callback",       st_logger_set_callback},
    {"debug",              st_logger_debug},
    {"info",               st_logger_info},
    {"warning",            st_logger_warning},
    {"error",              st_logger_error},
    {"set_postmortem_msg", st_logger_set_postmortem_msg},
    {NULL, NULL},
};
