#include "logger.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "st_logger"

static st_modsmgr_t                       *global_modsmgr;
static st_modsmgr_funcs_t                  global_modsmgr_funcs;
static char                                err_msg_buf[ERR_MSG_BUF_SIZE];

static st_logger_init_t                    st_logger_init;
static st_logger_quit_t                    st_logger_quit;
static st_logger_set_stdout_levels_t       st_logger_set_stdout_levels;
static st_logger_set_stderr_levels_t       st_logger_set_stderr_levels;
static st_logger_set_syslog_levels_t       st_logger_set_syslog_levels;
static st_logger_set_log_file_t            st_logger_set_log_file;
static st_logger_set_callback_t            st_logger_set_callback;
static st_logger_debug_t                   st_logger_debug;
static st_logger_info_t                    st_logger_info;
static st_logger_warning_t                 st_logger_warning;
static st_logger_error_t                   st_logger_error;
static st_logger_set_postmortem_msg_t      st_logger_set_postmortem_msg;

static st_lua_get_state_t                  st_lua_get_state;
static st_lua_create_userdata_t            st_lua_create_userdata;
static st_lua_create_metatable_t           st_lua_create_metatable;
static st_lua_set_metatable_t              st_lua_set_metatable;
static st_lua_push_bool_t                  st_lua_push_bool;
static st_lua_set_integer_to_field_t       st_lua_set_integer_to_field;
static st_lua_set_cfunction_to_field_t     st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t          st_lua_set_copy_to_field;
static st_lua_get_integer_t                st_lua_get_integer;
static st_lua_get_string_t                 st_lua_get_string;
static st_lua_get_named_userdata_t         st_lua_get_named_userdata;
static st_lua_get_named_userdata_or_null_t st_lua_get_named_userdata_or_null;
static st_lua_get_global_userdata_t        st_lua_get_global_userdata;
static st_lua_register_cfunction_t         st_lua_register_cfunction;
static st_lua_pop_t                        st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

void *st_module_luabind_logger_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_luabind_logger_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_luabind_logger_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"luabind_logger\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_luabind_logger_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_logger_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_logger_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_logger: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_logger", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_logger", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_stdout_levels);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_stderr_levels);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_syslog_levels);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_log_file);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_callback);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, debug);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, info);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, warning);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, error);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", logger, set_postmortem_msg);

    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_named_userdata_or_null);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, get_global_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, register_cfunction);
    ST_LOAD_GLOBAL_FUNCTION("luabind_logger", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t         *luabind_ctx;
    st_luabind_logger_t *luabind;
    errno_t              err;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_logger_data, sizeof(st_luabind_logger_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_logger_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_logger: Logger binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_logger_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_logger: Logger binding destroyed.");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_logger_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *events_ctx =
     *(st_modctx_t **)st_lua_get_named_userdata_or_null(lua_state, 1,
      "st_events");

    *(st_modctx_t **)userdata = st_logger_init(events_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_logger_get_instance_bind(st_luastate_t *lua_state) {
    void *userdata = st_lua_create_userdata(lua_state, sizeof(st_modctx_t *));

    *(st_modctx_t **)userdata = st_lua_get_global_userdata(lua_state,
     "__st_logger_ctx");
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_logger_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    st_modctx_t *logger_ctx_instance = st_lua_get_global_userdata(lua_state,
     "__st_logger_ctx");

    if (logger_ctx != logger_ctx_instance)
        st_logger_quit(logger_ctx);

    return 0;
}

static int st_logger_set_stdout_levels_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    levels = st_lua_get_integer(lua_state, 2);
    bool         result = st_logger_set_stdout_levels(logger_ctx,
     (st_loglvl_t)levels);

    st_lua_push_bool(lua_state, result);

    return 1;
}

static int st_logger_set_stderr_levels_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    levels = st_lua_get_integer(lua_state, 2);
    bool         result = st_logger_set_stderr_levels(logger_ctx,
     (st_loglvl_t)levels);

    st_lua_push_bool(lua_state, result);

    return 1;
}

static int st_logger_set_syslog_levels_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    ptrdiff_t    levels = st_lua_get_integer(lua_state, 2);
    bool         result = st_logger_set_syslog_levels(logger_ctx,
     (st_loglvl_t)levels);

    st_lua_push_bool(lua_state, result);

    return 1;
}

static int st_logger_set_log_file_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *filename = st_lua_get_string(lua_state, 2);
    ptrdiff_t    levels = st_lua_get_integer(lua_state, 3);
    bool         result = st_logger_set_log_file(logger_ctx, filename,
     (st_loglvl_t)levels);

    st_lua_push_bool(lua_state, result);

    return 1;
}

// TODO(edomin): implement later
static int st_logger_set_callback_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_logger_error(logger_ctx, "luabind_logger: Binding for function "
     "st_logger_set_callback is not implemented");

    st_lua_push_bool(lua_state, false);

    return 1;
}

static int st_logger_debug_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *msg = st_lua_get_string(lua_state, 2);

    st_logger_debug(logger_ctx, msg);

    return 0;
}

static int st_logger_info_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *msg = st_lua_get_string(lua_state, 2);

    st_logger_info(logger_ctx, msg);

    return 0;
}

static int st_logger_warning_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *msg = st_lua_get_string(lua_state, 2);

    st_logger_warning(logger_ctx, msg);

    return 0;
}

static int st_logger_error_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *msg = st_lua_get_string(lua_state, 2);

    st_logger_error(logger_ctx, msg);

    return 0;
}

static int st_logger_set_postmortem_msg_bind(st_luastate_t *lua_state) {
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    const char  *msg = st_lua_get_string(lua_state, 2);

    st_logger_set_postmortem_msg(logger_ctx, msg);

    return 0;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_logger_t *module = luabind_ctx->data;
    st_luastate_t       *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_register_cfunction(lua_state, METATABLE_NAME, st_logger_init_bind);
    st_lua_register_cfunction(lua_state, "st_logger_get_instance",
     st_logger_get_instance_bind);
    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_logger_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "quit", st_logger_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "set_stdout_levels",
     st_logger_set_stdout_levels_bind);
    st_lua_set_cfunction_to_field(lua_state, "set_stderr_levels",
     st_logger_set_stderr_levels_bind);
    st_lua_set_cfunction_to_field(lua_state, "set_syslog_levels",
     st_logger_set_syslog_levels_bind);
    st_lua_set_cfunction_to_field(lua_state, "set_log_file",
     st_logger_set_log_file_bind);
    st_lua_set_cfunction_to_field(lua_state, "set_callback",
     st_logger_set_callback_bind);
    st_lua_set_cfunction_to_field(lua_state, "debug", st_logger_debug_bind);
    st_lua_set_cfunction_to_field(lua_state, "info", st_logger_info_bind);
    st_lua_set_cfunction_to_field(lua_state, "warning", st_logger_warning_bind);
    st_lua_set_cfunction_to_field(lua_state, "error", st_logger_error_bind);
    st_lua_set_cfunction_to_field(lua_state, "set_postmortem_msg",
     st_logger_set_postmortem_msg_bind);
    st_lua_set_integer_to_field(lua_state, "ll_none", ST_LL_NONE);
    st_lua_set_integer_to_field(lua_state, "ll_error", ST_LL_ERROR);
    st_lua_set_integer_to_field(lua_state, "ll_warning", ST_LL_WARNING);
    st_lua_set_integer_to_field(lua_state, "ll_info", ST_LL_INFO);
    st_lua_set_integer_to_field(lua_state, "ll_debug", ST_LL_DEBUG);
    st_lua_set_integer_to_field(lua_state, "ll_all", ST_LL_ALL);

    st_lua_pop(lua_state, 1);
}
