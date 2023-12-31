#ifndef ST_STEROIDS_VECTOR_H
#define ST_STEROIDS_VECTOR_H

#include "steroids/module.h"
#include "steroids/types/modules/vector.h"

static st_modctx_t *st_vector_init(st_modctx_t *logger_ctx);
static void st_vector_quit(st_modctx_t *vector_ctx);

static void st_vector_rrotate(st_modctx_t *vector_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y, float radians, float pivot_x,
 float pivot_y);
static void st_vector_drotate(st_modctx_t *vector_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y, float degrees, float pivot_x,
 float pivot_y);

#endif /* ST_STEROIDS_VECTOR_H */
