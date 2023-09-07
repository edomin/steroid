#include "simple.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define EVQ_POOL_SIZE    1024

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(mouse_simple)
ST_MODULE_DEF_INIT_FUNC(mouse_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_mouse_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_mouse_import_functions(st_modctx_t *mouse_ctx,
 st_modctx_t *events_ctx, st_modctx_t *logger_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "mouse_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, get_type_id);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, create_queue);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, destroy_queue);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, subscribe);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, is_empty);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, peek_type);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", events, pop);

    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("mouse_simple", logger, info);

    return true;
}

static st_modctx_t *st_mouse_init(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx) {
    st_modctx_t       *mouse_ctx;
    st_mouse_simple_t *module;

    mouse_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_mouse_simple_data, sizeof(st_mouse_simple_t));

    if (!mouse_ctx)
        return NULL;

    mouse_ctx->funcs = &st_mouse_simple_funcs;

    module = mouse_ctx->data;
    module->events.ctx = events_ctx;
    module->logger.ctx = logger_ctx;

    if (!st_mouse_import_functions(mouse_ctx, events_ctx, logger_ctx))
        goto fail;

    module->evtypes[EV_MOUSE_PRESS] = module->events.get_type_id(events_ctx,
     "window_mouse_press");
    module->evtypes[EV_MOUSE_RELEASE] = module->events.get_type_id(events_ctx,
     "window_mouse_release");
    module->evtypes[EV_MOUSE_WHEEL] = module->events.get_type_id(events_ctx,
     "window_mouse_wheel");
    module->evtypes[EV_MOUSE_MOVE] = module->events.get_type_id(events_ctx,
     "window_mouse_move");
    module->evtypes[EV_MOUSE_ENTER] = module->events.get_type_id(events_ctx,
     "window_mouse_enter");
    module->evtypes[EV_MOUSE_LEAVE] = module->events.get_type_id(events_ctx,
     "window_mouse_leave");

    module->evq = module->events.create_queue(events_ctx, EVQ_POOL_SIZE);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_PRESS]);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_RELEASE]);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_WHEEL]);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_MOVE]);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_ENTER]);
    module->events.subscribe(module->evq, module->evtypes[EV_MOUSE_LEAVE]);

    module->x = 0;
    module->y = 0;
    memset_s(module->prev_mbstate, sizeof(int) * ST_MB_MAX, 0,
     sizeof(int) * ST_MB_MAX);
    memset_s(module->curr_mbstate, sizeof(int) * ST_MB_MAX, 0,
     sizeof(int) * ST_MB_MAX);
    module->wheel = 0;
    module->move = NULL;
    module->enter = NULL;
    module->leave = NULL;
    module->current_window = NULL;

    module->logger.info(module->logger.ctx,
     "mouse_simple: Mouse initialized");

    return mouse_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, mouse_ctx);

    return NULL;
}

static void st_mouse_quit(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    module->events.destroy_queue(module->evq);

    module->logger.info(module->logger.ctx, "mouse_simple: Mouse destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, mouse_ctx);
}

static void st_mouse_process_press(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwinunsigned_t event;

    module->events.pop(module->evq, &event);

    module->curr_mbstate[event.value] = true;
}

static void st_mouse_process_release(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwinunsigned_t event;

    module->events.pop(module->evq, &event);

    module->curr_mbstate[event.value] = false;
}

static void st_mouse_process_wheel(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwininteger_t  event;

    module->events.pop(module->evq, &event);

    module->wheel = event.value;
}

static void st_mouse_process_move(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwinuvec2_t    event;

    module->events.pop(module->evq, &event);

    module->x = event.hvalue;
    module->y = event.vvalue;
    module->move = true;
    module->current_window = event.window;
}

static void st_mouse_process_enter(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwinuvec2_t    event;

    module->events.pop(module->evq, &event);

    module->enter = true;
    module->current_window = event.window;
}

static void st_mouse_process_leave(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;
    st_evwinuvec2_t    event;

    module->events.pop(module->evq, &event);

    module->leave = true;
    module->current_window = NULL;
}

static void (*procfuncs[])(st_modctx_t *mouse_ctx) = {
    [EV_MOUSE_PRESS] = st_mouse_process_press,
    [EV_MOUSE_RELEASE] = st_mouse_process_release,
    [EV_MOUSE_WHEEL] = st_mouse_process_wheel,
    [EV_MOUSE_MOVE] = st_mouse_process_move,
    [EV_MOUSE_ENTER] = st_mouse_process_enter,
    [EV_MOUSE_LEAVE] = st_mouse_process_leave,
};

static void st_mouse_process(st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    for (unsigned i = 0; i < ST_MB_MAX; i++)
        module->prev_mbstate[i] = module->curr_mbstate[i];

    module->wheel = 0;
    module->move = false;
    module->enter = false;
    module->leave = false;

    while (!module->events.is_empty(module->evq)) {
        st_evtypeid_t evtype = module->events.peek_type(module->evq);

        for (evtype_index_t evt = 0; evt < EV_MAX; evt++) {
            if (evtype == module->evtypes[evt]) {
                procfuncs[evt](mouse_ctx);

                break;
            }
        }
    }
}

static bool st_mouse_press(const st_modctx_t *mouse_ctx, st_mbutton_t button) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return !module->prev_mbstate[button] && module->curr_mbstate[button];
}

static bool st_mouse_release(const st_modctx_t *mouse_ctx,
 st_mbutton_t button) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->prev_mbstate[button] && !module->curr_mbstate[button];
}

static bool st_mouse_pressed(const st_modctx_t *mouse_ctx,
 st_mbutton_t button) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->curr_mbstate[button];
}

static int st_mouse_get_wheel_relative(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->wheel;
}

static bool st_mouse_moved(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->move;
}

static bool st_mouse_entered(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->enter;
}

static bool st_mouse_leaved(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->leave;
}

static unsigned st_mouse_get_x(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->x;
}

static unsigned st_mouse_get_y(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->y;
}

static const st_window_t *st_mouse_get_window(const st_modctx_t *mouse_ctx) {
    st_mouse_simple_t *module = mouse_ctx->data;

    return module->current_window;
}
