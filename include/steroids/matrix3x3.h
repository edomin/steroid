#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/matrix3x3.h"

static st_modctx_t *st_matrix3x3_init(st_modctx_t *logger_ctx,
 st_modctx_t *angle_ctx);
static void st_matrix3x3_quit(st_modctx_t *matrix3x3_ctx);

static void st_matrix3x3_clone(st_matrix3x3_t *dst,
 const st_matrix3x3_t *matrix);
static void st_matrix3x3_custom(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float r1c1, float r1c2, float r1c3, float r2c1,
 float r2c2, float r2c3);
static void st_matrix3x3_identity(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix);
static void st_matrix3x3_translation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float x, float y);
static void st_matrix3x3_scaling(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float hscale, float vscale);
static void st_matrix3x3_rrotation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_drotation(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_rhshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_dhshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_rvshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_dvshearing(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_apply(st_matrix3x3_t *matrix,
 const st_matrix3x3_t *other);
static void st_matrix3x3_translate(st_matrix3x3_t *matrix, float x, float y);
static void st_matrix3x3_scale(st_matrix3x3_t *matrix, float hscale,
 float vscale);
static void st_matrix3x3_rrotate(st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_drotate(st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_rhshear(st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_dhshear(st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_rvshear(st_matrix3x3_t *matrix, float radians);
static void st_matrix3x3_dvshear(st_matrix3x3_t *matrix, float degrees);
static void st_matrix3x3_get_data(float *r1c1, float *r1c2, float *r1c3,
 float *r2c1, float *r2c2, float *r2c3, const st_matrix3x3_t *matrix);