#include "simple.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(angle_simple)
ST_MODULE_DEF_INIT_FUNC(angle_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_angle_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_angle_import_functions(st_modctx_t *angle_ctx,
 st_modctx_t *logger_ctx) {
    st_angle_simple_t *module = angle_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "angle_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("angle_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("angle_simple", logger, info);

    return true;
}

static st_modctx_t *st_angle_init(st_modctx_t *logger_ctx) {
    st_modctx_t       *angle_ctx;
    st_angle_simple_t *module;

    angle_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_angle_simple_data, sizeof(st_angle_simple_t));

    if (!angle_ctx)
        return NULL;

    angle_ctx->funcs = &st_angle_simple_funcs;

    module = angle_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_angle_import_functions(angle_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, angle_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "%s",
     "angle_simple: Angle utilities module initialized.");

    return angle_ctx;
}

static void st_angle_quit(st_modctx_t *angle_ctx) {
    st_angle_simple_t *module = angle_ctx->data;

    module->logger.info(module->logger.ctx, "%s",
     "angle_simple: Angle utilities module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, angle_ctx);
}

static float st_angle_rtod(__attribute__((unused)) st_modctx_t *angle_ctx,
 float radians) {
    return radians * 180 / (float)M_PI;
}

static float st_angle_dtor(__attribute__((unused)) st_modctx_t *angle_ctx,
 float degrees) {
    return degrees * (float)M_PI / 180;
}

static void st_angle_rnormalize360(
 __attribute__((unused)) st_modctx_t *angle_ctx, float *radians) {
    *radians = fmodf(*radians, 2.0f * (float)M_PI);
    if (*radians < 0.0f)
        *radians += 2.0f * (float)M_PI;
}

static float st_angle_rnormalized360(st_modctx_t *angle_ctx, float radians) {
    st_angle_rnormalize360(angle_ctx, &radians);

    return radians;
}

static void st_angle_dnormalize360(
 __attribute__((unused)) st_modctx_t *angle_ctx, float *degrees) {
    *degrees = fmodf(*degrees, 360.0f);
    if (*degrees < 0.0f)
        *degrees += 360.0f;
}

static float st_angle_dnormalized360(st_modctx_t *angle_ctx, float degrees) {
    st_angle_dnormalize360(angle_ctx, &degrees);

    return degrees;
}

static float st_angle_rdsin(__attribute__((unused)) st_modctx_t *angle_ctx,
 float radians) {
    return sinf(radians);
}

static float st_angle_dgsin(st_modctx_t *angle_ctx, float degrees) {
    return sinf(st_angle_dtor(angle_ctx, degrees));
}

static float st_angle_rdcos(__attribute__((unused)) st_modctx_t *angle_ctx,
 float radians) {
    return cosf(radians);
}

static float st_angle_dgcos(st_modctx_t *angle_ctx, float degrees) {
    return cosf(st_angle_dtor(angle_ctx, degrees));
}

static float st_angle_rdtan(__attribute__((unused)) st_modctx_t *angle_ctx,
 float radians) {
    return tanf(radians);
}

static float st_angle_dgtan(st_modctx_t *angle_ctx, float degrees) {
    return tanf(st_angle_dtor(angle_ctx, degrees));
}

static float st_angle_rdacos(__attribute__((unused)) st_modctx_t *angle_ctx,
 float angle_cos) {
    return acosf(angle_cos);
}

static float st_angle_dgacos(st_modctx_t *angle_ctx, float angle_cos) {
    return st_angle_rtod(angle_ctx, acosf(angle_cos));
}

