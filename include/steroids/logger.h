#ifndef ST_STEROIDS_LOGGER_H
#define ST_STEROIDS_LOGGER_H

#include <stdbool.h>

#include "steroids/module.h"
#include "steroids/types/modules/logger.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_logger_init(void);
ST_VISIBILITY void st_logger_quit(st_modctx_t *logger_ctx);
ST_VISIBILITY bool st_logger_set_stdout_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
ST_VISIBILITY bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
ST_VISIBILITY bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels);
ST_VISIBILITY bool st_logger_debug(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_info(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_notice(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_warning(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_error(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_critical(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_alert(const st_modctx_t *logger_ctx,
 const char* format, ...);
ST_VISIBILITY bool st_logger_emergency(const st_modctx_t *logger_ctx,
 const char* format, ...);

#undef ST_VISIBILITY

#endif
