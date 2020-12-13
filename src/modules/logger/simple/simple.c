#include "simple.h"

#include <stdarg.h>
#include <stdio.h>

void *st_module_logger_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_logger_simple_funcs_table;

    for (size_t i = 0; i < funcs_table->funcs_count; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_logger_simple_init(
 __attribute__((unused)) void *modsmgr,
 __attribute__((unused)) void *modsmgr_get_function) {
    return &st_module_logger_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr) {
    return st_module_logger_simple_init(void *modsmgr);
}
#endif

static st_modctx_t *st_logger_init(void) {
    st_modctx_t        *logger_ctx = st_init_module_ctx(
     &st_module_logger_simple_data, sizeof(st_logger_simple_t));
    st_logger_simple_t *logger;

    if (logger_ctx == NULL)
        return NULL;

    logger = logger_ctx->data;
    logger->stdout_levels = ST_LL_NONE;
    logger->stderr_levels = ST_LL_ALL;

    st_logger_info(logger_ctx, "%s\n", "logger_simple: Logger initialized.");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_info(logger_ctx, "%s\n", "logger_simple: Destroying logger.");
    st_free_module_ctx(logger_ctx);
}

static bool st_logger_set_stdout_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_simple_t *logger = logger_ctx->data;

    logger->stdout_levels = levels;

    return true;
}

static bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_simple_t *logger = logger_ctx->data;

    logger->stderr_levels = levels;

    return true;
}

static bool st_logger_set_log_file(
 __attribute__((unused)) st_modctx_t *logger_ctx,
 __attribute__((unused)) const char *filename,
 __attribute__((unused)) st_loglvl_t levels) {
    /* TODO */
    return false;
}

#define ST_LOGGER_SIMPLE_LOG_FUNC(st_func, level)                 \
    static  __attribute__ ((format (printf, 2, 3))) bool st_func( \
     const st_modctx_t *logger_ctx, const char* format, ...) {    \
        st_logger_simple_t *logger = logger_ctx->data;            \
        va_list             args;                                 \
        va_start(args, format);                                   \
        if ((logger->stdout_levels & level) == level)             \
            vprintf(format, args);                                \
        if ((logger->stderr_levels & level) == level)             \
            vfprintf(stderr, format, args);                       \
        va_end(args);                                             \
        return true;                                              \
    }

ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_debug, ST_LL_DEBUG);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_info, ST_LL_INFO);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_notice, ST_LL_NOTICE);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_warning, ST_LL_WARNING);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_error, ST_LL_ERROR);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_critical, ST_LL_CRITICAL);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_alert, ST_LL_ALERT);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_emergency, ST_LL_EMERGENCY);
