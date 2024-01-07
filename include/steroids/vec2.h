#ifndef ST_STEROIDS_VEC2_H
#define ST_STEROIDS_VEC2_H

#include "steroids/module.h"
#include "steroids/types/modules/vec2.h"

static st_modctx_t *st_vec2_init(st_modctx_t *logger_ctx,
 st_modctx_t *angle_ctx);
static void st_vec2_quit(st_modctx_t *vec2_ctx);

static void st_vec2_add(st_modctx_t *vec2_ctx, float *vec_x, float *vec_y,
 float add_x, float add_y);
static void st_vec2_sum(st_modctx_t *vec2_ctx, float *sum_x, float *sum_y,
 float first_x, float first_y, float second_x, float second_y);
static void st_vec2_sub(st_modctx_t *vec2_ctx, float *vec_x, float *vec_y,
 float sub_x, float sub_y);
static void st_vec2_diff(st_modctx_t *vec2_ctx, float *diff_x, float *diff_y,
 float first_x, float first_y, float second_x, float second_y);
static void st_vec2_mul(st_modctx_t *vec2_ctx, float *x, float *y,
 float scalar);
static void st_vec2_product(st_modctx_t *vec2_ctx, float *product_x,
 float *product_y, float x, float y, float scalar);
static float st_vec2_len(st_modctx_t *vec2_ctx, float x, float y);
static float st_vec2_distance(st_modctx_t *vec2_ctx, float first_x,
 float first_y, float second_x, float second_y);
static void st_vec2_normalize(st_modctx_t *vec2_ctx, float *x, float *y);
static void st_vec2_unit(st_modctx_t *vec2_ctx, float *unit_x, float *unit_y,
 float x, float y);
static float st_vec2_dot_product(st_modctx_t *vec2_ctx, float first_x,
 float first_y, float second_x, float second_y);
static float st_vec2_rangle(st_modctx_t *vec2_ctx, float first_x, float first_y,
 float second_x, float second_y);
static float st_vec2_dangle(st_modctx_t *vec2_ctx, float first_x, float first_y,
 float second_x, float second_y);
static void st_vec2_rrotate(st_modctx_t *vec2_ctx, float *x, float *y,
 float radians);
static void st_vec2_rrotation(st_modctx_t *vec2_ctx, float *dst_x, float *dst_y,
 float src_x, float src_y, float radians);
static void st_vec2_drotate(st_modctx_t *vec2_ctx, float *x, float *y,
 float degrees);
static void st_vec2_drotation(st_modctx_t *vec2_ctx, float *dst_x, float *dst_y,
 float src_x, float src_y, float degrees);
static void st_vec2_rotate90(st_modctx_t *vec2_ctx, float *x, float *y);
static void st_vec2_rotation90(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y);
static void st_vec2_rotate180(st_modctx_t *vec2_ctx, float *x, float *y);
static void st_vec2_rotation180(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y);
static void st_vec2_rotate270(st_modctx_t *vec2_ctx, float *x, float *y);
static void st_vec2_rotation270(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y);
static void st_vec2_default_basis_xvec(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y);
static void st_vec2_default_basis_yvec(st_modctx_t *vec2_ctx, float *dst_x,
 float *dst_y);

#endif /* ST_STEROIDS_VEC2_H */
