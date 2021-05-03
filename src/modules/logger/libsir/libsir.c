#include "libsir.h"

#include <stdarg.h>

#include <sir.h>

static void                     *global_modsmgr;
static st_modsmgr_get_function_t global_modsmgr_get_function;
static bool                      global_sir_inited = false;

void *st_module_logger_libsir_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_logger_libsir_funcs_table;

    for (size_t i = 0; i < funcs_table->funcs_count; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_logger_libsir_init(void *modsmgr,
 st_modsmgr_get_function_t modsmgr_get_function) {
    global_modsmgr = modsmgr;
    global_modsmgr_get_function = modsmgr_get_function;

    return &st_module_logger_libsir_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 void *modsmgr_get_function) {
    return st_module_logger_libsir_init(modsmgr, modsmgr_get_function);
}
#endif

//void ST_LoggerReportError(void) {
    //sirchar_t message[SIR_MAXERROR] = {0};
    //uint16_t  code = sir_geterror(message);

    //fprintf(stderr, "libsir error: (%hu, %s)\n", code, message);
//}

static void st_logger_init_fallback(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    logger->use_fallback_module = true;

    logger->logger_fallback_init = global_modsmgr_get_function(global_modsmgr,
     "logger", "simple", "st_logger_init");
    logger->logger_fallback_quit = global_modsmgr_get_function(global_modsmgr,
     "logger", "simple", "st_logger_quit");
    logger->logger_fallback_set_stdout_levels = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_set_stdout_levels");
    logger->logger_fallback_set_stderr_levels = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_set_stderr_levels");
    logger->logger_fallback_set_log_file = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_set_log_file");
    logger->logger_fallback_debug = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_debug");
    logger->logger_fallback_info = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_info");
    logger->logger_fallback_notice = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_notice");
    logger->logger_fallback_warning = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_warning");
    logger->logger_fallback_error = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_error");
    logger->logger_fallback_critical = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_critical");
    logger->logger_fallback_alert = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_alert");
    logger->logger_fallback_emergency = global_modsmgr_get_function(
     global_modsmgr, "logger", "simple", "st_logger_emergency");

    logger->logger_fallback_ctx = logger->logger_fallback_init();
    logger->logger_fallback_warning(logger->logger_fallback_ctx, "%s\n",
     "logger_libsir: Unable to initialize \"logger_libsir\" properly. Using "
     "fallback module \"logger_simple\" internally.");
}

static st_modctx_t *st_logger_init(void) {
    sirinit      si = {0};
    st_modctx_t *logger_ctx;
    // st_logger_libsir_t *logger;

    if (global_sir_inited)
        return NULL;

    logger_ctx = st_init_module_ctx(&st_module_logger_libsir_data,
     sizeof(st_logger_libsir_t));

    if (logger_ctx == NULL)
        return NULL;

    logger_ctx->funcs = &st_logger_libsir_funcs;

    si.d_stdout.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_stderr.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_syslog.levels = 0;

    strcpy(si.processName, "steroids"); // TODO: move name management to module

    if (sir_init(&si)) {
        st_logger_libsir_t *logger = logger_ctx->data;

        sir_stdoutlevels(ST_LL_NONE);
        sir_stderrlevels(ST_LL_ALL);
        sir_sysloglevels(ST_LL_NONE);
        logger->use_fallback_module = false;
    } else {
        st_logger_init_fallback(logger_ctx);
    }

    global_sir_inited = true;

    st_logger_info(logger_ctx, "%s\n", "logger_libsir: Logger initialized.");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    st_logger_info(logger_ctx, "%s\n", "logger_libsir: Destroying logger.");
    if (logger->use_fallback_module)
        logger->logger_fallback_quit(logger->logger_fallback_ctx);
    sir_cleanup();
    st_free_module_ctx(logger_ctx);
    global_sir_inited = false;
}

static bool st_logger_set_stdout_levels( st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_stdout_levels(
         logger->logger_fallback_ctx, levels);

    return sir_stdoutlevels(levels);
}

static bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_stderr_levels(
         logger->logger_fallback_ctx, levels);

    return sir_stderrlevels(levels);
}

static bool st_logger_set_syslog_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_syslog_levels(
         logger->logger_fallback_ctx, levels);

    return sir_sysloglevels(levels);
}

static bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;
    sirfileid_t file;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_log_file(
         logger->logger_fallback_ctx, filename, levels);

    file = sir_addfile(filename, SIRL_ALL,
     SIRO_NONAME | SIRO_NOPID | SIRO_NOTID);

    if (file == NULL)
        return false;

    return sir_filelevels(file, levels);
}

#define ST_LOGGER_MESSAGE_LEN_MAX 4096
#define ST_LOGGER_LIBSIR_LOG_FUNC(st_func, st_fallback_func, sir_func)   \
    static __attribute__ ((format (printf, 2, 3))) bool st_func(         \
     const st_modctx_t *logger_ctx, const char* format, ...) {           \
        st_logger_libsir_t *logger = logger_ctx->data;                   \
        va_list args;                                                    \
        char    message[ST_LOGGER_MESSAGE_LEN_MAX];                      \
        va_start(args, format);                                          \
        vsnprintf(message, ST_LOGGER_MESSAGE_LEN_MAX, format, args);     \
        va_end(args);                                                    \
        if (logger->use_fallback_module)                                 \
            return logger->st_fallback_func(logger->logger_fallback_ctx, \
             "%s", message);                                             \
        return sir_func("%s", message);                                  \
    }

ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_debug, logger_fallback_debug, sir_debug);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_info, logger_fallback_info, sir_info);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_notice, logger_fallback_notice, sir_notice);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_warning, logger_fallback_warning, sir_warn);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_error, logger_fallback_error, sir_error);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_critical, logger_fallback_critical,
 sir_crit);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_alert, logger_fallback_alert, sir_alert);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_emergency, logger_fallback_emergency,
 sir_emerg);
