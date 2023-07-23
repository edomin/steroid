#include "opts.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE     1024
#define METATABLE_NAME       "st_opts"
#define ST_OPT_SIZE_MAX      256
#define HELP_BUFFER_SIZE_MAX 131072

static void                               *global_modsmgr;
static st_modsmgr_funcs_t                  global_modsmgr_funcs;
static char                                err_msg_buf[ERR_MSG_BUF_SIZE];

static st_opts_init_t                      st_opts_init;
static st_opts_quit_t                      st_opts_quit;
static st_opts_add_option_t                st_opts_add_option;
static st_opts_get_str_t                   st_opts_get_str;
static st_opts_get_help_t                  st_opts_get_help;

static st_lua_get_state_t                  st_lua_get_state;
static st_lua_create_userdata_t            st_lua_create_userdata;
static st_lua_create_metatable_t           st_lua_create_metatable;
static st_lua_set_metatable_t              st_lua_set_metatable;
static st_lua_push_bool_t                  st_lua_push_bool;
static st_lua_push_nil_t                   st_lua_push_nil;
static st_lua_push_string_t                st_lua_push_string;
static st_lua_raise_error_t                st_lua_raise_error;
static st_lua_set_integer_to_field_t       st_lua_set_integer_to_field;
static st_lua_set_cfunction_to_field_t     st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t          st_lua_set_copy_to_field;
static st_lua_get_char_t                   st_lua_get_char;
static st_lua_get_integer_t                st_lua_get_integer;
static st_lua_get_string_t                 st_lua_get_string;
static st_lua_get_string_or_null_t         st_lua_get_string_or_null;
static st_lua_get_named_userdata_t         st_lua_get_named_userdata;
static st_lua_get_global_userdata_t        st_lua_get_global_userdata;
static st_lua_register_cfunction_t         st_lua_register_cfunction;
static st_lua_pop_t                        st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

void *st_module_luabind_opts_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_luabind_opts_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_luabind_opts_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"luabind_opts\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_luabind_opts_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_opts_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_opts_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_opts: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_opts", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_opts", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", opts, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", opts, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", opts, add_option);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", opts, get_str);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", opts, get_help);

    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, push_nil);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, push_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, raise_error);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_char);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_string_or_null);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, get_global_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, register_cfunction);
    ST_LOAD_GLOBAL_FUNCTION("luabind_opts", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t       *luabind_ctx;
    st_luabind_opts_t *luabind;
    errno_t            err;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_opts_data, sizeof(st_luabind_opts_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_opts_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx))
        return NULL;

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_opts: Opts binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_opts_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_opts: Opts binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_opts_init_bind(st_luastate_t *lua_state) {
    st_lua_raise_error(lua_state,
     "luabind_opts: st_opts() is not implemented. Use st_opts_get_instance() "
     "instead");

    st_lua_push_nil(lua_state);

    return 1;
}

static int st_opts_get_instance_bind(st_luastate_t *lua_state) {
    void *userdata = st_lua_create_userdata(lua_state, sizeof(st_modctx_t *));

    *(st_modctx_t **)userdata = st_lua_get_global_userdata(lua_state,
     "__st_opts_ctx");
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_opts_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *opts_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    st_modctx_t *opts_ctx_instance = st_lua_get_global_userdata(lua_state,
     "__st_opts_ctx");

    if (opts_ctx != opts_ctx_instance)
        st_opts_quit(opts_ctx);

    return 0;
}

static int st_opts_add_option_bind(st_luastate_t *lua_state) {
    st_modctx_t *opts_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    char        shortopt = st_lua_get_char(lua_state, 2);
    const char *longopt = st_lua_get_string_or_null(lua_state, 3);
    ptrdiff_t   argreq = st_lua_get_integer(lua_state, 4);
    const char *argfmt = st_lua_get_string_or_null(lua_state, 5); // NOLINT(readability-magic-numbers)
    const char *optdescr = st_lua_get_string_or_null(lua_state, 6); // NOLINT(readability-magic-numbers)
    bool        result;

    result = st_opts_add_option(opts_ctx, shortopt, longopt,
     (st_opt_arg_t)argreq, argfmt, optdescr);

    st_lua_push_bool(lua_state, result);

    return 1;
}

static int st_opts_get_str_bind(st_luastate_t *lua_state) {
    st_modctx_t *opts_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char *opt = st_lua_get_string(lua_state, 2);
    char        buffer[ST_OPT_SIZE_MAX];

    if (st_opts_get_str(opts_ctx, opt, buffer, ST_OPT_SIZE_MAX))
        st_lua_push_string(lua_state, buffer);
    else
        st_lua_push_nil(lua_state);

    return 1;
}

static int st_opts_get_help_bind(st_luastate_t *lua_state) {
    st_modctx_t *opts_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    columns = st_lua_get_integer(lua_state, 2);
    char         buffer[HELP_BUFFER_SIZE_MAX];

    if (st_opts_get_help(opts_ctx, buffer, HELP_BUFFER_SIZE_MAX,
     (size_t)columns))
        st_lua_push_string(lua_state, buffer);
    else
        st_lua_push_nil(lua_state);

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_opts_t *module = luabind_ctx->data;
    st_luastate_t     *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_register_cfunction(lua_state, METATABLE_NAME, st_opts_init_bind);
    st_lua_register_cfunction(lua_state, "st_opts_get_instance",
     st_opts_get_instance_bind);
    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_opts_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "quit", st_opts_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "add_option",
     st_opts_add_option_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_str", st_opts_get_str_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_help", st_opts_get_help_bind);
    st_lua_set_integer_to_field(lua_state, "oa_no", ST_OA_NO);
    st_lua_set_integer_to_field(lua_state, "oa_required", ST_OA_REQUIRED);
    st_lua_set_integer_to_field(lua_state, "oa_optional", ST_OA_OPTIONAL);

    st_lua_pop(lua_state, 1);
}
