#include "libsir.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sir.h>
#pragma GCC diagnostic pop
#include <sir/errors.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(logger_libsir)
ST_MODULE_DEF_INIT_FUNC(logger_libsir)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_logger_libsir_init(modsmgr, modsmgr_funcs);
}
#endif

static void log_file_destroy(void *plog_file) {
    st_logger_libsir_log_file_t *log_file = plog_file;

    sir_remfile(log_file->file);
}

static st_modctx_t *st_logger_init(st_modctx_t *events_ctx) {
    st_logger_libsir_t *module;
    sirinit             init_options = {
        .d_stdout = {
            .levels = ST_LL_NONE,
            .opts = (sir_options)SIRO_NOHOST | (sir_options)SIRO_NONAME |
             (sir_options)SIRO_NOPID | (sir_options)SIRO_NOTID,
        },
        .d_stderr = {
            .levels = ST_LL_ALL,
            .opts = (sir_options)SIRO_NOHOST | (sir_options)SIRO_NONAME |
             (sir_options)SIRO_NOPID | (sir_options)SIRO_NOTID,
        },
        .d_syslog = {
            .levels = ST_LL_NONE,
            .opts = (sir_options)SIRO_NOHOST,
            .identity = "",
            .category = "",
        },
        .name = "steroids", /* TODO(edomin): move name management to module */
    };
    st_modctx_t *logger_ctx;

    if (sir_isinitialized())
        return NULL;

    logger_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_logger_libsir_data, sizeof(st_logger_libsir_t));

    if (!logger_ctx)
        return NULL;

    logger_ctx->funcs = &st_logger_libsir_funcs;

    module = logger_ctx->data;

    if (!sir_init(&init_options))
        return NULL;

    module->events.ctx = events_ctx;

    module->callbacks = st_dlist_create(sizeof(st_logger_libsir_callback_t),
     NULL);
    if (!module->callbacks)
        st_logger_warning(logger_ctx, "logger_libsir: Unable to initialize "
         "dlist for callbacks. Logging callbacks is not available on this run");

    module->log_files = st_dlist_create(sizeof(st_logger_libsir_log_file_t),
     log_file_destroy);
    if (!module->log_files)
        st_logger_warning(logger_ctx, "logger_libsir: Unable to initialize "
         "dlist for log files. Logging to file is not available on this run");

    if (events_ctx && !st_logger_enable_events(logger_ctx, events_ctx))
        module->events.ctx = NULL;

    if (mtx_init(&module->lock, mtx_plain) == thrd_error) {
        st_logger_error(logger_ctx,
         "logger_libsir: Unable to init lock mutex while initializing "
         "logger");
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);

        return NULL;
    }

    st_logger_info(logger_ctx, "logger_libsir: Logger initialized");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *module = logger_ctx->data;

    if (!sir_isinitialized())
        return;

    st_logger_info(logger_ctx, "logger_libsir: Destroying logger");

    if (module->callbacks)
        st_dlist_destroy(module->callbacks);

    if (module->log_files)
        st_dlist_destroy(module->log_files);

    sir_cleanup();
    mtx_destroy(&module->lock);

    printf("%s", module->postmortem_msg);

    global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);
}

#define ST_LOGGER_LOAD_FUNCTION(mod, function)                               \
    module->mod.function = global_modsmgr_funcs.get_function_from_ctx(       \
     global_modsmgr, mod##_ctx, #function);                                  \
    if (!module->mod.function) {                                             \
        st_logger_error(logger_ctx,                                          \
         "logger_libsir: Unable to load function \"%s\" from module \"%s\"", \
         #function, #mod);                                                   \
        return false;                                                        \
    }

static bool st_logger_import_events_functions(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx) {
    st_logger_libsir_t *module = logger_ctx->data;

    ST_LOGGER_LOAD_FUNCTION(events, register_type);
    ST_LOGGER_LOAD_FUNCTION(events, push);

    return true;
}

static bool st_logger_enable_events(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx) {
    st_logger_libsir_t *module = logger_ctx->data;

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

    st_logger_info(logger_ctx, "logger_libsir: Events enabled");

    return true;

register_fail:
    module->events.ctx = NULL;
import_fail:
    st_logger_error(logger_ctx, "logger_libsir: Unable to enable events");

    return false;
}

static bool st_logger_set_stdout_levels( st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    return sir_stdoutlevels((sir_levels)levels);
}

static bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    return sir_stderrlevels((sir_levels)levels);
}

static bool st_logger_set_syslog_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels) {
    return sir_sysloglevels((sir_levels)levels);
}

static bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels) {
    st_logger_libsir_t *module;
    st_dlnode_t        *node;

    if (!logger_ctx || !filename)
        return false;

    module = logger_ctx->data;

    if (!module->log_files)
        return false;

    node = st_dlist_get_head(module->log_files);

    while (node) {
        st_logger_libsir_log_file_t *log_file = st_dlist_get_data(node);

        if (strncmp(log_file->filename, filename, PATH_MAX) == 0) {
            if (levels == ST_LL_NONE) {
                st_dlist_remove(node);
            } else if (!sir_filelevels(log_file->file, (sir_levels)levels)) {
                char errmsg[SIR_MAXERROR];

                if (sir_geterror(errmsg) == SIR_E_NOERROR) {
                    st_logger_error(logger_ctx,
                     "logger_libsir: Unable to update \"%s\" log file levels: "
                     "%s", filename, errmsg);
                } else {
                    st_logger_error(logger_ctx,
                     "logger_libsir: Unable to update \"%s\" log file levels",
                     filename);
                }

                return false;
            }

            return true;
        }

        node = st_dlist_get_next(node);
    }

    if (levels != ST_LL_NONE) {
        int                         ret;
        st_logger_libsir_log_file_t log_file;

        ret = snprintf(log_file.filename, PATH_MAX, "%s", filename);
        if (ret < 0 || ret == PATH_MAX) {
            st_logger_error(logger_ctx,
             "logger_libsir: Unable to set log filename");

            return false;
        }

        log_file.file = sir_addfile(filename, SIRL_ALL,
         (sir_options)SIRO_NONAME | (sir_options)SIRO_NOPID |
         (sir_options)SIRO_NOTID);
        if (!log_file.file) {
            char errmsg[SIR_MAXERROR];

            if (sir_geterror(errmsg) == SIR_E_NOERROR) {
                st_logger_error(logger_ctx,
                 "logger_libsir: Unable to add \"%s\" log file: %s", filename,
                 errmsg);
            } else {
                st_logger_error(logger_ctx,
                 "logger_libsir: Unable to add \"%s\" log file", filename);
            }

            return false;
        }

        if (!sir_filelevels(log_file.file, (sir_levels)levels)) {
            char errmsg[SIR_MAXERROR];

            if (sir_geterror(errmsg) == SIR_E_NOERROR) {
                st_logger_error(logger_ctx,
                 "logger_libsir: Unable to set \"%s\" log file levels: %s",
                 filename, errmsg);
            } else {
                st_logger_error(logger_ctx,
                 "logger_libsir: Unable to set \"%s\" log file levels",
                 filename);
            }

            sir_remfile(log_file.file);

            return false;
        }

        if (!!st_dlist_push_back(module->log_files, &log_file)) {
            sir_remfile(log_file.file);

            st_logger_error(logger_ctx,
             "logger_libsir: Unable to add \"%s\" log file entry to the list",
             filename);

            return false;
        }
    }

    return true;
}

static bool st_logger_set_callback(st_modctx_t *logger_ctx, st_logcbk_t func,
 void *userdata, st_loglvl_t levels) {
    st_logger_libsir_t *module;
    st_dlnode_t        *node;

    if (!logger_ctx || !func)
        return false;

    module = logger_ctx->data;

    if (!module->callbacks)
        return false;

    node = st_dlist_get_head(module->callbacks);

    while (node) {
        st_logger_libsir_callback_t *callback = st_dlist_get_data(node);

        if (callback->func == func && callback->userdata == userdata) {
            if (levels == ST_LL_NONE)
                st_dlist_remove(node);
            else
                callback->log_levels = levels;

            return true;
        }

        node = st_dlist_get_next(node);
    }

    if (levels != ST_LL_NONE) {
        st_logger_libsir_callback_t callback = {
            .func       = func,
            .userdata   = userdata,
            .log_levels = levels,
        };

        if (!!st_dlist_push_back(module->callbacks, &callback)) {
            st_logger_error(logger_ctx,
             "libsir: Unable to add callback entry to the list");

            return false;
        }
    }

    return true;
}

#define ST_LOGGER_MESSAGE_LEN_MAX 4096
#define ST_LOGGER_NOLOCK_FUNC(st_func, sir_func, log_level, evtype_id_field) \
    static __attribute__ ((format (printf, 2, 0))) void st_func(             \
     const st_modctx_t *logger_ctx, const char* format, va_list args) {      \
        st_logger_libsir_t *module = logger_ctx->data;                       \
        char                message[ST_LOGGER_MESSAGE_LEN_MAX];              \
        vsnprintf(message, ST_LOGGER_MESSAGE_LEN_MAX, format, args);         \
        sir_func("%s", message);                                             \
        if (module->callbacks) {                                             \
            st_dlnode_t *node = st_dlist_get_head(module->callbacks);        \
            while (node) {                                                   \
                st_logger_libsir_callback_t *callback = st_dlist_get_data(   \
                 node);                                                      \
                if ((callback->log_levels & log_level) == log_level)         \
                    callback->func(message, callback->userdata);             \
                node = st_dlist_get_next(node);                              \
            }                                                                \
        }                                                                    \
        if (module->events.ctx) {                                            \
            size_t min_size = ST_EV_LOG_MSG_SIZE < ST_LOGGER_MESSAGE_LEN_MAX \
                ? ST_EV_LOG_MSG_SIZE                                         \
                : ST_LOGGER_MESSAGE_LEN_MAX;                                 \
            message[min_size - 1] = '\0';                                    \
            module->events.push(module->events.ctx, module->evtype_id_field, \
             message);                                                       \
        }                                                                    \
    }

ST_LOGGER_NOLOCK_FUNC(st_logger_debug_nolock, sir_debug, ST_LL_DEBUG, // NOLINT(cert-err33-c)
 ev_log_output_debug);
ST_LOGGER_NOLOCK_FUNC(st_logger_info_nolock, sir_info, ST_LL_INFO, // NOLINT(cert-err33-c)
 ev_log_output_info);
ST_LOGGER_NOLOCK_FUNC(st_logger_warning_nolock, sir_warn, ST_LL_WARNING, // NOLINT(cert-err33-c)
 ev_log_output_warning);
ST_LOGGER_NOLOCK_FUNC(st_logger_error_nolock, sir_error, ST_LL_ERROR, // NOLINT(cert-err33-c)
 ev_log_output_warning);

#define ST_LOGGER_LOG_FUNC(st_func, st_nolock_func) \
    static __attribute__((format (printf, 2, 3))) void st_func(   \
     const st_modctx_t *logger_ctx, const char* format, ...) {    \
        st_logger_libsir_t *module = logger_ctx->data;            \
        va_list             args;                                 \
        if (mtx_lock(&module->lock) == thrd_error)                \
            return;                                               \
        va_start(args, format);                                   \
        st_nolock_func(logger_ctx, format, args);                 \
        mtx_unlock(&module->lock);                                \
        va_end(args);                                             \
    }

ST_LOGGER_LOG_FUNC(st_logger_debug,   st_logger_debug_nolock);
ST_LOGGER_LOG_FUNC(st_logger_info,    st_logger_info_nolock);
ST_LOGGER_LOG_FUNC(st_logger_warning, st_logger_warning_nolock);
ST_LOGGER_LOG_FUNC(st_logger_error,   st_logger_error_nolock);

static void st_logger_set_postmortem_msg(st_modctx_t *logger_ctx,
 const char *msg) {
    st_logger_libsir_t *module = logger_ctx->data;

    snprintf(module->postmortem_msg, ST_POSTMORTEM_MSG_SIZE_MAX, "%s", msg);
}
