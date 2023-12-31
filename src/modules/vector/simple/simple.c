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

ST_MODULE_DEF_GET_FUNC(vector_simple)
ST_MODULE_DEF_INIT_FUNC(vector_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_vector_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_vector_import_functions(st_modctx_t *vector_ctx,
 st_modctx_t *logger_ctx) {
    st_vector_simple_t *module = vector_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "vector_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("vector_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("vector_simple", logger, info);

    return true;
}

static st_modctx_t *st_vector_init(st_modctx_t *logger_ctx) {
    st_modctx_t        *vector_ctx;
    st_vector_simple_t *vector;

    vector_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_vector_simple_data, sizeof(st_vector_simple_t));

    if (!vector_ctx)
        return NULL;

    vector_ctx->funcs = &st_vector_simple_funcs;

    vector = vector_ctx->data;
    vector->logger.ctx = logger_ctx;

    if (!st_vector_import_functions(vector_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, vector_ctx);

        return NULL;
    }

    vector->logger.info(vector->logger.ctx, "%s",
     "vector_simple: Vector utilities module initialized.");

    return vector_ctx;
}

static void st_vector_quit(st_modctx_t *vector_ctx) {
    st_vector_simple_t *vector = vector_ctx->data;

    vector->logger.info(vector->logger.ctx, "%s",
     "vector_simple: Vector utilities module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, vector_ctx);
}

static void st_vector_rrotate(__attribute__((unused)) st_modctx_t *vector_ctx,
 float *dst_x, float *dst_y, float src_x, float src_y, float radians,
 float pivot_x, float pivot_y) {
    radians = fmodf(radians, 2.0f * (float)M_PI);
    if (radians < 0.0f)
        radians += 2.0f * (float)M_PI;

    *dst_x = (pivot_x
     + (src_x - pivot_x) * cosf(radians)
     - ((src_y - pivot_y) * sinf(radians)));
    *dst_y = (pivot_y
     + (src_x - pivot_x) * sinf(radians)
     + ((src_y - pivot_y) * cosf(radians)));
}

static void st_vector_drotate(__attribute__((unused)) st_modctx_t *vector_ctx,
 float *dst_x, float *dst_y, float src_x, float src_y, float degrees,
 float pivot_x, float pivot_y) {
    return st_vector_rrotate(vector_ctx, dst_x, dst_y, src_x, src_y,
     degrees * (float)M_PI / 180.0f, pivot_x, pivot_y);
}
