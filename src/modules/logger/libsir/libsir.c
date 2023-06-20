#include "libsir.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#include <sir.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static bool               global_sir_inited = false;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_logger_libsir_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_logger_libsir_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_logger_libsir_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"logger_libsir\": %s\n",
         err_msg_buf);

        return NULL;
    }

    return &st_module_logger_libsir_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_logger_libsir_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_logger_init_fallback(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    logger->use_fallback_module = true;
    logger->logger_fallback_active = true;

    logger->logger_fallback_init = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_init");
    logger->logger_fallback_quit = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_quit");
    logger->logger_fallback_set_stdout_levels =
     global_modsmgr_funcs.get_function(global_modsmgr, "logger", "simple",
      "st_logger_set_stdout_levels");
    logger->logger_fallback_set_stderr_levels =
     global_modsmgr_funcs.get_function(global_modsmgr, "logger", "simple",
      "st_logger_set_stderr_levels");
    logger->logger_fallback_set_log_file = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_set_log_file");
    logger->logger_fallback_debug = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_debug");
    logger->logger_fallback_info = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_info");
    logger->logger_fallback_notice = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_notice");
    logger->logger_fallback_warning = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_warning");
    logger->logger_fallback_error = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_error");
    logger->logger_fallback_critical = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_critical");
    logger->logger_fallback_alert = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_alert");
    logger->logger_fallback_emergency = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "st_logger_emergency");

    logger->logger_fallback_ctx = logger->logger_fallback_init();
    logger->logger_fallback_warning(logger->logger_fallback_ctx, "%s\n",
     "logger_libsir: Unable to initialize \"logger_libsir\" properly. Using "
     "fallback module \"logger_simple\" internally.");
}

static st_modctx_t *st_logger_init(void) {
    sirinit      initOptions = {
        .d_stdout = {
            .levels = ST_LL_NONE,
            .opts = (sir_options)SIRO_NONAME | (sir_options)SIRO_NOPID |
             (sir_options)SIRO_NOTID,
        },
        .d_stderr = {
            .levels = ST_LL_ALL,
            .opts = (sir_options)SIRO_NONAME | (sir_options)SIRO_NOPID |
             (sir_options)SIRO_NOTID,
        },
        .d_syslog = {
            .levels = ST_LL_NONE,
            .includePID = true,
        },
        .processName = "steroids", /* TODO(edomin): move name management to
                                    * module
                                    */
    };
    st_modctx_t *logger_ctx;

    if (global_sir_inited)
        return NULL;

    logger_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_logger_libsir_data, sizeof(st_logger_libsir_t));

    if (logger_ctx == NULL)
        return NULL;

    logger_ctx->funcs = &st_logger_libsir_funcs;

    if (sir_init(&initOptions)) {
        st_logger_libsir_t *logger = logger_ctx->data;

        logger->use_fallback_module = false;
    } else {
        st_logger_init_fallback(logger_ctx);
    }

    global_sir_inited = true;

    st_logger_info(logger_ctx, "%s", "logger_libsir: Logger initialized.");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    st_logger_info(logger_ctx, "%s", "logger_libsir: Destroying logger.");
    if (logger->use_fallback_module)
        logger->logger_fallback_quit(logger->logger_fallback_ctx);
    sir_cleanup();
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);
    global_sir_inited = false;
}

static bool st_logger_set_stdout_levels( st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_stdout_levels(
         logger->logger_fallback_ctx, levels);

    return sir_stdoutlevels((sir_levels)levels);
}

static bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_stderr_levels(
         logger->logger_fallback_ctx, levels);

    return sir_stderrlevels((sir_levels)levels);
}

static bool st_logger_set_syslog_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_syslog_levels(
         logger->logger_fallback_ctx, levels);

    return sir_sysloglevels((sir_levels)levels);
}

static bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;
    sirfileid_t file;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_log_file(
         logger->logger_fallback_ctx, filename, levels);

    file = sir_addfile(filename, SIRL_ALL,
     (sir_options)SIRO_NONAME | (sir_options)SIRO_NOPID |
     (sir_options)SIRO_NOTID);

    if (file == NULL)
        return false;

    return sir_filelevels(file, (sir_levels)levels);
}

#define ST_LOGGER_MESSAGE_LEN_MAX 4096
#define ST_LOGGER_LIBSIR_LOG_FUNC(st_func, st_fallback_func, sir_func)   \
    static __attribute__ ((format (printf, 2, 3))) bool st_func(         \
     const st_modctx_t *logger_ctx, const char* format, ...) {           \
        st_logger_libsir_t *logger = logger_ctx->data;                   \
        va_list args;                                                    \
        char    message[ST_LOGGER_MESSAGE_LEN_MAX];                      \
        va_start(args, format);                                          \
        vsnprintf_s(message, ST_LOGGER_MESSAGE_LEN_MAX, format, args);   \
        va_end(args);                                                    \
        if (logger->use_fallback_module)                                 \
            return logger->st_fallback_func(logger->logger_fallback_ctx, \
             "%s", message);                                             \
        return sir_func("%s", message);                                  \
    }

ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_debug, logger_fallback_debug, sir_debug); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_info, logger_fallback_info, sir_info); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_notice, logger_fallback_notice, sir_notice); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_warning, logger_fallback_warning, sir_warn); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_error, logger_fallback_error, sir_error); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_critical, logger_fallback_critical, // NOLINT(cert-err33-c)
 sir_crit);
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_alert, logger_fallback_alert, sir_alert); // NOLINT(cert-err33-c)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_emergency, logger_fallback_emergency, // NOLINT(cert-err33-c)
 sir_emerg);
