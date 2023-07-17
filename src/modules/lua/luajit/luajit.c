#include "luajit.h"

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_lua_luajit_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_lua_luajit_funcs_table;

    for (size_t i = 0; i < funcs_table->funcs_count; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_lua_luajit_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"lua_luajit\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_lua_luajit_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_lua_luajit_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_lua_import_functions(st_modctx_t *lua_ctx,
 st_modctx_t *logger_ctx) {
    st_lua_luajit_t *module = lua_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "lua_luajit: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("lua_luajit", logger, debug);
    ST_LOAD_FUNCTION("lua_luajit", logger, info);

    return true;
}

static st_modctx_t *st_lua_init(st_modctx_t *logger_ctx,
 st_modctx_t *opts_ctx) {
    st_modctx_t     *lua_ctx;
    st_lua_luajit_t *module;

    lua_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_lua_luajit_data, sizeof(st_lua_luajit_t));

    if (!lua_ctx)
        return NULL;

    lua_ctx->funcs = &st_lua_luajit_funcs;

    module = lua_ctx->data;
    module->logger.ctx = logger_ctx;
    module->opts.ctx = opts_ctx;

    if (!st_lua_import_functions(lua_ctx, logger_ctx))
        return NULL;

    module->state = luaL_newstate();
    luaL_openlibs(module->state);

    module->logger.info(module->logger.ctx, "lua_luajit: Lua initialized.");

    return lua_ctx;
}

static void st_lua_quit(st_modctx_t *lua_ctx) {
    st_lua_luajit_t *lua = lua_ctx->data;

    lua_close(lua->state);

    lua->logger.info(lua->logger.ctx, "lua_luajit: Lua destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, lua_ctx);
}

static st_modctx_t *st_lua_get_logger_ctx(st_modctx_t *lua_ctx) {
    st_lua_luajit_t *module = lua_ctx->data;

    return module->logger.ctx;
}

static st_modctx_t *st_lua_get_opts_ctx(st_modctx_t *lua_ctx) {
    st_lua_luajit_t *module = lua_ctx->data;

    return module->opts.ctx;
}

static bool st_lua_run(st_modctx_t *lua_ctx,
 int (*func)(lua_State *, const char *), const char *arg) {
    st_lua_luajit_t *lua = lua_ctx->data;
    bool             success = (func(lua->state, arg) ||
     lua_pcall(lua->state, 0, LUA_MULTRET, 0)) == LUA_OK;

    if (success) {
        lua_pop(lua->state, lua_gettop(lua->state));
    } else {
        const char *error = lua_tostring(lua->state, lua_gettop(lua->state));

        lua->logger.error(lua->logger.ctx, "lua_luajit: %s", error);
    }

    return success;
}

static bool st_lua_run_string(st_modctx_t *lua_ctx, const char *string) {
    return st_lua_run(lua_ctx, luaL_loadstring, string);
}

static bool st_lua_run_file(st_modctx_t *lua_ctx, const char *filename) {
    return st_lua_run(lua_ctx, luaL_loadfile, filename);
}

static st_luastate_t *st_lua_get_state(st_modctx_t *lua_ctx) {
    st_lua_luajit_t *lua = lua_ctx->data;

    return (st_luastate_t *)lua->state;
}

static void *st_lua_create_userdata(st_luastate_t *lua_state, size_t size) {
    return lua_newuserdata((lua_State *)lua_state, size);
}

static void st_lua_create_metatable(st_luastate_t *lua_state,
 const char *name) {
    luaL_newmetatable((lua_State *)lua_state, name);
}

static void st_lua_set_metatable(st_luastate_t *lua_state, const char *name) {
    luaL_setmetatable((lua_State *)lua_state, name);
}

static void st_lua_push_bool(st_luastate_t *lua_state, bool val) {
    lua_pushboolean((lua_State *)lua_state, val);
}

static void st_lua_set_nil_to_field(st_luastate_t *lua_state,
 const char *name) {
    lua_pushnil((lua_State *)lua_state);
    lua_setfield((lua_State *)lua_state, -2, name);
}

static void st_lua_set_integer_to_field(st_luastate_t *lua_state,
 const char *name, ptrdiff_t integer) {
    lua_pushinteger((lua_State *)lua_state, integer);
    lua_setfield((lua_State *)lua_state, -2, name);
}

static void st_lua_set_cfunction_to_field(st_luastate_t *lua_state,
 const char *name, void *cfunction) {
    lua_pushcfunction((lua_State *)lua_state, cfunction);
    lua_setfield((lua_State *)lua_state, -2, name);
}

static ptrdiff_t st_lua_get_integer(st_luastate_t *lua_state, int index) {
    return luaL_checkinteger((lua_State *)lua_state, index);
}

static const char *st_lua_get_string(st_luastate_t *lua_state, int index) {
    return luaL_checkstring((lua_State *)lua_state, index);
}

static void *st_lua_get_named_userdata(st_luastate_t *lua_state, int index,
 const char *name) {
    return luaL_checkudata((lua_State *)lua_state, index, name);
}

static void st_lua_register_cfunction(st_luastate_t *lua_state,
 const char *name, void *cfunction) {
    lua_register((lua_State *)lua_state, name, cfunction);
}

static void st_lua_pop(st_luastate_t *lua_state, size_t elements_count) {
    lua_pop((lua_State *)lua_state, (int)elements_count);
}
