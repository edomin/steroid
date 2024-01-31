#include "gfxctx.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE      1024
#define MODCTX_METATABLE_NAME "gfxctx_ctx"
#define GFXCTX_METATABLE_NAME "gfxctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_gfxctx_init_t                st_gfxctx_init;
static st_gfxctx_quit_t                st_gfxctx_quit;
static st_gfxctx_create_t              st_gfxctx_create;
static st_gfxctx_create_shared_t       st_gfxctx_create_shared;
static st_gfxctx_make_current_t        st_gfxctx_make_current;
static st_gfxctx_swap_buffers_t        st_gfxctx_swap_buffers;
static st_gfxctx_get_api_t             st_gfxctx_get_api;
static st_gfxctx_destroy_t             st_gfxctx_destroy;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_push_bool_t              st_lua_push_bool;
static st_lua_push_integer_t           st_lua_push_integer;
static st_lua_set_integer_to_field_t   st_lua_set_integer_to_field;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_integer_t            st_lua_get_integer;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_gfxctx)
ST_MODULE_DEF_INIT_FUNC(luabind_gfxctx)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_gfxctx_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_gfxctx_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_gfxctx: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_gfxctx", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_gfxctx", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, create);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, create_shared);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, make_current);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, swap_buffers);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, get_api);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", gfxctx, destroy);

    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, push_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_gfxctx", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t         *luabind_ctx;
    st_luabind_gfxctx_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_gfxctx_data, sizeof(st_luabind_gfxctx_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_gfxctx_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_gfxctx: Graphics context funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_gfxctx_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_gfxctx: Graphics context funcs binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_gfxctx_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");
    st_modctx_t *monitor_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "monitor_ctx");
    st_modctx_t *window_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 3, "window_ctx");

    *(st_modctx_t **)userdata = st_gfxctx_init(logger_ctx, monitor_ctx,
     window_ctx);
    st_lua_set_metatable(lua_state, MODCTX_METATABLE_NAME);

    return 1;
}

static int st_gfxctx_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *gfxctx_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, MODCTX_METATABLE_NAME);

    st_gfxctx_quit(gfxctx_ctx);

    return 0;
}

static int st_gfxctx_create_bind(st_luastate_t *lua_state) {
    st_modctx_t  *gfxctx_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, MODCTX_METATABLE_NAME);
    st_monitor_t *monitor = *(st_monitor_t **)st_lua_get_named_userdata(
     lua_state, 2, "monitor");
    st_window_t  *window = *(st_window_t **)st_lua_get_named_userdata(
     lua_state, 3, "window");
    ptrdiff_t     api = st_lua_get_integer(lua_state, 4);
    void         *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_gfxctx_t *));

    *(st_gfxctx_t **)userdata = st_gfxctx_create(gfxctx_ctx, monitor, window,
     (st_gapi_t)api);

    st_lua_set_metatable(lua_state, GFXCTX_METATABLE_NAME);

    return 1;
}

static int st_gfxctx_create_shared_bind(st_luastate_t *lua_state) {
    st_modctx_t  *gfxctx_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, MODCTX_METATABLE_NAME);
    st_monitor_t *monitor = *(st_monitor_t **)st_lua_get_named_userdata(
     lua_state, 2, "monitor");
    st_window_t  *window = *(st_window_t **)st_lua_get_named_userdata(
     lua_state, 3, "window");
    st_gfxctx_t  *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 4, GFXCTX_METATABLE_NAME);
    void         *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_gfxctx_t *));

    *(st_gfxctx_t **)userdata = st_gfxctx_create_shared(gfxctx_ctx, monitor,
     window, gfxctx);

    st_lua_set_metatable(lua_state, GFXCTX_METATABLE_NAME);

    return 1;
}

static int st_gfxctx_make_current_bind(st_luastate_t *lua_state) {
    st_gfxctx_t *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 1, GFXCTX_METATABLE_NAME);

    st_lua_push_bool(lua_state, st_gfxctx_make_current(gfxctx));

    return 1;
}

static int st_gfxctx_swap_buffers_bind(st_luastate_t *lua_state) {
    st_gfxctx_t *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 1, GFXCTX_METATABLE_NAME);

    st_lua_push_bool(lua_state, st_gfxctx_swap_buffers(gfxctx));

    return 1;
}

static int st_gfxctx_get_api_bind(st_luastate_t *lua_state) {
    st_gfxctx_t *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 1, GFXCTX_METATABLE_NAME);

    st_lua_push_integer(lua_state, st_gfxctx_get_api(gfxctx));

    return 1;
}

static int st_gfxctx_destroy_bind(st_luastate_t *lua_state) {
    st_gfxctx_t *gfxctx = *(st_gfxctx_t **)st_lua_get_named_userdata(
     lua_state, 1, GFXCTX_METATABLE_NAME);

    st_gfxctx_destroy(gfxctx);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_gfxctx_t *module = luabind_ctx->data;
    st_luastate_t       *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "GfxCtx");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_gfxctx_init_bind);
    st_lua_set_integer_to_field(lua_state, "gapi_gl11", ST_GAPI_GL11);
    st_lua_set_integer_to_field(lua_state, "gapi_gl12", ST_GAPI_GL12);
    st_lua_set_integer_to_field(lua_state, "gapi_gl13", ST_GAPI_GL13);
    st_lua_set_integer_to_field(lua_state, "gapi_gl14", ST_GAPI_GL14);
    st_lua_set_integer_to_field(lua_state, "gapi_gl15", ST_GAPI_GL15);
    st_lua_set_integer_to_field(lua_state, "gapi_gl2", ST_GAPI_GL2);
    st_lua_set_integer_to_field(lua_state, "gapi_gl21", ST_GAPI_GL21);
    st_lua_set_integer_to_field(lua_state, "gapi_gl3", ST_GAPI_GL3);
    st_lua_set_integer_to_field(lua_state, "gapi_gl31", ST_GAPI_GL31);
    st_lua_set_integer_to_field(lua_state, "gapi_gl32", ST_GAPI_GL32);
    st_lua_set_integer_to_field(lua_state, "gapi_gl33", ST_GAPI_GL33);
    st_lua_set_integer_to_field(lua_state, "gapi_gl4", ST_GAPI_GL4);
    st_lua_set_integer_to_field(lua_state, "gapi_gl41", ST_GAPI_GL41);
    st_lua_set_integer_to_field(lua_state, "gapi_gl42", ST_GAPI_GL42);
    st_lua_set_integer_to_field(lua_state, "gapi_gl43", ST_GAPI_GL43);
    st_lua_set_integer_to_field(lua_state, "gapi_gl44", ST_GAPI_GL44);
    st_lua_set_integer_to_field(lua_state, "gapi_gl45", ST_GAPI_GL45);
    st_lua_set_integer_to_field(lua_state, "gapi_gl46", ST_GAPI_GL46);
    st_lua_set_integer_to_field(lua_state, "gapi_es1", ST_GAPI_ES1);
    st_lua_set_integer_to_field(lua_state, "gapi_es11", ST_GAPI_ES11);
    st_lua_set_integer_to_field(lua_state, "gapi_es2", ST_GAPI_ES2);
    st_lua_set_integer_to_field(lua_state, "gapi_es3", ST_GAPI_ES3);
    st_lua_set_integer_to_field(lua_state, "gapi_es31", ST_GAPI_ES31);
    st_lua_set_integer_to_field(lua_state, "gapi_es32", ST_GAPI_ES32);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, MODCTX_METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_gfxctx_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_gfxctx_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "create", st_gfxctx_create_bind);
    st_lua_set_cfunction_to_field(lua_state, "create_shared",
     st_gfxctx_create_shared_bind);

    st_lua_pop(lua_state, 1);

    st_lua_create_metatable(lua_state, GFXCTX_METATABLE_NAME);
    st_lua_set_cfunction_to_field(lua_state, "__gc", st_gfxctx_destroy_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_gfxctx_destroy_bind);
    st_lua_set_cfunction_to_field(lua_state, "make_current",
     st_gfxctx_make_current_bind);
    st_lua_set_cfunction_to_field(lua_state, "swap_buffers",
     st_gfxctx_swap_buffers_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_api", st_gfxctx_get_api_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);

    st_lua_pop(lua_state, 1);
}
