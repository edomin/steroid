#include "xlib.h"

#include <sys/queue.h>

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE      1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_window_xlib_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_window_xlib_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_window_xlib_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"window_xlib\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_window_xlib_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_window_xlib_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_window_import_functions(st_modctx_t *window_ctx,
 st_modctx_t *events_ctx, st_modctx_t *logger_ctx) {
    st_window_xlib_t *module = window_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "window_xlib: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("window_xlib", events, register_type);
    ST_LOAD_FUNCTION_FROM_CTX("window_xlib", events, push);

    ST_LOAD_FUNCTION_FROM_CTX("window_xlib", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("window_xlib", logger, info);

    return true;
}

static st_modctx_t *st_window_init(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *monitor_ctx) {
    st_modctx_t      *window_ctx;
    st_window_xlib_t *module;

    window_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_window_xlib_data, sizeof(st_window_xlib_t));

    if (!window_ctx)
        return NULL;

    window_ctx->funcs = &st_window_xlib_funcs;

    module = window_ctx->data;
    module->events.ctx = events_ctx;
    module->logger.ctx = logger_ctx;
    module->monitor.ctx = monitor_ctx;

    if (!st_window_import_functions(window_ctx, events_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, window_ctx);

        return NULL;
    }

    module->evtypes[EV_FOCUS_IN] = module->events.register_type(events_ctx,
     "window_focus_in", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_FOCUS_OUT] = module->events.register_type(events_ctx,
     "window_focus_in", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_RESIZE] = module->events.register_type(events_ctx,
     "window_resize", sizeof(st_evwinresize_t));
    module->evtypes[EV_PLACE_ON_TOP] = module->events.register_type(events_ctx,
     "window_place_on_top", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_PLACE_ON_BOTTOM] = module->events.register_type(
     events_ctx, "window_place_on_bottom", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_CREATE] = module->events.register_type(events_ctx,
     "window_create", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_DESTROY] = module->events.register_type(events_ctx,
     "window_destroy", sizeof(st_evwinnoargs_t));
    // module->evtypes[EV_MOVE] = module->events.register_type(events_ctx,
    //  "window_move", sizeof(st_evwinmove_t));
    module->evtypes[EV_SHOW] = module->events.register_type(events_ctx,
     "window_show", sizeof(st_evwinnoargs_t));
    module->evtypes[EV_HIDE] = module->events.register_type(events_ctx,
     "window_hide", sizeof(st_evwinnoargs_t));

    SLIST_INIT(&module->windows); // NOLINT(altera-unroll-loops)

    module->logger.info(module->logger.ctx,
     "window_xlib: Monitors mgr initialized");

    return window_ctx;
}

static void st_window_quit(st_modctx_t *window_ctx) {
    st_window_xlib_t *module = window_ctx->data;

    while (!SLIST_EMPTY(&module->windows)) {
        st_snode_t  *node = SLIST_FIRST(&module->windows);
        st_window_t *window = node->data;

        SLIST_REMOVE_HEAD(&module->windows, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
        XDestroyWindow(window->display, window->handle);
        free(window);
        free(node);
    }

    module->logger.info(module->logger.ctx,
     "monitor_xlib: Monitors mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, window_ctx);
}

static st_window_t *st_window_create(st_modctx_t *window_ctx,
 st_monitor_t *monitor, int x, int y, unsigned width, unsigned height,
 bool fullscreen, const char *title) {
    st_window_xlib_t    *module = window_ctx->data;
    XSetWindowAttributes event_attrs = {
        .event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | // NOLINT(hicpp-signed-bitwise)
         ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | // NOLINT(hicpp-signed-bitwise)
         PointerMotionMask | ExposureMask | StructureNotifyMask | // NOLINT(hicpp-signed-bitwise)
         ResizeRedirectMask | FocusChangeMask, // NOLINT(hicpp-signed-bitwise)
    };
    XSetWindowAttributes override_redirect_attrs = {
        .override_redirect = False,
    };
    XWMHints             hints = { .input = True, .flags = InputHint }; // NOLINT(hicpp-signed-bitwise)
    Atom                 wm_state;
    Atom                 wm_fullscreen;
    XEvent               ev_mask = {0};
    st_snode_t          *node;
    st_window_t         *window = malloc(sizeof(st_window_t));

    if (!window) {
        module->logger.error(module->logger.ctx,
         "window_xlib: Unable to allocate memory for window structure: %s",
         strerror(errno));

        return NULL;
    }

    node = malloc(sizeof(st_snode_t));
    if (!node) {
        module->logger.error(module->logger.ctx,
         "Error occured while allocating memory for window entry: %s",
         strerror(errno));
        free(window);

        return NULL;
    }

    window->handle = XCreateWindow(monitor->display, monitor->root_window,
     x, y, width, height, 0, CopyFromParent, InputOutput, CopyFromParent,
     CWEventMask, &event_attrs); // NOLINT(hicpp-signed-bitwise)
    XChangeWindowAttributes(monitor->display, window->handle,
     CWOverrideRedirect, &override_redirect_attrs);  // NOLINT(hicpp-signed-bitwise)

    XChangeProperty(monitor->display, window->handle,
     XInternAtom(monitor->display, "_HILDON_NON_COMPOSITED_WINDOW", False),
     XA_INTEGER, 32, PropModeReplace, (unsigned char*)(int[]){1}, 1); // NOLINT(readability-magic-numbers)

    XSetWMHints(monitor->display, window->handle, &hints);
    XMapWindow(monitor->display, window->handle);
    XStoreName(monitor->display, window->handle, title);

    wm_state = XInternAtom(monitor->display, "_NET_WM_STATE", False);
    wm_fullscreen = XInternAtom(monitor->display, "_NET_WM_STATE_FULLSCREEN",
     False);

    ev_mask.type                 = ClientMessage;
    ev_mask.xclient.window       = window->handle;
    ev_mask.xclient.message_type = wm_state;
    ev_mask.xclient.format       = 32; // NOLINT(readability-magic-numbers)
    ev_mask.xclient.data.l[0]    = 1;
    ev_mask.xclient.data.l[1]    = (long)wm_fullscreen;

    XSendEvent(monitor->display, DefaultRootWindow(monitor->display), False,
     SubstructureNotifyMask, &ev_mask);  // NOLINT(hicpp-signed-bitwise)

    window->display = monitor->display;
    node->data = window;
    SLIST_INSERT_HEAD(&module->windows, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)

    return window;
}

static void st_window_destroy(st_window_t *window) {
    st_window_xlib_t *module = window->module;

    while (!SLIST_EMPTY(&module->windows)) {
        st_snode_t *node = SLIST_FIRST(&module->windows);

        if (node->data == window) {
            SLIST_REMOVE_HEAD(&module->windows, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
            XDestroyWindow(window->display, window->handle);
            free(window);
            free(node);

            break;
        }
    }
}

static st_window_t *get_window_by_xwindow(st_modctx_t *window_ctx,
 Window xwindow) {
    st_window_xlib_t *module = window_ctx->data;
    st_snode_t       *node;

    SLIST_FOREACH(node, &module->windows, ST_SNODE_NEXT) {
        st_window_t *window = node->data;

        if (window->handle == xwindow)
            return window;
    }

    return NULL;
}

static void st_window_process(st_modctx_t *window_ctx) {
    st_window_xlib_t *module = window_ctx->data;
    st_snode_t       *node;

    SLIST_FOREACH(node, &module->windows, ST_SNODE_NEXT) {
        st_window_t *window = node->data;

        while (XPending(window->display)) {
            XEvent xevent;

            XNextEvent(window->display, &xevent);
            switch (xevent.type) {
                case ButtonPress:
                    /* TODO(edomin): Mouse button event structure required */
                    break;
                case ButtonRelease:
                    /* TODO(edomin): Mouse button event structure required */
                    break;
                case MotionNotify:
                    /* TODO(edomin): Mouse motion event structure required */
                    break;
                case EnterNotify:
                    /* TODO(edomin): Mouse enter event structure required */
                    break;
                case LeaveNotify:
                    /* TODO(edomin): Mouse leave event structure required */
                    break;
                case KeyPress:
                    /* TODO(edomin): Keyboard event structure required */
                    break;
                case KeyRelease:
                    /* TODO(edomin): Keyboard event structure required */
                    break;
                case FocusIn: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xfocus.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_FOCUS_IN], &event);
                    break;
                }
                case FocusOut: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xfocus.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_FOCUS_OUT], &event);
                    break;
                }
                case ResizeRequest: {
                    st_evwinresize_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xresizerequest.window),
                        .width = (unsigned)xevent.xresizerequest.width,
                        .height = (unsigned)xevent.xresizerequest.height,
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_RESIZE], &event);
                    break;
                }
                case CirculateNotify: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xcirculate.window),
                    };
                    if (xevent.xcirculate.place == PlaceOnTop)
                        module->events.push(module->events.ctx,
                         module->evtypes[EV_PLACE_ON_TOP], &event);
                    else
                        module->events.push(module->events.ctx,
                         module->evtypes[EV_PLACE_ON_BOTTOM], &event);
                    break;
                }
                case CreateNotify: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xcreatewindow.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_CREATE], &event);
                    break;
                }
                case DestroyNotify: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xdestroywindow.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_DESTROY], &event);
                    break;
                }
                case MapNotify: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xmap.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_SHOW], &event);
                    break;
                }
                case UnmapNotify: {
                    st_evwinnoargs_t event = {
                        .window = get_window_by_xwindow(window_ctx,
                         xevent.xunmap.window),
                    };
                    module->events.push(module->events.ctx,
                     module->evtypes[EV_HIDE], &event);
                    break;
                }
                case ConfigureNotify:
                case GravityNotify:
                default:
                    break;
            }
        }
    }
}