#include "xlib.h"

#include <X11/Xlib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE      1024
#define DISPLAY_NAME_SIZE_MAX 128

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_monitor_xlib_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_monitor_xlib_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_monitor_xlib_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"monitor_xlib\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_monitor_xlib_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
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

    ST_LOAD_FUNCTION_FROM_CTX("monitor_xlib", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("monitor_xlib", logger, info);

    return true;
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
        return NULL;

    module->logger.info(module->logger.ctx,
     "monitor_xlib: Monitors mgr initialized.");

    return monitor_ctx;
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
    Display           *display;
    st_monitor_t      *monitor;

    if (snprintf_s(display_name, DISPLAY_NAME_SIZE_MAX, ":0.%i",
     index) < 0) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to construct display name: %s", strerror(errno));

        return NULL;
    }

    monitor = malloc(sizeof(st_monitor_t));
    if (!monitor) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to allocate memory for monitor structure: %s",
         strerror(errno));

        return NULL;
    }

    monitor->display = XOpenDisplay(display_name);

    if (!monitor->display) {
        module->logger.error(module->logger.ctx,
         "monitor_xlib: Unable to open display");
        free(monitor);

        return NULL;
    }

    monitor->root_window = DefaultRootWindow(monitor->display);
    monitor->index = index;

    return monitor;
}

static void st_monitor_release(st_monitor_t *monitor) {
    XCloseDisplay(monitor->display);
    free(monitor);
}

static unsigned st_monitor_get_width(st_monitor_t *monitor) {
    int width = XDisplayWidth(monitor->display, (int)monitor->index);

    return width > 0 ? (unsigned)width : 0u;
}

static unsigned st_monitor_get_height(st_monitor_t *monitor) {
    int height = XDisplayHeight(monitor->display, (int)monitor->index);

    return height > 0 ? (unsigned)height : 0u;
}

