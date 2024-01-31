#include "mouse.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define CTX_METATABLE_NAME "mouse_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;

static st_mouse_init_t                 st_mouse_init;
static st_mouse_quit_t                 st_mouse_quit;
static st_mouse_process_t              st_mouse_process;
static st_mouse_press_t                st_mouse_press;
static st_mouse_release_t              st_mouse_release;
static st_mouse_pressed_t              st_mouse_pressed;
static st_mouse_get_wheel_relative_t   st_mouse_get_wheel_relative;
static st_mouse_moved_t                st_mouse_moved;
static st_mouse_entered_t              st_mouse_entered;
static st_mouse_leaved_t               st_mouse_leaved;
static st_mouse_get_x_t                st_mouse_get_x;
static st_mouse_get_y_t                st_mouse_get_y;
static st_mouse_get_window_t           st_mouse_get_window;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_integer_to_field_t   st_lua_set_integer_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_get_integer_t            st_lua_get_integer;
static st_lua_push_nil_t               st_lua_push_nil;
static st_lua_push_bool_t              st_lua_push_bool;
static st_lua_push_integer_t           st_lua_push_integer;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_mouse)
ST_MODULE_DEF_INIT_FUNC(luabind_mouse)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_mouse_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_mouse_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_mouse: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_mouse", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_mouse", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, process);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, press);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, release);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, pressed);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, get_wheel_relative);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, moved);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, entered);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, leaved);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, get_x);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, get_y);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", mouse, get_window);

    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, push_nil);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, push_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_mouse", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t        *luabind_ctx;
    st_luabind_mouse_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_mouse_data, sizeof(st_luabind_mouse_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_mouse_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_mouse: Mouse funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_mouse_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_mouse: Mouse funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_mouse_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *events_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "events_ctx");
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "logger_ctx");

    *(st_modctx_t **)userdata = st_mouse_init(events_ctx, logger_ctx);
    st_lua_set_metatable(lua_state, CTX_METATABLE_NAME);

    return 1;
}

static int st_mouse_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_mouse_quit(mouse_ctx);

    return 0;
}

static int st_mouse_process_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_mouse_process(mouse_ctx);

    return 0;
}

static int st_mouse_press_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    button = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_mouse_press(mouse_ctx,
     (st_mbutton_t)button));

    return 1;
}

static int st_mouse_release_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    button = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_mouse_release(mouse_ctx,
     (st_mbutton_t)button));

    return 1;
}

static int st_mouse_pressed_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    button = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_mouse_pressed(mouse_ctx,
     (st_mbutton_t)button));

    return 1;
}

static int st_mouse_get_wheel_relative_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_integer(lua_state, st_mouse_get_wheel_relative(mouse_ctx));

    return 1;
}

static int st_mouse_moved_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_bool(lua_state, st_mouse_moved(mouse_ctx));

    return 1;
}

static int st_mouse_entered_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_bool(lua_state, st_mouse_entered(mouse_ctx));

    return 1;
}

static int st_mouse_leaved_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_bool(lua_state, st_mouse_leaved(mouse_ctx));

    return 1;
}

static int st_mouse_get_window_bind(st_luastate_t *lua_state) {
    st_modctx_t       *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    const st_window_t *window = st_mouse_get_window(mouse_ctx);

    if (window) {
        void *userdata = st_lua_create_userdata(lua_state,
         sizeof(const st_window_t *));

        *(const st_window_t **)userdata = window;
        st_lua_set_metatable(lua_state, "window");
    } else {
        st_lua_push_nil(lua_state);
    }

    return 1;
}

static int st_mouse_get_x_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_integer(lua_state, st_mouse_get_x(mouse_ctx));

    return 1;
}

static int st_mouse_get_y_bind(st_luastate_t *lua_state) {
    st_modctx_t *mouse_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_lua_push_integer(lua_state, st_mouse_get_y(mouse_ctx));

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_mouse_t *module = luabind_ctx->data;
    st_luastate_t      *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Mouse");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_mouse_init_bind);
    st_lua_set_integer_to_field(lua_state, "mb_left", ST_MB_LEFT);
    st_lua_set_integer_to_field(lua_state, "mb_middle", ST_MB_MIDDLE);
    st_lua_set_integer_to_field(lua_state, "mb_right", ST_MB_RIGHT);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, CTX_METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_mouse_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_mouse_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "process", st_mouse_process_bind);
    st_lua_set_cfunction_to_field(lua_state, "press", st_mouse_press_bind);
    st_lua_set_cfunction_to_field(lua_state, "release", st_mouse_release_bind);
    st_lua_set_cfunction_to_field(lua_state, "pressed", st_mouse_pressed_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_wheel_relative",
     st_mouse_get_wheel_relative_bind);
    st_lua_set_cfunction_to_field(lua_state, "moved", st_mouse_moved_bind);
    st_lua_set_cfunction_to_field(lua_state, "entered", st_mouse_entered_bind);
    st_lua_set_cfunction_to_field(lua_state, "leaved", st_mouse_leaved_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_x", st_mouse_get_x_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_y", st_mouse_get_y_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_window",
     st_mouse_get_window_bind);

    st_lua_pop(lua_state, 1);
}
