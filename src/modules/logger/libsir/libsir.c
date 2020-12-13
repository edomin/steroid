#include "libsir.h"

#include <stdarg.h>

#include <sir.h>

static void                     *global_modsmgr;
static st_modsmgr_get_function_t global_modsmgr_get_function;
static bool                      global_sir_inited = false;

static st_logger_init_t  st_logger_simple_init;
static st_logger_quit_t  st_logger_simple_quit;
static st_logger_debug_t st_logger_simple_debug;

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

static st_modctx_t *st_logger_init(void) {
    sirinit      si = {0};
    st_modctx_t *logger_ctx;


    void         *logger_simple;



    if (global_sir_inited)
        return NULL;

    logger_ctx = st_init_module_ctx(&st_module_logger_libsir_data,
     sizeof(st_logger_libsir_t));

    if (logger_ctx == NULL)
        return NULL;

    si.d_stdout.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_stderr.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_syslog.levels = 0;

    strcpy(si.processName, "steroids"); // TODO: move name management to module

    if (!sir_init(&si)) {
        st_free_module_ctx(logger_ctx);

        return NULL;
    }

    sir_stdoutlevels(ST_LL_NONE);
    sir_stderrlevels(ST_LL_ALL);

    global_sir_inited = true;




    st_logger_simple_init = global_modsmgr_get_function(global_modsmgr, "logger", "simple",
     "st_logger_init");
    st_logger_simple_quit = global_modsmgr_get_function(global_modsmgr, "logger", "simple",
     "st_logger_quit");
    st_logger_simple_debug = global_modsmgr_get_function(global_modsmgr, "logger", "simple",
     "st_logger_debug");

    logger_simple = st_logger_simple_init();
    st_logger_simple_debug(logger_simple, "hello %s\n", "world");
    st_logger_simple_quit(logger_simple);


    return logger_ctx;
}

static void st_logger_quit(__attribute__((unused)) st_modctx_t *logger_ctx) {
    sir_cleanup();
    st_free_module_ctx(logger_ctx);
    global_sir_inited = false;
}

static bool st_logger_set_stdout_levels(
 __attribute__((unused)) st_modctx_t *logger_ctx, st_loglvl_t levels) {
    return sir_stdoutlevels(levels);
}

static bool st_logger_set_stderr_levels(
 __attribute__((unused)) st_modctx_t *logger_ctx, st_loglvl_t levels) {
    return sir_stderrlevels(levels);
}

static bool st_logger_set_log_file(
 __attribute__((unused)) st_modctx_t *logger_ctx, const char *filename,
 st_loglvl_t levels) {
    sirfileid_t file = sir_addfile(filename, SIRL_ALL,
     SIRO_NONAME | SIRO_NOPID | SIRO_NOTID);

    if (file == NULL)
        return false;

    return sir_filelevels(file, levels);
}

#define ST_LOGGER_MESSAGE_LEN_MAX 8192
#define ST_LOGGER_LIBSIR_LOG_FUNC(st_func, sir_func)                 \
    static __attribute__ ((format (printf, 2, 3))) bool st_func(     \
     __attribute__((unused)) const st_modctx_t *logger_ctx,          \
     const char* format,                                             \
     ...) {                                                          \
        va_list args;                                                \
        char    message[ST_LOGGER_MESSAGE_LEN_MAX];                  \
        va_start(args, format);                                      \
        vsnprintf(message, ST_LOGGER_MESSAGE_LEN_MAX, format, args); \
        va_end(args);                                                \
        return sir_func("%s", message);                              \
    }

ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_debug, sir_debug)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_info, sir_info)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_notice, sir_notice)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_warning, sir_warn)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_error, sir_error)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_critical, sir_crit)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_alert, sir_alert)
ST_LOGGER_LIBSIR_LOG_FUNC(st_logger_emergency, sir_emerg)
