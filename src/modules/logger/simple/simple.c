#include "simple.h"

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <syslog.h>

#define ERRMSGBUF_SIZE 1024
#define CBK_BUF_SIZE   4096

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(logger_simple)
ST_MODULE_DEF_INIT_FUNC(logger_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_logger_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static void log_file_destroy(void *plog_file) {
    st_logger_simple_log_file_t *log_file = plog_file;

    fflush(log_file->file);
    fclose(log_file->file);
}

static st_modctx_t *st_logger_init(st_modctx_t *events_ctx) {
    st_modctx_t        *logger_ctx = global_modsmgr_funcs.init_module_ctx(
     global_modsmgr, &st_module_logger_simple_data, sizeof(st_logger_simple_t));
    st_logger_simple_t *module;

    if (!logger_ctx)
        return NULL;

    logger_ctx->funcs = &st_logger_simple_funcs;

    module = logger_ctx->data;
    module->events.ctx = events_ctx;
    module->stdout_levels = ST_LL_NONE;
    module->stderr_levels = ST_LL_ALL;
    module->syslog_levels = ST_LL_NONE;
    module->callbacks_count = 0;
    module->log_files = st_dlist_create(sizeof(st_logger_simple_log_file_t),
     log_file_destroy);
    if (!module->log_files)
        st_logger_warning(logger_ctx, "logger_simple: Unable to initialize "
         "dlist for opened files. Logging to file is not available on this run"
        );

    if (events_ctx && !st_logger_enable_events(logger_ctx, events_ctx))
        module->events.ctx = NULL;

    st_logger_info(logger_ctx, "%s", "logger_simple: Logger initialized");

    return logger_ctx;
}

static void st_logger_quit(st_modctx_t *logger_ctx) {
    st_logger_simple_t *module = logger_ctx->data; // NOLINT(altera-id-dependent-backward-branch)

    st_logger_info(logger_ctx, "%s", "logger_simple: Destroying logger");

    if (module->syslog_levels != ST_LL_NONE)
        closelog();

    if (module->log_files)
        st_dlist_destroy(module->log_files);

    flockfile(stdout);
        printf("%s", module->postmortem_msg);
    funlockfile(stdout);

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
    st_logger_simple_t *module;
    st_dlnode_t        *node;

    if (!logger_ctx || !filename)
        return false;

    module = logger_ctx->data;

    if (!module->log_files)
        return false;

    node = st_dlist_get_head(module->log_files);

    while (node) {
        st_logger_simple_log_file_t *log_file = st_dlist_get_data(node);

        if (strncmp(log_file->filename, filename, PATH_MAX) == 0) {
            if (levels == ST_LL_NONE)
                st_dlist_remove(node);
            else
                log_file->log_levels = levels;

            return true;
        }

        node = st_dlist_get_next(node);
    }

    if (levels != ST_LL_NONE) {
        int                         ret;
        st_logger_simple_log_file_t log_file = {
            .file = NULL,
            .log_levels = levels,
        };

        ret = snprintf(log_file.filename, PATH_MAX, "%s", filename);
        if (ret < 0 || ret == PATH_MAX) {
            st_logger_error(logger_ctx, "%s",
             "logger_simple: Unable to set log filename");

            return false;
        }

        log_file.file = fopen(filename, "wbe");
        if (!log_file.file) {
            char errbuf[ERRMSGBUF_SIZE];

            if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
                st_logger_error(logger_ctx,
                 "logger_simple: Unable to open file \"%s\": %s", filename,
                 errbuf);
            else
                st_logger_error(logger_ctx,
                 "logger_simple: Unable to open file \"%s\"", filename);

            return false;
        }

        if (!!st_dlist_push_back(module->log_files, &log_file)) {
            fclose(log_file.file);

            st_logger_error(logger_ctx,
             "logger_simple: Unable to add \"%s\" log file entry to the list",
             filename);

            return false;
        }
    }

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
    st_logger_simple_t *module = logger_ctx->data;
    char                buffer[CBK_BUF_SIZE];

    if ((module->stdout_levels & log_level) == log_level) {
        flockfile(stdout);
            if (vprintf(format, args) > 0)
                putc('\n', stdout);
        funlockfile(stdout);
    }
    if ((module->stderr_levels & log_level) == log_level) {
        flockfile(stderr);
            if (vfprintf(stderr, format, args) > 0)
                putc('\n', stderr);
        funlockfile(stderr);
    }
    if ((module->syslog_levels & log_level) == log_level)
        vsyslog(st_logger_level_to_syslog_priority(log_level), format, args);

    if (module->log_files) {
        st_dlnode_t *node = st_dlist_get_head(module->log_files);

        while (node) {
            st_logger_simple_log_file_t *log_file = st_dlist_get_data(node);

            if ((log_file->log_levels & log_level) == log_level) {
                flockfile(log_file->file);
                    if (vfprintf(log_file->file, format, args) > 0)
                        fflush(log_file->file);
                funlockfile(log_file->file);
            }

            node = st_dlist_get_next(node);
        }
    }

    if (vsnprintf(buffer, CBK_BUF_SIZE, format, args) > 0) {
        for (unsigned i = 0; i < module->callbacks_count; i++) { // NOLINT(altera-id-dependent-backward-branch)
            if ((module->callbacks[i].log_levels & log_level) == log_level) {
                module->callbacks[i].func(buffer,
                 module->callbacks[i].userdata);
            }
        }
    }

    if (module->events.ctx) {
        buffer[ST_EV_LOG_MSG_SIZE - 1] = '\0';
        module->events.push(module->events.ctx, evtype_id, buffer);
    }
}

#define ST_LOGGER_SIMPLE_VFUNC(st_func, level, evtype_id_field)               \
    static __attribute__((format (printf, 2, 0))) void st_func(               \
     const st_modctx_t *logger_ctx, const char* format, va_list args) {       \
        st_logger_simple_t *module = logger_ctx->data;                        \
        st_logger_general(logger_ctx, level, module->evtype_id_field, format, \
         args);                                                               \
    }

ST_LOGGER_SIMPLE_VFUNC(st_logger_debug_nolock, ST_LL_DEBUG,
 ev_log_output_debug);
ST_LOGGER_SIMPLE_VFUNC(st_logger_info_nolock, ST_LL_INFO, ev_log_output_info);
ST_LOGGER_SIMPLE_VFUNC(st_logger_warning_nolock, ST_LL_WARNING,
 ev_log_output_warning);
ST_LOGGER_SIMPLE_VFUNC(st_logger_error_nolock, ST_LL_ERROR,
 ev_log_output_error);

#define ST_LOGGER_SIMPLE_LOG_FUNC(st_func, st_vfunc) \
    static __attribute__((format (printf, 2, 3))) void st_func( \
     const st_modctx_t *logger_ctx, const char* format, ...) {  \
        st_logger_simple_t *module = logger_ctx->data;          \
        va_list             args;                               \
        va_start(args, format);                                 \
            st_vfunc(logger_ctx, format, args);                 \
        va_end(args);                                           \
    }

ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_debug    , st_logger_debug_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_info     , st_logger_info_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_warning  , st_logger_warning_nolock);
ST_LOGGER_SIMPLE_LOG_FUNC(st_logger_error    , st_logger_error_nolock);

static void st_logger_set_postmortem_msg(st_modctx_t *logger_ctx,
 const char *msg) {
    st_logger_simple_t *module = logger_ctx->data;

    snprintf(module->postmortem_msg, ST_POSTMORTEM_MSG_SIZE_MAX, "%s", msg);
}
