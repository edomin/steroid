#include "events.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "events_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_events_init_t                st_events_init;
static st_events_quit_t                st_events_quit;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_events)
ST_MODULE_DEF_INIT_FUNC(luabind_events)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_events_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_events_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_events: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_events", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_events", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_events", events, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", events, quit);

    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_events", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t         *luabind_ctx;
    st_luabind_events_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_events_data, sizeof(st_luabind_events_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_events_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_events: Events funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_events_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_events: Events funcs binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_events_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");

    *(st_modctx_t **)userdata = st_events_init(logger_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_events_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *events_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_events_quit(events_ctx);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_events_t *module = luabind_ctx->data;
    st_luastate_t       *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Events");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_events_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_events_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_events_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);

    st_lua_pop(lua_state, 1);
}
