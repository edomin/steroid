#include "simple.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(vec2_simple)
ST_MODULE_DEF_INIT_FUNC(vec2_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_vec2_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_vec2_import_functions(st_modctx_t *vec2_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *angle_ctx) {
    st_vec2_simple_t *module = vec2_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "vec2_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, rtod);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, dtor);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, rnormalize360);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, rdsin);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, rdcos);
    ST_LOAD_FUNCTION_FROM_CTX("vec2_simple", angle, rdacos);

    return true;
}

static st_modctx_t *st_vec2_init(st_modctx_t *logger_ctx,
 st_modctx_t *angle_ctx) {
    st_modctx_t      *vec2_ctx;
    st_vec2_simple_t *module;

    vec2_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_vec2_simple_data, sizeof(st_vec2_simple_t));

    if (!vec2_ctx)
        return NULL;

    vec2_ctx->funcs = &st_vec2_simple_funcs;

    module = vec2_ctx->data;
    module->logger.ctx = logger_ctx;
    module->angle.ctx = angle_ctx;

    if (!st_vec2_import_functions(vec2_ctx, logger_ctx, angle_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, vec2_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "%s",
     "vec2_simple: 2D Vector utilities module initialized.");

    return vec2_ctx;
}

static void st_vec2_quit(st_modctx_t *vec2_ctx) {
    st_vec2_simple_t *module = vec2_ctx->data;

    module->logger.info(module->logger.ctx, "%s",
     "vec2_simple: 2D Vector utilities module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, vec2_ctx);
}

static void st_vec2_add(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *vec_x, float *vec_y, float add_x, float add_y) {
    *vec_x += add_x;
    *vec_y += add_y;
}

static void st_vec2_sum(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *sum_x, float *sum_y, float first_x, float first_y, float second_x,
 float second_y) {
    *sum_x = first_x + second_x;
    *sum_y = first_y + second_y;
}

static void st_vec2_sub(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *vec_x, float *vec_y, float sub_x, float sub_y) {
    *vec_x -= sub_x;
    *vec_y -= sub_y;
}

static void st_vec2_diff(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *diff_x, float *diff_y, float first_x, float first_y, float second_x,
 float second_y) {
    *diff_x = first_x - second_x;
    *diff_y = first_y - second_y;
}

static void st_vec2_mul(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *x, float *y, float scalar) {
    *x *= scalar;
    *y *= scalar;
}

static void st_vec2_product(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *product_x, float *product_y, float x, float y, float scalar) {
    *product_x = x * scalar;
    *product_y = y * scalar;
}

static float st_vec2_len(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float x, float y) {
    return sqrtf(x * x + y * y);
}

static float st_vec2_distance(st_modctx_t *vec2_ctx, float first_x,
 float first_y, float second_x, float second_y) {
    float diff_x;
    float diff_y;

    st_vec2_diff(vec2_ctx, &diff_x, &diff_y, first_x, first_y, second_x,
     second_y);
    return st_vec2_len(vec2_ctx, diff_x, diff_y);
}

static void st_vec2_normalize(st_modctx_t *vec2_ctx, float *x, float *y) {
    float len = st_vec2_len(vec2_ctx, *x, *y);

    *x /= len;
    *y /= len;
}

static void st_vec2_unit(st_modctx_t *vec2_ctx, float *unit_x, float *unit_y,
 float x, float y) {
    float len = st_vec2_len(vec2_ctx, x, y);

    *unit_x = x / len;
    *unit_y = y / len;
}

static float st_vec2_dot_product(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float first_x, float first_y, float second_x, float second_y) {
    return first_x * second_x + first_y * second_y;
}

static float st_vec2_rangle(st_modctx_t *vec2_ctx, float first_x, float first_y,
 float second_x, float second_y) {
    st_vec2_simple_t *module = vec2_ctx->data;

    float dot_product = st_vec2_dot_product(vec2_ctx, first_x, first_y,
     second_x, second_y);
    float first_len = st_vec2_len(vec2_ctx, first_x, first_y);
    float second_len = st_vec2_len(vec2_ctx, second_x, second_y);

    return module->angle.rdacos(module->angle.ctx,
     dot_product / (first_len * second_len));
}

static float st_vec2_dangle(st_modctx_t *vec2_ctx, float first_x, float first_y,
 float second_x, float second_y) {
    st_vec2_simple_t *module = vec2_ctx->data;

    return module->angle.rtod(module->angle.ctx,
     st_vec2_rangle(vec2_ctx, first_x, first_y, second_x, second_y));
}

static void st_vec2_rrotation(__attribute__((unused)) st_modctx_t *vec2_ctx,
 float *dst_x, float *dst_y, float src_x, float src_y, float radians) {
    st_vec2_simple_t *module = vec2_ctx->data;

    module->angle.rnormalize360(module->angle.ctx, &radians);

    *dst_x = (src_x * module->angle.rdcos(module->angle.ctx, radians)
     - (src_y * module->angle.rdsin(module->angle.ctx, radians)));
    *dst_y = (src_x * module->angle.rdsin(module->angle.ctx, radians)
     + (src_y * module->angle.rdcos(module->angle.ctx, radians)));
}

static void st_vec2_rrotate(st_modctx_t *vec2_ctx, float *x, float *y,
 float radians) {
    float src_x = *x;
    float src_y = *y;

    st_vec2_rrotation(vec2_ctx, x, y, src_x, src_y, radians);
}

static void st_vec2_drotation(st_modctx_t *vec2_ctx, float *dst_x, float *dst_y,
 float src_x, float src_y, float degrees) {
    st_vec2_simple_t *module = vec2_ctx->data;

    return st_vec2_rrotation(vec2_ctx, dst_x, dst_y, src_x, src_y,
     module->angle.dtor(module->angle.ctx, degrees));
}

static void st_vec2_drotate(st_modctx_t *vec2_ctx, float *x, float *y,
 float degrees) {
    float src_x = *x;
    float src_y = *y;

    st_vec2_drotation(vec2_ctx, x, y, src_x, src_y, degrees);
}

static void st_vec2_rotation90(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y) {
    *dst_x = -src_y;
    *dst_y = src_x;
}

static void st_vec2_rotate90(st_modctx_t *vec2_ctx, float *x, float *y) {
    float new_x;
    float new_y;

    st_vec2_rotation90(vec2_ctx, &new_x, &new_y, *x, *y);
    *x = new_x;
    *y = new_y;
}

static void st_vec2_rotation180(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y) {
    *dst_x = -src_x;
    *dst_y = -src_y;
}

static void st_vec2_rotate180(st_modctx_t *vec2_ctx, float *x, float *y) {
    float new_x;
    float new_y;

    st_vec2_rotation180(vec2_ctx, &new_x, &new_y, *x, *y);
    *x = new_x;
    *y = new_y;
}

static void st_vec2_rotation270(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y) {
    *dst_x = src_y;
    *dst_y = -src_x;
}

static void st_vec2_rotate270(st_modctx_t *vec2_ctx, float *x, float *y) {
    float new_x;
    float new_y;

    st_vec2_rotation270(vec2_ctx, &new_x, &new_y, *x, *y);
    *x = new_x;
    *y = new_y;
}

static void st_vec2_applying_matrix3x3(
 __attribute__((unused)) st_modctx_t *vec2_ctx, float *dst_x, float *dst_y,
 float src_x, float src_y, const st_matrix3x3_t *matrix) {
    *dst_x = matrix->r1c1 * src_x + matrix->r1c2 * src_y + matrix->r1c3;
    *dst_y = matrix->r2c1 * src_x + matrix->r2c2 * src_y + matrix->r2c3;
}

static void st_vec2_apply_matrix3x3(st_modctx_t *vec2_ctx, float *x, float *y,
 const st_matrix3x3_t *matrix) {
    float new_x;
    float new_y;

    st_vec2_applying_matrix3x3(vec2_ctx, &new_x, &new_y, *x, *y, matrix);
    *x = new_x;
    *y = new_y;
}

static void st_vec2_default_basis_xvec(
 __attribute__((unused)) st_modctx_t *vec2_ctx, float *dst_x, float *dst_y) {
    *dst_x = 1.0f;
    *dst_y = 0.0f;
}

static void st_vec2_default_basis_yvec(
 __attribute__((unused)) st_modctx_t *vec2_ctx, float *dst_x, float *dst_y) {
    *dst_x = 0.0f;
    *dst_y = 1.0f;
}
