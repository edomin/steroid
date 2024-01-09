#include "angle.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "angle_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_angle_init_t                 st_angle_init;
static st_angle_quit_t                 st_angle_quit;
static st_angle_rtod_t                 st_angle_rtod;
static st_angle_dtor_t                 st_angle_dtor;
static st_angle_rnormalized360_t       st_angle_rnormalized360;
static st_angle_dnormalized360_t       st_angle_dnormalized360;
static st_angle_rdsin_t                st_angle_rdsin;
static st_angle_dgsin_t                st_angle_dgsin;
static st_angle_rdcos_t                st_angle_rdcos;
static st_angle_dgcos_t                st_angle_dgcos;
static st_angle_rdtan_t                st_angle_rdtan;
static st_angle_dgtan_t                st_angle_dgtan;
static st_angle_rdacos_t               st_angle_rdacos;
static st_angle_dgacos_t               st_angle_dgacos;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_push_double_t            st_lua_push_double;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_double_t             st_lua_get_double;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_angle)
ST_MODULE_DEF_INIT_FUNC(luabind_angle)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_angle_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_angle_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_angle: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_angle", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_angle", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rtod);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dtor);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rnormalized360);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dnormalized360);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rdsin);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dgsin);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rdcos);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dgcos);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rdtan);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dgtan);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, rdacos);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", angle, dgacos);

    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, push_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, get_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_angle", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t        *luabind_ctx;
    st_luabind_angle_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_angle_data, sizeof(st_luabind_angle_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_angle_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_angle: Angle funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_angle_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_angle: Angle funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_angle_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");

    *(st_modctx_t **)userdata = st_angle_init(logger_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_angle_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_angle_quit(angle_ctx);

    return 0;
}

static int st_angle_rtod_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       radians = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_rtod(angle_ctx, (float)radians));

    return 1;
}

static int st_angle_dtor_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       degrees = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_dtor(angle_ctx, (float)degrees));

    return 1;
}

static int st_angle_rnormalized360_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       radians = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state,
     st_angle_rnormalized360(angle_ctx, (float)radians));

    return 1;
}

static int st_angle_dnormalized360_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       degrees = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state,
     st_angle_dnormalized360(angle_ctx, (float)degrees));

    return 1;
}

static int st_angle_rdsin_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       radians = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_rdsin(angle_ctx, (float)radians));

    return 1;
}

static int st_angle_dgsin_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       degrees = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_dgsin(angle_ctx, (float)degrees));

    return 1;
}

static int st_angle_rdcos_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       radians = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_rdcos(angle_ctx, (float)radians));

    return 1;
}

static int st_angle_dgcos_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       degrees = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_dgcos(angle_ctx, (float)degrees));

    return 1;
}

static int st_angle_rdtan_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       radians = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_rdtan(angle_ctx, (float)radians));

    return 1;
}

static int st_angle_dgtan_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       degrees = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_dgtan(angle_ctx, (float)degrees));

    return 1;
}

static int st_angle_rdacos_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       angle_cos = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_rdacos(angle_ctx, (float)angle_cos));

    return 1;
}

static int st_angle_dgacos_bind(st_luastate_t *lua_state) {
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       angle_cos = st_lua_get_double(lua_state, 2);

    st_lua_push_double(lua_state, st_angle_dgacos(angle_ctx, (float)angle_cos));

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_angle_t *module = luabind_ctx->data;
    st_luastate_t       *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Angle");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_angle_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_angle_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_angle_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "rtod", st_angle_rtod_bind);
    st_lua_set_cfunction_to_field(lua_state, "dtor", st_angle_dtor_bind);
    st_lua_set_cfunction_to_field(lua_state, "rnormalized360",
     st_angle_rnormalized360_bind);
    st_lua_set_cfunction_to_field(lua_state, "dnormalized360",
     st_angle_dnormalized360_bind);
    st_lua_set_cfunction_to_field(lua_state, "rdsin", st_angle_rdsin_bind);
    st_lua_set_cfunction_to_field(lua_state, "dgsin", st_angle_dgsin_bind);
    st_lua_set_cfunction_to_field(lua_state, "rdcos", st_angle_rdcos_bind);
    st_lua_set_cfunction_to_field(lua_state, "dgcos", st_angle_dgcos_bind);
    st_lua_set_cfunction_to_field(lua_state, "rdtan", st_angle_rdtan_bind);
    st_lua_set_cfunction_to_field(lua_state, "dgtan", st_angle_dgtan_bind);
    st_lua_set_cfunction_to_field(lua_state, "rdacos", st_angle_rdacos_bind);
    st_lua_set_cfunction_to_field(lua_state, "dgacos", st_angle_dgacos_bind);

    st_lua_pop(lua_state, 1);
}
