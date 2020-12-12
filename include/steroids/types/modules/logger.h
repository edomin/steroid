#ifndef ST_STEROIDS_TYPES_MODULES_LOGGER_H
#define ST_STEROIDS_TYPES_MODULES_LOGGER_H

#include <stdbool.h>

typedef enum {
    ST_LL_NONE      = 0x0,
    ST_LL_EMERGENCY = 0x1,
    ST_LL_ALERT     = 0x2,
    ST_LL_CRITICAL  = 0x4,
    ST_LL_ERROR     = 0x8,
    ST_LL_WARNING   = 0x10,
    ST_LL_NOTICE    = 0x20,
    ST_LL_INFO      = 0x40,
    ST_LL_DEBUG     = 0x80,
    ST_LL_ALL       = 0xFF
} st_loglvl_t;

#ifdef ST_ENABLE_FUNC_DECLS

typedef void *(*st_logger_init_t)(void);
typedef void (*st_logger_quit_t)(void *logger_ctx);
typedef bool (*st_logger_set_stdout_levels_t)(void *logger_ctx,
 st_loglvl_t levels);
typedef bool (*st_logger_set_stderr_levels_t)(void *logger_ctx,
 st_loglvl_t levels);
typedef bool (*st_logger_set_log_file_t)(void *logger_ctx,
 const char *filename, st_loglvl_t levels);
typedef bool (*st_logger_debug_t)(void *logger_ctx, const char* format,
 ...);
typedef bool (*st_logger_info_t)(void *logger_ctx, const char* format,
 ...);
typedef bool (*st_logger_notice_t)(void *logger_ctx, const char* format,
 ...);
typedef bool (*st_logger_warning_t)(void *logger_ctx, const char* format, ...);
typedef bool (*st_logger_error_t)(void *logger_ctx, const char* format,
 ...);
typedef bool (*st_logger_critical_t)(void *logger_ctx,
 const char* format, ...);
typedef bool (*st_logger_alert_t)(void *logger_ctx, const char* format,
 ...);
typedef bool (*st_logger_emergency_t)(void *logger_ctx,
 const char* format, ...);

#endif /* ST_ENABLE_FUNC_DECLS */
#undef ST_ENABLE_FUNC_DECLS

#endif /* ST_STEROIDS_TYPES_MODULES_LOGGER_H */
