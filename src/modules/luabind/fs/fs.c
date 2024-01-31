#include "fs.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define METATABLE_NAME "fs_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;

static st_fs_init_t                    st_fs_init;
static st_fs_quit_t                    st_fs_quit;
static st_fs_get_file_type_t           st_fs_get_file_type;
static st_fs_mkdir_t                   st_fs_mkdir;

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
static st_lua_get_string_t             st_lua_get_string;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_fs)
ST_MODULE_DEF_INIT_FUNC(luabind_fs)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_fs_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_fs_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_fs: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_fs", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_fs", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", fs, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", fs, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", fs, get_file_type);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", fs, mkdir);

    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, push_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, get_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_fs", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t     *luabind_ctx;
    st_luabind_fs_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_fs_data, sizeof(st_luabind_fs_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_fs_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_fs: FS funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_fs_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_fs: FS funcs binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_fs_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");
    st_modctx_t *pathtools_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "pathtools_ctx");

    *(st_modctx_t **)userdata = st_fs_init(logger_ctx, pathtools_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_fs_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *fs_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_fs_quit(fs_ctx);

    return 0;
}

static int fs_get_file_type_bind(st_luastate_t *lua_state) {
    st_modctx_t *fs_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *filename = st_lua_get_string(lua_state, 2);

    st_lua_push_integer(lua_state, st_fs_get_file_type(fs_ctx, filename));

    return 1;
}

static int fs_mkdir_bind(st_luastate_t *lua_state) {
    st_modctx_t *fs_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *dirname = st_lua_get_string(lua_state, 2);

    st_lua_push_bool(lua_state, st_fs_mkdir(fs_ctx, dirname));

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_fs_t *module = luabind_ctx->data;
    st_luastate_t   *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Fs");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_fs_init_bind);
    st_lua_set_integer_to_field(lua_state, "ft_unknown", ST_FT_UNKNOWN);
    st_lua_set_integer_to_field(lua_state, "ft_reg", ST_FT_REG);
    st_lua_set_integer_to_field(lua_state, "ft_dir", ST_FT_DIR);
    st_lua_set_integer_to_field(lua_state, "ft_chr", ST_FT_CHR);
    st_lua_set_integer_to_field(lua_state, "ft_blk", ST_FT_BLK);
    st_lua_set_integer_to_field(lua_state, "ft_fifo", ST_FT_FIFO);
    st_lua_set_integer_to_field(lua_state, "ft_link", ST_FT_LINK);
    st_lua_set_integer_to_field(lua_state, "ft_sock", ST_FT_SOCK);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_fs_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_fs_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "get_file_type",
     fs_get_file_type_bind);
    st_lua_set_cfunction_to_field(lua_state, "mkdir", fs_mkdir_bind);

    st_lua_pop(lua_state, 1);
}
