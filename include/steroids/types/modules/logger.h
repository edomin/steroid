#ifndef ST_STEROIDS_TYPES_MODULES_LOGGER_H
#define ST_STEROIDS_TYPES_MODULES_LOGGER_H

#include <stdbool.h>

#include "steroids/module.h"

typedef enum {
    ST_LL_NONE    = 0x0,
    ST_LL_ERROR   = 0x8,
    ST_LL_WARNING = 0x10,
    ST_LL_INFO    = 0x40,
    ST_LL_DEBUG   = 0x80,
    ST_LL_ALL     = 0xFF,
} st_loglvl_t;

typedef void (*st_logcbk_t)(const char *msg, void *userdata);

typedef st_modctx_t *(*st_logger_init_t)(void);
typedef void (*st_logger_quit_t)(st_modctx_t *logger_ctx);

typedef bool (*st_logger_set_stdout_levels_t)(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
typedef bool (*st_logger_set_stderr_levels_t)(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
typedef bool (*st_logger_set_syslog_levels_t)(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
typedef bool (*st_logger_set_log_file_t)(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels);
typedef bool (*st_logger_set_callback_t)(st_modctx_t *logger_ctx,
 st_logcbk_t callback, void *userdata, st_loglvl_t levels);
typedef void (*st_logger_debug_t)(const st_modctx_t *logger_ctx,
 const char* format, ...);
typedef void (*st_logger_info_t)(const st_modctx_t *logger_ctx,
 const char* format, ...);
typedef void (*st_logger_warning_t)(const st_modctx_t *logger_ctx,
 const char* format, ...);
typedef void (*st_logger_error_t)(const st_modctx_t *logger_ctx,
 const char* format, ...);

typedef struct {
    st_logger_init_t              logger_init;
    st_logger_quit_t              logger_quit;
    st_logger_set_stdout_levels_t logger_set_stdout_levels;
    st_logger_set_stderr_levels_t logger_set_stderr_levels;
    st_logger_set_syslog_levels_t logger_set_syslog_levels;
    st_logger_set_log_file_t      logger_set_log_file;
    st_logger_set_callback_t      logger_set_callback;
    st_logger_debug_t             logger_debug;
    st_logger_info_t              logger_info;
    st_logger_warning_t           logger_warning;
    st_logger_error_t             logger_error;
} st_logger_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_LOGGER_H */
