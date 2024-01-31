#include "atlas.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define CTX_METATABLE_NAME "atlas_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;

static st_atlas_init_t                 st_atlas_init;
static st_atlas_quit_t                 st_atlas_quit;

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

ST_MODULE_DEF_GET_FUNC(luabind_atlas)
ST_MODULE_DEF_INIT_FUNC(luabind_atlas)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_atlas_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_atlas_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_atlas: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_atlas", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_atlas", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", atlas, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", atlas, quit);

    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_atlas", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t        *luabind_ctx;
    st_luabind_atlas_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_atlas_data, sizeof(st_luabind_atlas_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_atlas_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_atlas: Atlas funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_atlas_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_atlas: Atlas funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_atlas_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");
    st_modctx_t *texture_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "texture_ctx");

    *(st_modctx_t **)userdata = st_atlas_init(logger_ctx, texture_ctx);
    st_lua_set_metatable(lua_state, CTX_METATABLE_NAME);

    return 1;
}

static int st_atlas_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *atlas_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_atlas_quit(atlas_ctx);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_atlas_t *module = luabind_ctx->data;
    st_luastate_t      *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Atlas");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_atlas_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, CTX_METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_atlas_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_atlas_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);

    st_lua_pop(lua_state, 1);
}
