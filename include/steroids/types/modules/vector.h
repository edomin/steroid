#ifndef ST_STEROIDS_TYPES_MODULES_VECTOR_H
#define ST_STEROIDS_TYPES_MODULES_VECTOR_H

#include "steroids/module.h"

typedef st_modctx_t *(*st_vector_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_vector_quit_t)(st_modctx_t *vector_ctx);
typedef void (*st_vector_rrotate_t)(st_modctx_t *vector_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y, float radians, float pivot_x,
 float pivot_y);
typedef void (*st_vector_drotate_t)(st_modctx_t *vector_ctx, float *dst_x,
 float *dst_y, float src_x, float src_y, float degrees, float pivot_x,
 float pivot_y);

typedef struct {
    st_vector_init_t    vector_init;
    st_vector_quit_t    vector_quit;
    st_vector_rrotate_t vector_rrotate;
    st_vector_drotate_t vector_drotate;
} st_vector_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_VECTOR_H */
