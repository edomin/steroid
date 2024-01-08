#pragma once

#include <stdbool.h>
#include <threads.h>

#include "config.h" // IWYU pragma: keep
#include "steroids/logger.h"
#include "steroids/types/modules/events.h"

#define ST_LOGGER_CALLBACKS_MAX    16
#define ST_POSTMORTEM_MSG_SIZE_MAX 131072

typedef struct {
    st_modctx_t              *ctx;
    st_events_register_type_t register_type;
    st_events_push_t          push;
} st_logger_libsir_events_t;

typedef struct {
    st_logcbk_t func;
    void       *userdata;
    st_loglvl_t log_levels;
} st_logger_libsir_callback_t;

typedef struct {
    st_logger_libsir_events_t      events;
    bool                           use_fallback_module;
    bool                           logger_fallback_active;
    st_modctx_t                   *logger_fallback_ctx;
    st_logger_init_t               logger_fallback_init;
    st_logger_quit_t                logger_fallback_quit;
    st_logger_set_stdout_levels_t  logger_fallback_set_stdout_levels;
    st_logger_set_stderr_levels_t  logger_fallback_set_stderr_levels;
    st_logger_set_syslog_levels_t  logger_fallback_set_syslog_levels;
    st_logger_set_log_file_t       logger_fallback_set_log_file;
    st_logger_set_callback_t       logger_fallback_set_callback;
    st_logger_debug_t              logger_fallback_debug;
    st_logger_info_t               logger_fallback_info;
    st_logger_warning_t            logger_fallback_warning;
    st_logger_error_t              logger_fallback_error;
    st_logger_set_postmortem_msg_t logger_fallback_set_postmortem_msg;
    st_logger_libsir_callback_t    callbacks[ST_LOGGER_CALLBACKS_MAX];
    unsigned                       callbacks_count;
    st_evtypeid_t                  ev_log_output_debug;
    st_evtypeid_t                  ev_log_output_info;
    st_evtypeid_t                  ev_log_output_warning;
    st_evtypeid_t                  ev_log_output_error;
    mtx_t                          lock;
    char                           postmortem_msg[ST_POSTMORTEM_MSG_SIZE_MAX];
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
