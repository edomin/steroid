#include "simple.h"

#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>
#include <threads.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define CBK_BUF_SIZE     4096

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_logger_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_logger_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_logger_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module: logger_simple: %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_logger_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_logger_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static st_modctx_t *st_logger_init(st_modctx_t *events_ctx) {
    st_modctx_t        *logger_ctx = global_modsmgr_funcs.init_module_ctx(
     global_modsmgr, &st_module_logger_simple_data, sizeof(st_logger_simple_t));
    st_logger_simple_t *logger;

    if (!logger_ctx)
        return NULL;

    logger_ctx->funcs = &st_logger_simple_funcs;

    logger = logger_ctx->data;
    logger->events.ctx = events_ctx;
    logger->stdout_levels = ST_LL_NONE;
    logger->stderr_levels = ST_LL_ALL;
    logger->syslog_levels = ST_LL_NONE;
    logger->log_files_count = 0;
    logger->callbacks_count = 0;

    if (events_ctx && !st_logger_enable_events(logger_ctx, events_ctx))
        logger->events.ctx = NULL;

    if (mtx_init(&logger->lock, mtx_plain) == thrd_error) {
        st_logger_error(logger_ctx,
         "logger_simple: Unable to init lock mutex while initializing logger");
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);

        return NULL;
    }

    st_logger_info(logger_ctx, "%s", "logger_simple: Logger initialized");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_simple_t *logger = logger_ctx->data; // NOLINT(altera-id-dependent-backward-branch)

    st_logger_info(logger_ctx, "%s", "logger_simple: Destroying logger");

    if (logger->syslog_levels != ST_LL_NONE)
        closelog();

    for (unsigned i = 0; i < logger->log_files_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
        (void)fflush(logger->log_files[i].file);
        (void)fclose(logger->log_files[i].file);
    }

    mtx_destroy(&logger->lock);

    global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);
}

#define ST_LOGGER_LOAD_FUNCTION(mod, function)                               \
    module->mod.function = global_modsmgr_funcs.get_function_from_ctx(       \
     global_modsmgr, mod##_ctx, #function);                                  \
    if (!module->mod.function) {                                             \
        st_logger_error(logger_ctx,                                          \
         "logger_simple: Unable to load function \"%s\" from module \"%s\"", \
         #function, #mod);                                                   \
        return false;                                                        \
    }

static bool st_logger_import_events_functions(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx) {
    st_logger_simple_t *module = logger_ctx->data;

    ST_LOGGER_LOAD_FUNCTION(events, register_type);
    ST_LOGGER_LOAD_FUNCTION(events, push);

    return true;
}

static bool st_logger_enable_events(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx) {
    st_logger_simple_t *module = logger_ctx->data;

    if (!st_logger_import_events_functions(logger_ctx, events_ctx))
        goto import_fail;

    module->events.ctx = events_ctx;
    module->ev_log_output_debug = module->events.register_type(
     module->events.ctx, "log_output_debug", ST_EV_LOG_MSG_SIZE);
    if (module->ev_log_output_debug == ST_EVTYPE_ID_NONE)
        goto register_fail;

    module->ev_log_output_info = module->events.register_type(
     module->events.ctx, "log_output_info", ST_EV_LOG_MSG_SIZE);
    if (module->ev_log_output_info == ST_EVTYPE_ID_NONE)
        goto register_fail;

    module->ev_log_output_warning = module->events.register_type(
     module->events.ctx, "log_output_warning", ST_EV_LOG_MSG_SIZE);
    if (module->ev_log_output_warning == ST_EVTYPE_ID_NONE)
        goto register_fail;

    module->ev_log_output_error = module->events.register_type(
     module->events.ctx, "log_output_error", ST_EV_LOG_MSG_SIZE);
    if (module->ev_log_output_error == ST_EVTYPE_ID_NONE)
        goto register_fail;

    st_logger_info(logger_ctx, "logger_simple: Events enabled");

    return true;

register_fail:
    module->events.ctx = NULL;
import_fail:
    st_logger_error(logger_ctx, "logger_simple: Unable to enable events");

    return false;
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

static bool st_logger_set_syslog_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    st_logger_simple_t *logger = logger_ctx->data;

    if (logger->syslog_levels == ST_LL_NONE && levels != ST_LL_NONE)
        openlog("steroids", LOG_CONS, LOG_USER); // NOLINT(hicpp-signed-bitwise)
    else if (levels == ST_LL_NONE)
        closelog();

    logger->syslog_levels = levels;

    return true;
}

static bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels) {
    st_logger_simple_t *logger = logger_ctx->data;
    unsigned            file_num = logger->log_files_count;

    for (unsigned i = 0; i < logger->log_files_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
        bool filenames_equal = strcmp(filename,
         logger->log_files[logger->log_files_count].filename) == 0;

        if (filenames_equal) {
            file_num = i;

            break;
        }
    }

    if (file_num == logger->log_files_count) {
        if (file_num == ST_LOGGER_LOG_FILES_MAX) {
            st_logger_error(logger_ctx, "%s",
             "logger_simple: Unable to set log file because opened files "
             "limit reached.");

            return false;
        }
        logger->log_files[file_num].file = fopen(filename, "wbe");
        logger->log_files_count++;
    }

    logger->log_files[file_num].log_levels = levels;

    return true;
}

static bool st_logger_set_callback(st_modctx_t *logger_ctx,
 st_logcbk_t callback, void *userdata, st_loglvl_t levels) {
    st_logger_simple_t *logger = logger_ctx->data;
    unsigned            cbk_num = logger->callbacks_count;

    for (unsigned i = 0; i < logger->callbacks_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
        if (callback == logger->callbacks[logger->callbacks_count].func) {
            cbk_num = i;

            break;
        }
    }

    if (cbk_num == logger->callbacks_count) {
        if (cbk_num == ST_LOGGER_CALLBACKS_MAX) {
            st_logger_error(logger_ctx, "%s",
             "logger_simple: Unable to set callback because callbacks limit "
             "reached.");

            return false;
        }
        logger->callbacks[cbk_num].func = callback;
        logger->callbacks[cbk_num].userdata = userdata;
        logger->callbacks_count++;
    }

    logger->callbacks[cbk_num].log_levels = levels;

    return true;
}

static inline int st_logger_level_to_syslog_priority(st_loglvl_t log_level) {
    int      result = 0;
    unsigned u_log_level = log_level;

    for (u_log_level >>= 1u; u_log_level > 0; u_log_level >>= 1u) // NOLINT(altera-id-dependent-backward-branch)
        result++;

    return result;
}

static inline __attribute__((format (printf, 4, 0))) void st_logger_general(
 const st_modctx_t *logger_ctx, st_loglvl_t log_level, st_evtypeid_t evtype_id,
 const char *format, va_list args) {
    st_logger_simple_t *logger = logger_ctx->data;
    char                buffer[CBK_BUF_SIZE];

    if ((logger->stdout_levels & log_level) == log_level) {
        if (vprintf(format, args) > 0)
            putc('\n', stdout);
    }
    if ((logger->stderr_levels & log_level) == log_level) {
        if (vfprintf(stderr, format, args) > 0)
            putc('\n', stdout);
    }
    if ((logger->syslog_levels & log_level) == log_level)
        vsyslog(st_logger_level_to_syslog_priority(log_level), format, args);

    for (unsigned i = 0; i < logger->log_files_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
        if ((logger->log_files[i].log_levels & log_level) == log_level &&
         vfprintf(logger->log_files[i].file, format, args) > 0)
            fflush(logger->log_files[i].file);
    }

    if (vsprintf_s(buffer, CBK_BUF_SIZE, format, args) > 0) {
        for (unsigned i = 0; i < logger->callbacks_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
            if ((logger->callbacks[i].log_levels & log_level) == log_level) {
                logger->callbacks[i].func(buffer,
                 logger->callbacks[i].userdata);
            }
        }
    }

    if (logger->events.ctx) {
        buffer[ST_EV_LOG_MSG_SIZE - 1] = '\0';
        logger->events.push(logger->events.ctx, evtype_id, buffer);
    }
}

#define ST_LOGGER_SIMPLE_NOLOCK_FUNC(st_func, level, evtype_id_field)         \
    static __attribute__((format (printf, 2, 0))) void st_func(               \
     const st_modctx_t *logger_ctx, const char* format, va_list args) {       \
        st_logger_simple_t *module = logger_ctx->data;                        \
        st_logger_general(logger_ctx, level, module->evtype_id_field, format, \
         args);                                                               \
    }

ST_LOGGER_SIMPLE_NOLOCK_FUNC(st_logger_debug_nolock, ST_LL_DEBUG,
 ev_log_output_debug);
ST_LOGGER_SIMPLE_NOLOCK_FUNC(st_logger_info_nolock, ST_LL_INFO,
 ev_log_output_info);
ST_LOGGER_SIMPLE_NOLOCK_FUNC(st_logger_warning_nolock, ST_LL_WARNING,
 ev_log_output_warning);
ST_LOGGER_SIMPLE_NOLOCK_FUNC(st_logger_error_nolock, ST_LL_ERROR,
 ev_log_output_error);

#define ST_LOGGER_SIMPLE_LOG_FUNC(st_func, st_nolock_func) \
    static __attribute__((format (printf, 2, 3))) void st_func(   \
     const st_modctx_t *logger_ctx, const char* format, ...) {    \
        st_logger_simple_t *module = logger_ctx->data;            \
        va_list             args;                                 \
        if (mtx_lock(&module->lock) == thrd_error)                \
            return;                                               \
        va_start(args, format);                                   \
        st_nolock_func(logger_ctx, format, args);                 \
        mtx_unlock(&module->lock);                                \
        va_end(args);                                             \
    }

ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_debug    , st_logger_debug_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_info     , st_logger_info_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_warning  , st_logger_warning_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_error    , st_logger_error_nolock);