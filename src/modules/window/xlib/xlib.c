#include "xlib.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define ATOM_BITS        32

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(window_xlib)
ST_MODULE_DEF_INIT_FUNC(window_xlib)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
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

    if (!st_window_import_functions(window_ctx, events_ctx, logger_ctx))
        goto fail;

    module->windows = st_slist_create(sizeof(st_window_t));
    if (!module->windows) {
        module->logger.error(module->logger.ctx,
         "window_xlib: Unable to create list for windows entries");

        goto fail;
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

    module->logger.info(module->logger.ctx,
     "window_xlib: Monitors mgr initialized");

    return window_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, window_ctx);

    return NULL;
}

static void st_window_quit(st_modctx_t *window_ctx) {
    st_window_xlib_t *module = window_ctx->data;

    while (!st_slist_empty(module->windows)) {
        st_slnode_t *node = st_slist_get_first(module->windows);
        st_window_t *window = st_slist_get_data(node);

        st_slist_remove_head(module->windows); // NOLINT(altera-unroll-loops)
        XDestroyWindow(window->monitor->handle, window->handle);
        free(window);
    }

    st_slist_destroy(module->windows);
    module->windows = NULL;

    module->logger.info(module->logger.ctx,
     "monitor_xlib: Monitors mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, window_ctx);
}

void fullscreen_window(Display* display, Window window) {
    XWindowAttributes attrs;
    XEvent            event = {0};
    XGetWindowAttributes(display, window, &attrs);


    event.xclient.type = ClientMessage;
    event.xclient.message_type = XInternAtom(display, "_NET_WM_STATE", false);
    event.xclient.display = display;
    event.xclient.window = window;
    event.xclient.format = ATOM_BITS;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = (long)XInternAtom(display,
     "_NET_WM_STATE_FULLSCREEN", false);
    XSendEvent(display, attrs.root, false,
     SubstructureNotifyMask | SubstructureRedirectMask, &event); // NOLINT(hicpp-signed-bitwise)
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
    st_window_t         *window = malloc(sizeof(st_window_t));

    if (!window) {
        module->logger.error(module->logger.ctx,
         "window_xlib: Unable to allocate memory for window structure: %s",
         strerror(errno));

        return NULL;
    }

    window->handle = XCreateWindow(monitor->handle, monitor->root_window,
     x, y, width, height, 0, CopyFromParent, InputOutput, CopyFromParent,
     CWEventMask, &event_attrs); // NOLINT(hicpp-signed-bitwise)
    XChangeWindowAttributes(monitor->handle, window->handle,
     CWOverrideRedirect, &override_redirect_attrs);  // NOLINT(hicpp-signed-bitwise)

    window->wm_delete_msg = XInternAtom(monitor->handle, "WM_DELETE_WINDOW",
     False);
    XSetWMProtocols(monitor->handle, window->handle, &window->wm_delete_msg, 1);
    window->xed = false;

    XSetWMHints(monitor->handle, window->handle, &hints);
    XMapWindow(monitor->handle, window->handle);
    XStoreName(monitor->handle, window->handle, title);

    if (fullscreen) {
        fullscreen_window(monitor->handle, window->handle);
    } else {
        XChangeProperty(monitor->handle, window->handle,
         XInternAtom(monitor->handle, "_HILDON_NON_COMPOSITED_WINDOW", False),
         XA_INTEGER, ATOM_BITS, PropModeReplace, (unsigned char*)(int[]){1}, 1);
    }

    window->monitor = monitor;
    if (!st_slist_insert_head(module->windows, window)) {
        module->logger.error(module->logger.ctx,
         "window_xlib: Unable to create list entry for window");
        XDestroyWindow(window->monitor->handle, window->handle);
        free(window);

        return NULL;
    }

    return window;
}

static void st_window_destroy(st_window_t *window) {
    st_window_xlib_t *module = window->module;

    while (!st_slist_empty(module->windows)) {
        st_slnode_t *node = st_slist_get_first(module->windows);

        if (st_slist_get_data(node) == window) {
            st_slist_remove_head(module->windows); // NOLINT(altera-unroll-loops)
            XDestroyWindow(window->monitor->handle, window->handle);
            free(window);

            break;
        }
    }
}

static st_window_t *get_window_by_xwindow(st_modctx_t *window_ctx,
 Window xwindow) {
    st_window_xlib_t *module = window_ctx->data;
    st_slnode_t      *node = st_slist_get_first(module->windows);

    while (node) {
        st_window_t *window = st_slist_get_data(node);

        if (window->handle == xwindow)
            return window;

        node = st_slist_get_next(node);
    }

    return NULL;
}

static void st_window_process(st_modctx_t *window_ctx) {
    st_window_xlib_t *module = window_ctx->data;
    st_slnode_t      *node = st_slist_get_first(module->windows);

    while (node) {
        st_window_t *window = st_slist_get_data(node);

        while (XPending(window->monitor->handle)) {
            XEvent xevent;

            XNextEvent(window->monitor->handle, &xevent);
            switch (xevent.type) {
                case ClientMessage: {
                    st_window_t *event_window = get_window_by_xwindow(
                     window_ctx, xevent.xclient.window);

                    if (xevent.xclient.data.l[0] ==
                     (long)event_window->wm_delete_msg)
                        event_window->xed = true;

                    break;
                }
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

static bool st_window_xed(const st_window_t *window) {
    return window->xed;
}

static st_monitor_t *st_window_get_monitor(st_window_t *window) {
    return window->monitor;
}

static void *st_window_get_handle(st_window_t *window) {
    return (void *)(uintptr_t)window->handle;
}