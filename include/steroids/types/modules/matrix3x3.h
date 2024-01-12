#pragma once

#include "steroids/module.h"

#ifndef ST_MATRIX3X3_T_DEFINED
    typedef struct {
        float       private[6];
        const void *private2;
    } st_matrix3x3_t;
#endif

typedef st_modctx_t *(*st_matrix3x3_init_t)(st_modctx_t *logger_ctx,
 st_modctx_t *angle_ctx);
typedef void (*st_matrix3x3_quit_t)(st_modctx_t *matrix3x3_ctx);

typedef void (*st_matrix3x3_clone_t)(st_matrix3x3_t *dst,
 const st_matrix3x3_t *matrix);
typedef void (*st_matrix3x3_custom_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float r1c1, float r1c2, float r1c3, float r2c1,
 float r2c2, float r2c3);
typedef void (*st_matrix3x3_identity_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix);
typedef void (*st_matrix3x3_translation_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float x, float y);
typedef void (*st_matrix3x3_scaling_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float hscale, float vscale);
typedef void (*st_matrix3x3_rrotation_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_drotation_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_rhshearing_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_dhshearing_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_rvshearing_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_dvshearing_t)(const st_modctx_t *matrix3x3_ctx,
 st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_apply_t)(st_matrix3x3_t *matrix,
 const st_matrix3x3_t *other);
typedef void (*st_matrix3x3_translate_t)(st_matrix3x3_t *matrix, float x,
 float y);
typedef void (*st_matrix3x3_scale_t)(st_matrix3x3_t *matrix, float hscale,
 float vscale);
typedef void (*st_matrix3x3_rrotate_t)(st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_drotate_t)(st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_rhshear_t)(st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_dhshear_t)(st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_rvshear_t)(st_matrix3x3_t *matrix, float radians);
typedef void (*st_matrix3x3_dvshear_t)(st_matrix3x3_t *matrix, float degrees);
typedef void (*st_matrix3x3_get_data_t)(float *r1c1, float *r1c2, float *r1c3,
 float *r2c1, float *r2c2, float *r2c3, const st_matrix3x3_t *matrix);

typedef struct {
    st_matrix3x3_init_t        matrix3x3_init;
    st_matrix3x3_quit_t        matrix3x3_quit;
    st_matrix3x3_clone_t       matrix3x3_clone;
    st_matrix3x3_custom_t      matrix3x3_custom;
    st_matrix3x3_identity_t    matrix3x3_identity;
    st_matrix3x3_translation_t matrix3x3_translation;
    st_matrix3x3_scaling_t     matrix3x3_scaling;
    st_matrix3x3_rrotation_t   matrix3x3_rrotation;
    st_matrix3x3_drotation_t   matrix3x3_drotation;
    st_matrix3x3_rhshearing_t  matrix3x3_rhshearing;
    st_matrix3x3_dhshearing_t  matrix3x3_dhshearing;
    st_matrix3x3_rvshearing_t  matrix3x3_rvshearing;
    st_matrix3x3_dvshearing_t  matrix3x3_dvshearing;
    st_matrix3x3_apply_t       matrix3x3_apply;
    st_matrix3x3_translate_t   matrix3x3_translate;
    st_matrix3x3_scale_t       matrix3x3_scale;
    st_matrix3x3_rrotate_t     matrix3x3_rrotate;
    st_matrix3x3_drotate_t     matrix3x3_drotate;
    st_matrix3x3_rhshear_t     matrix3x3_rhshear;
    st_matrix3x3_dhshear_t     matrix3x3_dhshear;
    st_matrix3x3_rvshear_t     matrix3x3_rvshear;
    st_matrix3x3_dvshear_t     matrix3x3_dvshear;
    st_matrix3x3_get_data_t    matrix3x3_get_data;
} st_matrix3x3_funcs_t;
