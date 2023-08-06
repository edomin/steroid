#include "timer.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "timer_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_timer_init_t                 st_timer_init;
static st_timer_quit_t                 st_timer_quit;
static st_timer_start_t                st_timer_start;
static st_timer_get_elapsed_t          st_timer_get_elapsed;
static st_timer_sleep_t                st_timer_sleep;
static st_timer_sleep_for_fps_t        st_timer_sleep_for_fps;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_push_integer_t           st_lua_push_integer;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_get_integer_t            st_lua_get_integer;
static st_lua_register_cfunction_t     st_lua_register_cfunction;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_timer)
ST_MODULE_DEF_INIT_FUNC(luabind_timer)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_timer_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_timer_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_timer: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_timer", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_timer", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, start);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, get_elapsed);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, sleep);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", timer, sleep_for_fps);

    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, push_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, register_cfunction);
    ST_LOAD_GLOBAL_FUNCTION("luabind_timer", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t        *luabind_ctx;
    st_luabind_timer_t *luabind;
    errno_t             err;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_timer_data, sizeof(st_luabind_timer_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_timer_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_timer: Timer funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_timer_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_timer: Timer funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_timer_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");

    *(st_modctx_t **)userdata = st_timer_init(logger_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_timer_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *timer_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_timer_quit(timer_ctx);

    return 0;
}

static int timer_start_bind(st_luastate_t *lua_state) {
    st_modctx_t *timer_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_lua_push_integer(lua_state, (ptrdiff_t)st_timer_start(timer_ctx));

    return 1;
}

static int timer_get_elapsed_bind(st_luastate_t *lua_state) {
    st_modctx_t *timer_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    start = st_lua_get_integer(lua_state, 2);

    st_lua_push_integer(lua_state,
     st_timer_get_elapsed(timer_ctx, (uint64_t)start));

    return 1;
}

static int timer_sleep_bind(st_luastate_t *lua_state) {
    st_modctx_t *timer_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    ms = st_lua_get_integer(lua_state, 2);

    st_timer_sleep(timer_ctx, (unsigned)ms);

    return 0;
}

static int timer_sleep_for_fps_bind(st_luastate_t *lua_state) {
    st_modctx_t *timer_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    fps = st_lua_get_integer(lua_state, 2);

    st_timer_sleep_for_fps(timer_ctx, (unsigned)fps);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_timer_t *module = luabind_ctx->data;
    st_luastate_t      *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Timer");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_timer_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_timer_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_timer_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "start", timer_start_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_elapsed",
     timer_get_elapsed_bind);
    st_lua_set_cfunction_to_field(lua_state, "sleep", timer_sleep_bind);
    st_lua_set_cfunction_to_field(lua_state, "sleep_for_fps",
     timer_sleep_for_fps_bind);

    st_lua_pop(lua_state, 1);
}
