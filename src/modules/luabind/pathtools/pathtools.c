#include "pathtools.h"

#include <limits.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "pathtools_ctx"

static void                           *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_pathtools_init_t             st_pathtools_init;
static st_pathtools_quit_t             st_pathtools_quit;
static st_pathtools_get_parent_dir_t   st_pathtools_get_parent_dir;
static st_pathtools_concat_t           st_pathtools_concat;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_push_nil_t               st_lua_push_nil;
static st_lua_push_string_t            st_lua_push_string;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_string_t             st_lua_get_string;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_pathtools)
ST_MODULE_DEF_INIT_FUNC(luabind_pathtools)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_pathtools_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_pathtools_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_pathtools: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_pathtools", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_pathtools", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_pathtools", pathtools, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_pathtools", pathtools, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_pathtools", pathtools, get_parent_dir);
    ST_LOAD_GLOBAL_FUNCTION("luabind_pathtools", pathtools, concat);

    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, push_nil);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, push_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t            *luabind_ctx;
    st_luabind_pathtools_t *luabind;
    errno_t                 err;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_pathtools_data, sizeof(st_luabind_pathtools_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_pathtools_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_pathtools: Opts binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_pathtools_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_pathtools: Opts binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_pathtools_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");

    *(st_modctx_t **)userdata = st_pathtools_init(logger_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_pathtools_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *pathtools_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_pathtools_quit(pathtools_ctx);

    return 0;
}

static int st_pathtools_get_parent_dir_bind(st_luastate_t *lua_state) {
    st_modctx_t *pathtools_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *path = st_lua_get_string(lua_state, 2);
    char         result_path[PATH_MAX];
    bool         success = st_pathtools_get_parent_dir(pathtools_ctx,
     result_path, PATH_MAX, path);

    if (success)
        st_lua_push_string(lua_state, result_path);
    else
        st_lua_push_nil(lua_state);

    return 1;
}

static int st_pathtools_concat_bind(st_luastate_t *lua_state) {
    st_modctx_t *pathtools_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *path = st_lua_get_string(lua_state, 2);
    const char  *append = st_lua_get_string(lua_state, 3);
    char         result_path[PATH_MAX];
    bool         success = st_pathtools_concat(pathtools_ctx, result_path,
     PATH_MAX, path, append);

    if (success)
        st_lua_push_string(lua_state, result_path);
    else
        st_lua_push_nil(lua_state);

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_pathtools_t *module = luabind_ctx->data;
    st_luastate_t          *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "PathTools");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_pathtools_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_pathtools_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_pathtools_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "get_parent_dir",
     st_pathtools_get_parent_dir_bind);
    st_lua_set_cfunction_to_field(lua_state, "concat",
     st_pathtools_concat_bind);

    st_lua_pop(lua_state, 1);
}
