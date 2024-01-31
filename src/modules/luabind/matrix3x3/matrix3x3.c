#include "matrix3x3.h"

#include <stdio.h>

#define CTX_METATABLE_NAME    "matrix3x3_ctx"
#define MATRIX_METATABLE_NAME "matrix3x3"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;

static st_matrix3x3_init_t             st_matrix3x3_init;
static st_matrix3x3_quit_t             st_matrix3x3_quit;
static st_matrix3x3_clone_t            st_matrix3x3_clone;
static st_matrix3x3_custom_t           st_matrix3x3_custom;
static st_matrix3x3_identity_t         st_matrix3x3_identity;
static st_matrix3x3_translation_t      st_matrix3x3_translation;
static st_matrix3x3_scaling_t          st_matrix3x3_scaling;
static st_matrix3x3_rrotation_t        st_matrix3x3_rrotation;
static st_matrix3x3_drotation_t        st_matrix3x3_drotation;
static st_matrix3x3_rhshearing_t       st_matrix3x3_rhshearing;
static st_matrix3x3_dhshearing_t       st_matrix3x3_dhshearing;
static st_matrix3x3_rvshearing_t       st_matrix3x3_rvshearing;
static st_matrix3x3_dvshearing_t       st_matrix3x3_dvshearing;
static st_matrix3x3_apply_t            st_matrix3x3_apply;
static st_matrix3x3_translate_t        st_matrix3x3_translate;
static st_matrix3x3_scale_t            st_matrix3x3_scale;
static st_matrix3x3_rrotate_t          st_matrix3x3_rrotate;
static st_matrix3x3_drotate_t          st_matrix3x3_drotate;
static st_matrix3x3_rhshear_t          st_matrix3x3_rhshear;
static st_matrix3x3_dhshear_t          st_matrix3x3_dhshear;
static st_matrix3x3_rvshear_t          st_matrix3x3_rvshear;
static st_matrix3x3_dvshear_t          st_matrix3x3_dvshear;
static st_matrix3x3_get_data_t         st_matrix3x3_get_data;

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

ST_MODULE_DEF_GET_FUNC(luabind_matrix3x3)
ST_MODULE_DEF_INIT_FUNC(luabind_matrix3x3)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_matrix3x3_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_matrix3x3_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_matrix3x3: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_matrix3x3", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_matrix3x3", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, clone);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, custom);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, identity);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, translation);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, scaling);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rrotation);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, drotation);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rhshearing);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, dhshearing);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rvshearing);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, dvshearing);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, apply);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, translate);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, scale);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rrotate);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, drotate);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rhshear);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, dhshear);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, rvshear);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, dvshear);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", matrix3x3, get_data);

    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, push_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, get_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_matrix3x3", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t            *luabind_ctx;
    st_luabind_matrix3x3_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_matrix3x3_data, sizeof(st_luabind_matrix3x3_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_matrix3x3_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_matrix3x3: Matrix 3x3 utilities binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_matrix3x3_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_matrix3x3: Matrix 3x3 utilities binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_matrix3x3_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "angle_ctx");

    *(st_modctx_t **)userdata = st_matrix3x3_init(logger_ctx, angle_ctx);
    st_lua_set_metatable(lua_state, CTX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_matrix3x3_quit(matrix3x3_ctx);

    return 0;
}

static int st_matrix3x3_clone_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    st_matrix3x3_t  new_matrix;
    void           *userdata;

    st_matrix3x3_clone(&new_matrix, matrix);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_custom_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         r1c1 = st_lua_get_double(lua_state, 2);
    double         r1c2 = st_lua_get_double(lua_state, 3);
    double         r1c3 = st_lua_get_double(lua_state, 4);
    double         r2c1 = st_lua_get_double(lua_state, 5);
    double         r2c2 = st_lua_get_double(lua_state, 6);
    double         r2c3 = st_lua_get_double(lua_state, 7);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_custom(matrix3x3_ctx, &new_matrix, (float)r1c1, (float)r1c2,
     (float)r1c3, (float)r2c1, (float)r2c2, (float)r2c3);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_identity_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_identity(matrix3x3_ctx, &new_matrix);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_translation_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         x = st_lua_get_double(lua_state, 2);
    double         y = st_lua_get_double(lua_state, 3);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_translation(matrix3x3_ctx, &new_matrix, (float)x, (float)y);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_scaling_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         hscale = st_lua_get_double(lua_state, 2);
    double         vscale = st_lua_get_double(lua_state, 3);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_scaling(matrix3x3_ctx, &new_matrix, (float)hscale,
     (float)vscale);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rrotation_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         radians = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_rrotation(matrix3x3_ctx, &new_matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_drotation_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         degrees = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_drotation(matrix3x3_ctx, &new_matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rhshearing_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         radians = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_rhshearing(matrix3x3_ctx, &new_matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_dhshearing_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         degrees = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_dhshearing(matrix3x3_ctx, &new_matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rvshearing_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         radians = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_rvshearing(matrix3x3_ctx, &new_matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_dvshearing_bind(st_luastate_t *lua_state) {
    st_modctx_t   *matrix3x3_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    double         degrees = st_lua_get_double(lua_state, 2);
    st_matrix3x3_t new_matrix;
    void          *userdata;

    st_matrix3x3_dvshearing(matrix3x3_ctx, &new_matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = new_matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_apply_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    st_matrix3x3_t *other = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 2, MATRIX_METATABLE_NAME);
    void           *userdata;

    st_matrix3x3_apply(matrix, other);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_translate_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          x = st_lua_get_double(lua_state, 2);
    double          y = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_translate(matrix, (float)x, (float)y);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_scale_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          hscale = st_lua_get_double(lua_state, 2);
    double          vscale = st_lua_get_double(lua_state, 3);
    void           *userdata;

    st_matrix3x3_scale(matrix, (float)hscale, (float)vscale);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rrotate_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          radians = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_rrotate(matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_drotate_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          degrees = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_drotate(matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rhshear_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          radians = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_rhshear(matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_dhshear_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          degrees = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_dhshear(matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_rvshear_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          radians = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_rvshear(matrix, (float)radians);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_dvshear_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    double          degrees = st_lua_get_double(lua_state, 2);
    void           *userdata;

    st_matrix3x3_dvshear(matrix, (float)degrees);

    userdata = st_lua_create_userdata(lua_state, sizeof(st_matrix3x3_t));
    *(st_matrix3x3_t *)userdata = *matrix;
    st_lua_set_metatable(lua_state, MATRIX_METATABLE_NAME);

    return 1;
}

static int st_matrix3x3_get_data_bind(st_luastate_t *lua_state) {
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 1, MATRIX_METATABLE_NAME);
    float           r1c1;
    float           r1c2;
    float           r1c3;
    float           r2c1;
    float           r2c2;
    float           r2c3;

    st_matrix3x3_get_data(&r1c1, &r1c2, &r1c3, &r2c1, &r2c2, &r2c3, matrix);

    st_lua_push_double(lua_state, r1c1);
    st_lua_push_double(lua_state, r1c2);
    st_lua_push_double(lua_state, r1c3);
    st_lua_push_double(lua_state, r2c1);
    st_lua_push_double(lua_state, r2c2);
    st_lua_push_double(lua_state, r2c3);

    return 6;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_matrix3x3_t *module = luabind_ctx->data;
    st_luastate_t          *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Matrix3x3");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_matrix3x3_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, CTX_METATABLE_NAME);
    st_lua_set_cfunction_to_field(lua_state, "__gc", st_matrix3x3_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_matrix3x3_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "custom",
     st_matrix3x3_custom_bind);
    st_lua_set_cfunction_to_field(lua_state, "identity",
     st_matrix3x3_identity_bind);
    st_lua_set_cfunction_to_field(lua_state, "translation",
     st_matrix3x3_translation_bind);
    st_lua_set_cfunction_to_field(lua_state, "scaling",
     st_matrix3x3_scaling_bind);
    st_lua_set_cfunction_to_field(lua_state, "rrotation",
     st_matrix3x3_rrotation_bind);
    st_lua_set_cfunction_to_field(lua_state, "drotation",
     st_matrix3x3_drotation_bind);
    st_lua_set_cfunction_to_field(lua_state, "rhshearing",
     st_matrix3x3_rhshearing_bind);
    st_lua_set_cfunction_to_field(lua_state, "dhshearing",
     st_matrix3x3_dhshearing_bind);
    st_lua_set_cfunction_to_field(lua_state, "rvshearing",
     st_matrix3x3_rvshearing_bind);
    st_lua_set_cfunction_to_field(lua_state, "dvshearing",
     st_matrix3x3_dvshearing_bind);
    st_lua_pop(lua_state, 1);

    st_lua_create_metatable(lua_state, MATRIX_METATABLE_NAME);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "clone", st_matrix3x3_clone_bind);
    st_lua_set_cfunction_to_field(lua_state, "apply", st_matrix3x3_apply_bind);
    st_lua_set_cfunction_to_field(lua_state, "translate",
     st_matrix3x3_translate_bind);
    st_lua_set_cfunction_to_field(lua_state, "scale", st_matrix3x3_scale_bind);
    st_lua_set_cfunction_to_field(lua_state, "rrotate",
     st_matrix3x3_rrotate_bind);
    st_lua_set_cfunction_to_field(lua_state, "drotate",
     st_matrix3x3_drotate_bind);
    st_lua_set_cfunction_to_field(lua_state, "rhshear",
     st_matrix3x3_rhshear_bind);
    st_lua_set_cfunction_to_field(lua_state, "dhshear",
     st_matrix3x3_dhshear_bind);
    st_lua_set_cfunction_to_field(lua_state, "rvshear",
     st_matrix3x3_rvshear_bind);
    st_lua_set_cfunction_to_field(lua_state, "dvshear",
     st_matrix3x3_dvshear_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_data",
     st_matrix3x3_get_data_bind);
    st_lua_pop(lua_state, 1);
}
