#pragma once

#include <limits.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sir.h>
#pragma GCC diagnostic pop

#include "config.h" // IWYU pragma: keep
#include "dlist.h"
#include "steroids/logger.h"
#include "steroids/types/modules/events.h"

#define ST_POSTMORTEM_MSG_SIZE_MAX 131072

typedef struct {
    st_modctx_t              *ctx;
    st_events_register_type_t register_type;
    st_events_push_t          push;
} st_logger_libsir_events_t;

typedef struct {
    char      filename[PATH_MAX];
    sirfileid file;
} st_logger_libsir_log_file_t;

typedef struct {
    st_logcbk_t func;
    void       *userdata;
    st_loglvl_t log_levels;
} st_logger_libsir_callback_t;

typedef struct {
    st_logger_libsir_events_t   events;
    st_dlist_t                 *log_files;
    st_dlist_t                 *callbacks;
    st_evtypeid_t               ev_log_output_debug;
    st_evtypeid_t               ev_log_output_info;
    st_evtypeid_t               ev_log_output_warning;
    st_evtypeid_t               ev_log_output_error;
    char                        postmortem_msg[ST_POSTMORTEM_MSG_SIZE_MAX];
} st_logger_libsir_t;

st_logger_funcs_t st_logger_libsir_funcs = {
    .logger_init               = st_logger_init,
    .logger_quit               = st_logger_quit,
    .logger_enable_events      = st_logger_enable_events,
    .logger_set_stdout_levels  = st_logger_set_stdout_levels,
    .logger_set_stderr_levels  = st_logger_set_stderr_levels,
    .logger_set_syslog_levels  = st_logger_set_syslog_levels,
    .logger_set_log_file       = st_logger_set_log_file,
    .logger_set_callback       = st_logger_set_callback,
    .logger_debug              = st_logger_debug,
    .logger_info               = st_logger_info,
    .logger_warning            = st_logger_warning,
    .logger_error              = st_logger_error,
    .logger_set_postmortem_msg = st_logger_set_postmortem_msg,
};

st_modfuncentry_t st_module_logger_libsir_funcs[] = {
    {"init",               st_logger_init},
    {"quit",               st_logger_quit},
    {"enable_events",      st_logger_enable_events},
    {"set_stdout_levels",  st_logger_set_stdout_levels},
    {"set_stderr_levels",  st_logger_set_stderr_levels},
    {"set_syslog_levels",  st_logger_set_syslog_levels},
    {"set_log_file",       st_logger_set_log_file},
    {"set_callback",       st_logger_set_callback},
    {"debug",              st_logger_debug},
    {"info",               st_logger_info},
    {"warning",            st_logger_warning},
    {"error",              st_logger_error},
    {"set_postmortem_msg", st_logger_set_postmortem_msg},
    {NULL, NULL},
};
