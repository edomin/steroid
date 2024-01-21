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

ST_MODULE_DEF_GET_FUNC(matrix3x3_simple)
ST_MODULE_DEF_INIT_FUNC(matrix3x3_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_matrix3x3_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_matrix3x3_import_functions(st_modctx_t *matrix3x3_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *angle_ctx) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "matrix3x3_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", angle, dtor);
    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", angle, rnormalize360);
    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", angle, rdsin);
    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", angle, rdcos);
    ST_LOAD_FUNCTION_FROM_CTX("matrix3x3_simple", angle, rdtan);

    return true;
}

static st_modctx_t *st_matrix3x3_init(st_modctx_t *logger_ctx,
 st_modctx_t *angle_ctx) {
    st_modctx_t           *matrix3x3_ctx;
    st_matrix3x3_simple_t *module;

    matrix3x3_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_matrix3x3_simple_data, sizeof(st_matrix3x3_simple_t));

    if (!matrix3x3_ctx)
        return NULL;

    matrix3x3_ctx->funcs = &st_matrix3x3_simple_funcs;

    module = matrix3x3_ctx->data;
    module->logger.ctx = logger_ctx;
    module->angle.ctx = angle_ctx;

    if (!st_matrix3x3_import_functions(matrix3x3_ctx, logger_ctx, angle_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, matrix3x3_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "%s",
     "matrix3x3_simple: Matrix 3x3 utilities module initialized.");

    return matrix3x3_ctx;
}

static void st_matrix3x3_quit(st_modctx_t *matrix3x3_ctx) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    module->logger.info(module->logger.ctx, "%s",
     "matrix3x3_simple: Matrix 3x3 utilities module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, matrix3x3_ctx);
}

static void st_matrix3x3_clone(st_matrix3x3_t *dst,
 const st_matrix3x3_t *matrix) {
    dst->ctx = matrix->ctx;

    dst->r1c1 = matrix->r1c1;
    dst->r1c2 = matrix->r1c2;
    dst->r1c3 = matrix->r1c3;

    dst->r2c1 = matrix->r2c1;
    dst->r2c2 = matrix->r2c2;
    dst->r2c3 = matrix->r2c3;
}

static void st_matrix3x3_custom(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float r1c1, float r1c2, float r1c3, float r2c1,
 float r2c2, float r2c3) {
    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = r1c1;
    matrix->r1c2 = r1c2;
    matrix->r1c3 = r1c3;

    matrix->r2c1 = r2c1;
    matrix->r2c2 = r2c2;
    matrix->r2c3 = r2c3;
}

static void st_matrix3x3_identity(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix) {
    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = 1;
    matrix->r1c2 = 0;
    matrix->r1c3 = 0;

    matrix->r2c1 = 0;
    matrix->r2c2 = 1;
    matrix->r2c3 = 0;
}

static void st_matrix3x3_translation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float x, float y) {
    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = 1;
    matrix->r1c2 = 0;
    matrix->r1c3 = x;

    matrix->r2c1 = 0;
    matrix->r2c2 = 1;
    matrix->r2c3 = y;
}

static void st_matrix3x3_scaling(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float hscale, float vscale) {
    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = hscale;
    matrix->r1c2 = 0;
    matrix->r1c3 = 0;

    matrix->r2c1 = 0;
    matrix->r2c2 = vscale;
    matrix->r2c3 = 0;
}

static void st_matrix3x3_rrotation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    module->angle.rnormalize360(module->angle.ctx, &radians);

    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = module->angle.rdcos(module->angle.ctx, -radians);
    matrix->r1c2 = -module->angle.rdsin(module->angle.ctx, -radians);
    matrix->r1c3 = 0;

    matrix->r2c1 = module->angle.rdsin(module->angle.ctx, -radians);
    matrix->r2c2 = module->angle.rdcos(module->angle.ctx, -radians);
    matrix->r2c3 = 0;
}

static void st_matrix3x3_drotation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    st_matrix3x3_rrotation(matrix3x3_ctx, matrix,
     module->angle.dtor(module->angle.ctx, degrees));
}

static void st_matrix3x3_rhshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    module->angle.rnormalize360(module->angle.ctx, &radians);

    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = 1;
    matrix->r1c2 = module->angle.rdtan(module->angle.ctx, radians);
    matrix->r1c3 = 0;

    matrix->r2c1 = 0;
    matrix->r2c2 = 1;
    matrix->r2c3 = 0;
}

static void st_matrix3x3_dhshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    st_matrix3x3_rhshearing(matrix3x3_ctx, matrix,
     module->angle.dtor(module->angle.ctx, degrees));
}

static void st_matrix3x3_rvshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    module->angle.rnormalize360(module->angle.ctx, &radians);

    matrix->ctx = matrix3x3_ctx;

    matrix->r1c1 = 1;
    matrix->r1c2 = 0;
    matrix->r1c3 = 0;

    matrix->r2c1 = module->angle.rdtan(module->angle.ctx, radians);
    matrix->r2c2 = 1;
    matrix->r2c3 = 0;
}

static void st_matrix3x3_dvshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_simple_t *module = matrix3x3_ctx->data;

    st_matrix3x3_rvshearing(matrix3x3_ctx, matrix,
     module->angle.dtor(module->angle.ctx, degrees));
}

static void st_matrix3x3_apply(st_matrix3x3_t *matrix,
 const st_matrix3x3_t *other) {
    st_matrix3x3_t old;

    st_matrix3x3_clone(&old, matrix);

    matrix->ctx = other->ctx;

    matrix->r1c1 = old.r1c1 * other->r1c1
                 + old.r1c2 * other->r2c1;
    matrix->r1c2 = old.r1c1 * other->r1c2
                 + old.r1c2 * other->r2c2;
    matrix->r1c3 = old.r1c1 * other->r1c3
                 + old.r1c2 * other->r2c3
                 + old.r1c3;

    matrix->r2c1 = old.r2c1 * other->r1c1
                 + old.r2c2 * other->r2c1;
    matrix->r2c2 = old.r2c1 * other->r1c2
                 + old.r2c2 * other->r2c2;
    matrix->r2c3 = old.r2c1 * other->r1c3
                 + old.r2c2 * other->r2c3
                 + old.r2c3;
}

static void st_matrix3x3_translate(st_matrix3x3_t *matrix, float x, float y) {
    st_matrix3x3_t other;

    st_matrix3x3_translation(matrix->ctx, &other, x, y);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_scale(st_matrix3x3_t *matrix, float hscale,
 float vscale) {
    st_matrix3x3_t other;

    st_matrix3x3_scaling(matrix->ctx, &other, hscale, vscale);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_rrotate(st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_t other;

    st_matrix3x3_rrotation(matrix->ctx, &other, radians);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_drotate(st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_t other;

    st_matrix3x3_drotation(matrix->ctx, &other, degrees);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_rhshear(st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_t other;

    st_matrix3x3_rhshearing(matrix->ctx, &other, radians);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_dhshear(st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_t other;

    st_matrix3x3_dhshearing(matrix->ctx, &other, degrees);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_rvshear(st_matrix3x3_t *matrix, float radians) {
    st_matrix3x3_t other;

    st_matrix3x3_rvshearing(matrix->ctx, &other, radians);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_dvshear(st_matrix3x3_t *matrix, float degrees) {
    st_matrix3x3_t other;

    st_matrix3x3_dvshearing(matrix->ctx, &other, degrees);
    st_matrix3x3_apply(matrix, &other);
}

static void st_matrix3x3_get_data(float *r1c1, float *r1c2, float *r1c3,
 float *r2c1, float *r2c2, float *r2c3, const st_matrix3x3_t *matrix) {
    if (r1c1)
        *r1c1 = matrix->r1c1;
    if (r1c2)
        *r1c2 = matrix->r1c2;
    if (r1c3)
        *r1c3 = matrix->r1c3;
    if (r2c1)
        *r2c1 = matrix->r2c1;
    if (r2c2)
        *r2c2 = matrix->r2c2;
    if (r2c3)
        *r2c3 = matrix->r2c3;
}
