#include "libsir.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <sir.h>
#pragma GCC diagnostic pop

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static bool               global_sir_inited = false;

ST_MODULE_DEF_GET_FUNC(logger_libsir)
ST_MODULE_DEF_INIT_FUNC(logger_libsir)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_logger_libsir_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_logger_init_fallback(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    logger->use_fallback_module = true;
    logger->logger_fallback_active = true;

    logger->logger_fallback_init = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "init");
    logger->logger_fallback_quit = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "quit");
    logger->logger_fallback_set_stdout_levels =
     global_modsmgr_funcs.get_function(global_modsmgr, "logger", "simple",
      "set_stdout_levels");
    logger->logger_fallback_set_stderr_levels =
     global_modsmgr_funcs.get_function(global_modsmgr, "logger", "simple",
      "set_stderr_levels");
    logger->logger_fallback_set_log_file = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "set_log_file");
    logger->logger_fallback_set_callback = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "set_callback");
    logger->logger_fallback_debug = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "debug");
    logger->logger_fallback_info = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "info");
    logger->logger_fallback_warning = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "warning");
    logger->logger_fallback_error = global_modsmgr_funcs.get_function(
     global_modsmgr, "logger", "simple", "error");
    logger->logger_fallback_set_postmortem_msg =
     global_modsmgr_funcs.get_function(global_modsmgr, "logger", "simple",
      "set_postmortem_msg");

    logger->logger_fallback_ctx = logger->logger_fallback_init(events_ctx);
    logger->logger_fallback_warning(logger->logger_fallback_ctx, "%s\n",
     "logger_libsir: Unable to initialize \"logger_libsir\" properly. Using "
     "fallback module \"logger_simple\" internally.");
}

static st_modctx_t *st_logger_init(st_modctx_t *events_ctx) {
    bool         use_fallback = false;
    sirinit      init_options = {
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
        .name = "steroids", /* TODO(edomin): move name management to
                             * module
                             */
    };
    st_modctx_t *logger_ctx;

    if (global_sir_inited)
        return NULL;

    logger_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_logger_libsir_data, sizeof(st_logger_libsir_t));

    if (!logger_ctx)
        return NULL;

    logger_ctx->funcs = &st_logger_libsir_funcs;

    if (sir_init(&init_options)) {
        st_logger_libsir_t *logger = logger_ctx->data;

        logger->use_fallback_module = false;
        logger->events.ctx = events_ctx;
        logger->callbacks_count = 0;

        if (events_ctx && !st_logger_enable_events(logger_ctx, events_ctx))
            logger->events.ctx = NULL;

        if (mtx_init(&logger->lock, mtx_plain) == thrd_error) {
            st_logger_error(logger_ctx,
             "logger_libsir: Unable to init lock mutex while initializing "
             "logger");
            global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);
            use_fallback = true;
        }
    } else {
        use_fallback = true;
    }

    if (use_fallback)
        st_logger_init_fallback(logger_ctx, events_ctx);

    global_sir_inited = true;

    st_logger_info(logger_ctx, "logger_libsir: Logger initialized");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_libsir_t *logger = logger_ctx->data;

    st_logger_info(logger_ctx, "logger_libsir: Destroying logger");
    if (logger->use_fallback_module)
        logger->logger_fallback_quit(logger->logger_fallback_ctx);
    sir_cleanup();
    mtx_destroy(&logger->lock);

    printf("%s", logger->postmortem_msg);

    global_modsmgr_funcs.free_module_ctx(global_modsmgr, logger_ctx);
    global_sir_inited = false;
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
    sirfileid file;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_log_file(
         logger->logger_fallback_ctx, filename, levels);

    file = sir_addfile(filename, SIRL_ALL,
     (sir_options)SIRO_NONAME | (sir_options)SIRO_NOPID |
     (sir_options)SIRO_NOTID);

    if (file)
        return false;

    return sir_filelevels(file, (sir_levels)levels);
}

static bool st_logger_set_callback(st_modctx_t *logger_ctx,
 st_logcbk_t callback, void *userdata, st_loglvl_t levels) {
    st_logger_libsir_t *logger = logger_ctx->data;
    unsigned            cbk_num = logger->callbacks_count;

    if (logger->use_fallback_module)
        return logger->logger_fallback_set_callback(
         logger->logger_fallback_ctx, callback, userdata, levels);

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

#define ST_LOGGER_MESSAGE_LEN_MAX 4096
#define ST_LOGGER_NOLOCK_FUNC(st_func, st_fallback, sir_func, log_level,     \
 evtype_id_field)                                                            \
    static __attribute__ ((format (printf, 2, 0))) void st_func(             \
     const st_modctx_t *logger_ctx, const char* format, va_list args) {      \
        st_logger_libsir_t *logger = logger_ctx->data;                       \
        char                message[ST_LOGGER_MESSAGE_LEN_MAX];              \
        vsnprintf(message, ST_LOGGER_MESSAGE_LEN_MAX, format, args);         \
        if (logger->use_fallback_module)                                     \
            logger->st_fallback(logger->logger_fallback_ctx,                 \
             "%s", message);                                                 \
        else                                                                 \
            sir_func("%s", message);                                         \
        for (unsigned i = 0; i < logger->callbacks_count; i++) {             \
            if ((logger->callbacks[i].log_levels & (unsigned)(log_level)) == \
             (log_level))                                                    \
                logger->callbacks[i].func(message,                           \
                 logger->callbacks[i].userdata);                             \
        }                                                                    \
        if (logger->events.ctx) {                                            \
            message[ST_EV_LOG_MSG_SIZE - 1] = '\0';                          \
            logger->events.push(logger->events.ctx, logger->evtype_id_field, \
             message);                                                       \
        }                                                                    \
    }

ST_LOGGER_NOLOCK_FUNC(st_logger_debug_nolock, logger_fallback_debug, sir_debug, // NOLINT(cert-err33-c)
 ST_LL_DEBUG, ev_log_output_debug);
ST_LOGGER_NOLOCK_FUNC(st_logger_info_nolock, logger_fallback_info, sir_info, // NOLINT(cert-err33-c)
 ST_LL_INFO, ev_log_output_info);
ST_LOGGER_NOLOCK_FUNC(st_logger_warning_nolock, logger_fallback_warning, // NOLINT(cert-err33-c)
 sir_warn, ST_LL_WARNING, ev_log_output_warning);
ST_LOGGER_NOLOCK_FUNC(st_logger_error_nolock, logger_fallback_error, sir_error, // NOLINT(cert-err33-c)
 ST_LL_ERROR, ev_log_output_warning);

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

    if (module->use_fallback_module) {
        module->logger_fallback_set_postmortem_msg(module->logger_fallback_ctx,
         msg);

        return;
    }

    snprintf(module->postmortem_msg, ST_POSTMORTEM_MSG_SIZE_MAX, "%s", msg);
}
