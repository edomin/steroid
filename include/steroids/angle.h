#ifndef ST_STEROIDS_ANGLE_H
#define ST_STEROIDS_ANGLE_H

#include "steroids/module.h"
#include "steroids/types/modules/angle.h"

static st_modctx_t *st_angle_init(st_modctx_t *logger_ctx);
static void st_angle_quit(st_modctx_t *angle_ctx);

static float st_angle_rtod(st_modctx_t *angle_ctx, float radians);
static float st_angle_dtor(st_modctx_t *angle_ctx, float degrees);
static void st_angle_rnormalize360(st_modctx_t *angle_ctx, float *radians);
static float st_angle_rnormalized360(st_modctx_t *angle_ctx, float radians);
static void st_angle_dnormalize360(st_modctx_t *angle_ctx, float *degrees);
static float st_angle_dnormalized360(st_modctx_t *angle_ctx, float degrees);
static float st_angle_rdsin(st_modctx_t *angle_ctx, float radians);
static float st_angle_dgsin(st_modctx_t *angle_ctx, float degrees);
static float st_angle_rdcos(st_modctx_t *angle_ctx, float radians);
static float st_angle_dgcos(st_modctx_t *angle_ctx, float degrees);
static float st_angle_rdacos(st_modctx_t *angle_ctx, float angle_cos);
static float st_angle_dgacos(st_modctx_t *angle_ctx, float angle_cos);

#endif /* ST_STEROIDS_ANGLE_H */
