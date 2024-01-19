#include "vec2.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024
#define METATABLE_NAME   "vec2_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_vec2_init_t                  st_vec2_init;
static st_vec2_quit_t                  st_vec2_quit;
static st_vec2_sum_t                   st_vec2_sum;
static st_vec2_diff_t                  st_vec2_diff;
static st_vec2_product_t               st_vec2_product;
static st_vec2_len_t                   st_vec2_len;
static st_vec2_distance_t              st_vec2_distance;
static st_vec2_unit_t                  st_vec2_unit;
static st_vec2_dot_product_t           st_vec2_dot_product;
static st_vec2_rangle_t                st_vec2_rangle;
static st_vec2_dangle_t                st_vec2_dangle;
static st_vec2_rrotation_t             st_vec2_rrotation;
static st_vec2_drotation_t             st_vec2_drotation;
static st_vec2_rotation90_t            st_vec2_rotation90;
static st_vec2_rotation180_t           st_vec2_rotation180;
static st_vec2_rotation270_t           st_vec2_rotation270;
static st_vec2_applying_matrix3x3_t    st_vec2_applying_matrix3x3;
static st_vec2_default_basis_xvec_t    st_vec2_default_basis_xvec;
static st_vec2_default_basis_yvec_t    st_vec2_default_basis_yvec;

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

ST_MODULE_DEF_GET_FUNC(luabind_vec2)
ST_MODULE_DEF_INIT_FUNC(luabind_vec2)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_vec2_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_vec2_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_vec2: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_vec2", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_vec2", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, sum);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, diff);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, product);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, len);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, distance);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, unit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, dot_product);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, rangle);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, dangle);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, rrotation);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, drotation);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, rotation90);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, rotation180);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, rotation270);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, applying_matrix3x3);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, default_basis_xvec);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", vec2, default_basis_yvec);

    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, push_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, get_double);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_vec2", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t       *luabind_ctx;
    st_luabind_vec2_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_vec2_data, sizeof(st_luabind_vec2_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_vec2_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_vec2: 2D Vector funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_vec2_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_vec2: 2D Vector funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_vec2_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "logger_ctx");
    st_modctx_t *angle_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "angle_ctx");

    *(st_modctx_t **)userdata = st_vec2_init(logger_ctx, angle_ctx);
    st_lua_set_metatable(lua_state, METATABLE_NAME);

    return 1;
}

static int st_vec2_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);

    st_vec2_quit(vec2_ctx);

    return 0;
}

static int st_vec2_sum_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        sum_x;
    float        sum_y;

    st_vec2_sum(vec2_ctx, &sum_x, &sum_y, (float)first_x, (float)first_y,
     (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, sum_x);
    st_lua_push_double(lua_state, sum_x);

    return 2;
}

static int st_vec2_diff_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        diff_x;
    float        diff_y;

    st_vec2_diff(vec2_ctx, &diff_x, &diff_y, (float)first_x, (float)first_y,
     (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, diff_x);
    st_lua_push_double(lua_state, diff_y);

    return 2;
}

static int st_vec2_product_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       scalar = st_lua_get_double(lua_state, 4);
    float        product_x;
    float        product_y;

    st_vec2_product(vec2_ctx, &product_x, &product_y, (float)first_x,
     (float)first_y, (float)scalar);

    st_lua_push_double(lua_state, product_x);
    st_lua_push_double(lua_state, product_y);

    return 2;
}

static int st_vec2_len_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       x = st_lua_get_double(lua_state, 2);
    double       y = st_lua_get_double(lua_state, 3);

    st_lua_push_double(lua_state,
     st_vec2_len(vec2_ctx, (float)x, (float)y));

    return 1;
}

static int st_vec2_distance_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        distance = st_vec2_distance(vec2_ctx, (float)first_x,
     (float)first_y, (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, distance);

    return 1;
}

static int st_vec2_unit_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       x = st_lua_get_double(lua_state, 2);
    double       y = st_lua_get_double(lua_state, 3);
    float        unit_x;
    float        unit_y;

    st_vec2_unit(vec2_ctx, &unit_x, &unit_y, (float)x, (float)y);

    st_lua_push_double(lua_state, unit_x);
    st_lua_push_double(lua_state, unit_x);

    return 2;
}

static int st_vec2_dot_product_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        dot_product = st_vec2_dot_product(vec2_ctx, (float)first_x,
     (float)first_y, (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, dot_product);

    return 1;
}

static int st_vec2_rangle_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        radians = st_vec2_rangle(vec2_ctx, (float)first_x,
     (float)first_y, (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, radians);

    return 1;
}

static int st_vec2_dangle_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       first_x = st_lua_get_double(lua_state, 2);
    double       first_y = st_lua_get_double(lua_state, 3);
    double       second_x = st_lua_get_double(lua_state, 4);
    double       second_y = st_lua_get_double(lua_state, 5);
    float        degrees = st_vec2_dangle(vec2_ctx, (float)first_x,
     (float)first_y, (float)second_x, (float)second_y);

    st_lua_push_double(lua_state, degrees);

    return 1;
}

static int st_vec2_rrotation_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       src_x = st_lua_get_double(lua_state, 2);
    double       src_y = st_lua_get_double(lua_state, 3);
    double       radians = st_lua_get_double(lua_state, 4);
    float        dst_x;
    float        dst_y;

    st_vec2_rrotation(vec2_ctx, &dst_x, &dst_y, (float)src_x, (float)src_y,
     (float)radians);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_drotation_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       src_x = st_lua_get_double(lua_state, 2);
    double       src_y = st_lua_get_double(lua_state, 3);
    double       degrees = st_lua_get_double(lua_state, 4);
    float        dst_x;
    float        dst_y;

    st_vec2_drotation(vec2_ctx, &dst_x, &dst_y, (float)src_x, (float)src_y,
     (float)degrees);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_rotation90_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       src_x = st_lua_get_double(lua_state, 2);
    double       src_y = st_lua_get_double(lua_state, 3);
    float        dst_x;
    float        dst_y;

    st_vec2_rotation90(vec2_ctx, &dst_x, &dst_y, (float)src_x, (float)src_y);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_rotation180_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       src_x = st_lua_get_double(lua_state, 2);
    double       src_y = st_lua_get_double(lua_state, 3);
    float        dst_x;
    float        dst_y;

    st_vec2_rotation180(vec2_ctx, &dst_x, &dst_y, (float)src_x, (float)src_y);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_rotation270_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double       src_x = st_lua_get_double(lua_state, 2);
    double       src_y = st_lua_get_double(lua_state, 3);
    float        dst_x;
    float        dst_y;

    st_vec2_rotation270(vec2_ctx, &dst_x, &dst_y, (float)src_x, (float)src_y);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_applying_matrix3x3_bind(st_luastate_t *lua_state) {
    st_modctx_t    *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    double          src_x = st_lua_get_double(lua_state, 2);
    double          src_y = st_lua_get_double(lua_state, 3);
    st_matrix3x3_t *matrix = (st_matrix3x3_t *)st_lua_get_named_userdata(
     lua_state, 4, "matrix3x3");
    float           dst_x;
    float           dst_y;

    st_vec2_applying_matrix3x3(vec2_ctx, &dst_x, &dst_y, (float)src_x,
     (float)src_y, matrix);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_default_basis_xvec_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    float        dst_x;
    float        dst_y;

    st_vec2_default_basis_xvec(vec2_ctx, &dst_x, &dst_y);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static int st_vec2_default_basis_yvec_bind(st_luastate_t *lua_state) {
    st_modctx_t *vec2_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, METATABLE_NAME);
    float        dst_x;
    float        dst_y;

    st_vec2_default_basis_yvec(vec2_ctx, &dst_x, &dst_y);

    st_lua_push_double(lua_state, dst_x);
    st_lua_push_double(lua_state, dst_y);

    return 2;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_vec2_t *module = luabind_ctx->data;
    st_luastate_t     *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Vec2");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_vec2_init_bind);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_vec2_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_vec2_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);

    st_lua_set_cfunction_to_field(lua_state, "sum", st_vec2_sum_bind);
    st_lua_set_cfunction_to_field(lua_state, "diff", st_vec2_diff_bind);
    st_lua_set_cfunction_to_field(lua_state, "product", st_vec2_product_bind);
    st_lua_set_cfunction_to_field(lua_state, "len", st_vec2_len_bind);
    st_lua_set_cfunction_to_field(lua_state, "distance", st_vec2_distance_bind);
    st_lua_set_cfunction_to_field(lua_state, "unit", st_vec2_unit_bind);
    st_lua_set_cfunction_to_field(lua_state, "dot_product",
     st_vec2_dot_product_bind);
    st_lua_set_cfunction_to_field(lua_state, "rangle", st_vec2_rangle_bind);
    st_lua_set_cfunction_to_field(lua_state, "dangle", st_vec2_dangle_bind);
    st_lua_set_cfunction_to_field(lua_state, "rrotation",
     st_vec2_rrotation_bind);
    st_lua_set_cfunction_to_field(lua_state, "drotation",
     st_vec2_drotation_bind);
    st_lua_set_cfunction_to_field(lua_state, "rotation90",
     st_vec2_rotation90_bind);
    st_lua_set_cfunction_to_field(lua_state, "rotation180",
     st_vec2_rotation180_bind);
    st_lua_set_cfunction_to_field(lua_state, "rotation270",
     st_vec2_rotation270_bind);
    st_lua_set_cfunction_to_field(lua_state, "applying_matrix3x3",
     st_vec2_applying_matrix3x3_bind);
    st_lua_set_cfunction_to_field(lua_state, "default_basis_xvec",
     st_vec2_default_basis_xvec_bind);
    st_lua_set_cfunction_to_field(lua_state, "default_basis_yvec",
     st_vec2_default_basis_yvec_bind);

    st_lua_pop(lua_state, 1);
}
