#include "xlib.h"

#include <errno.h>
#include <stdio.h>

#include <X11/Xlib.h>

#define ERRMSGBUF_SIZE        128
#define DISPLAY_NAME_SIZE_MAX 128

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

static st_monitor_funcs_t monitor_funcs = {
    .release      = st_monitor_release,
    .get_width    = st_monitor_get_width,
    .get_height   = st_monitor_get_height,
    .get_handle   = st_monitor_get_handle,
    .set_userdata = st_monitor_set_userdata,
    .get_userdata = st_monitor_get_userdata,
};

ST_MODULE_DEF_GET_FUNC(monitor_xlib)
ST_MODULE_DEF_INIT_FUNC(monitor_xlib)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_monitor_xlib_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_monitor_import_functions(st_modctx_t *monitor_ctx,
 st_modctx_t *logger_ctx) {
    st_monitor_xlib_t *module = monitor_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "monitor_xlib: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("monitor_xlib", fnv1a, NULL, get_u32hashstr_func);

    ST_LOAD_FUNCTION("monitor_xlib", htable, NULL, create);
    ST_LOAD_FUNCTION("monitor_xlib", htable, NULL, init);
    ST_LOAD_FUNCTION("monitor_xlib", htable, NULL, quit);

    ST_LOAD_FUNCTION_FROM_CTX("monitor_xlib", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("monitor_xlib", logger, info);

    return true;
}

static bool st_keyeqfunc(const void *left, const void *right) {
    return left && right && strcmp(left, right) == 0;
}

static st_modctx_t *st_monitor_init(st_modctx_t *logger_ctx) {
    st_modctx_t       *monitor_ctx;
    st_monitor_xlib_t *module;

    monitor_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_monitor_xlib_data, sizeof(st_monitor_xlib_t));

    if (!monitor_ctx)
        return NULL;

    monitor_ctx->funcs = &st_monitor_xlib_funcs;

    module = monitor_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_monitor_import_functions(monitor_ctx, logger_ctx))
        goto import_fail;

    module->htable.ctx = module->htable.init(module->logger.ctx);
    if (!module->htable.ctx)
        goto ht_ctx_init_fail;

    module->logger.info(module->logger.ctx,
     "monitor_xlib: Monitors mgr initialized.");

    return monitor_ctx;

ht_ctx_init_fail:
import_fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, monitor_ctx);

    return NULL;
}

static void st_monitor_quit(st_modctx_t *monitor_ctx) {
    st_monitor_xlib_t *module = monitor_ctx->data;

    module->logger.info(module->logger.ctx,
     "monitor_xlib: Monitors mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, monitor_ctx);
}

static unsigned st_monitor_get_monitors_count(st_modctx_t *monitor_ctx) {
    st_monitor_xlib_t *module = monitor_ctx->data;
    unsigned           monitors_count;
    Display           *display = XOpenDisplay(NULL);

    if (!display) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to open default display");

        return 0;
    }

    monitors_count = (unsigned)ScreenCount(display);
    XCloseDisplay(display);
    return monitors_count;
}

static st_monitor_t *st_monitor_open(st_modctx_t *monitor_ctx, unsigned index) {
    st_monitor_xlib_t *module = monitor_ctx->data;
    char               display_name[DISPLAY_NAME_SIZE_MAX];
    st_monitor_t      *monitor;
    Window             root_window;
    int                ret = snprintf(display_name, DISPLAY_NAME_SIZE_MAX,
     ":0.%u", index);

    if (ret < 0 || ret == DISPLAY_NAME_SIZE_MAX) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to construct display name for display with "
         "index %u", index);

        return NULL;
    }

    monitor = malloc(sizeof(st_monitor_t));
    if (!monitor) {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "monitor_xlib: Unable to allocate memory for monitor structure: "
             "%s", errbuf);

        return NULL;
    }

    monitor->handle = XOpenDisplay(display_name);

    if (!monitor->handle) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to open display");
        free(monitor);

        return NULL;
    }


    root_window = DefaultRootWindow(monitor->handle);

    monitor->index = index;
    st_object_make(monitor, monitor_ctx, &monitor_funcs);

    monitor->userdata = module->htable.create(module->htable.ctx,
     (unsigned int (*)(const void *))module->fnv1a.get_u32hashstr_func(NULL),
     st_keyeqfunc, free, NULL);
    if (!monitor->userdata) {
        free(monitor);

        return NULL;
    }
    st_monitor_set_userdata(monitor, "root_window", root_window);

    return monitor;
}

static void st_monitor_release(st_monitor_t *monitor) {
    XCloseDisplay(monitor->handle);
    free(monitor);
}

static unsigned st_monitor_get_width(st_monitor_t *monitor) {
    int width = XDisplayWidth(monitor->handle, (int)monitor->index);

    return width > 0 ? (unsigned)width : 0u;
}

static unsigned st_monitor_get_height(st_monitor_t *monitor) {
    int height = XDisplayHeight(monitor->handle, (int)monitor->index);

    return height > 0 ? (unsigned)height : 0u;
}

static void *st_monitor_get_handle(st_monitor_t *monitor) {
    return monitor->handle;
}

static void st_monitor_set_userdata(const st_monitor_t *monitor,
 const char *key, uintptr_t value) {
    st_monitor_xlib_t *module = ((st_modctx_t *)st_object_get_owner(monitor)
     )->data;
    char              *keydup = strdup(key);

    if (keydup) {
        ST_HTABLE_CALL(monitor->userdata, insert, NULL, keydup, (void *)value);
    } else {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "monitor_xlib: Unable to allocate memory for userdata of monitor "
             "\"%s\": %s", key, errbuf);
    }
}

static bool st_monitor_get_userdata(const st_monitor_t *monitor, uintptr_t *dst,
 const char *key) {
    st_monitor_xlib_t *module = ((st_modctx_t *)st_object_get_owner(monitor)
     )->data;
    st_htiter_t        it;
    void              *userdata;

    if (!ST_HTABLE_CALL(monitor->userdata, find, &it, key))
        return false;

    userdata = ST_HTITER_CALL(&it, get_value);
    *dst = (uintptr_t)userdata;

    return true;
}
