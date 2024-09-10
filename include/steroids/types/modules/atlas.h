#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/texture.h"
#include "steroids/types/object.h"

#ifndef ST_ATLAS_T_DEFINED
    typedef struct st_atlas_s st_atlas_t;
#endif

typedef st_modctx_t *(*st_atlas_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_atlas_quit_t)(st_modctx_t *atlas_ctx);

typedef st_atlas_t *(*st_atlas_create_t)(st_modctx_t *atlas_ctx,
 st_texture_t *texture, size_t clips_count);
typedef void (*st_atlas_destroy_t)(st_atlas_t *atlas);
typedef bool (*st_atlas_set_clip_t)(st_atlas_t *atlas, size_t clip_num,
 unsigned x, unsigned y, unsigned width, unsigned height);
typedef const st_texture_t *(*st_atlas_get_texture_t)(const st_atlas_t *atlas);
typedef size_t (*st_atlas_get_clips_count_t)(const st_atlas_t *atlas);
typedef unsigned (*st_atlas_get_clip_x_t)(const st_atlas_t *atlas,
 size_t clip_num);
typedef unsigned (*st_atlas_get_clip_y_t)(const st_atlas_t *atlas,
 size_t clip_num);
typedef unsigned (*st_atlas_get_clip_width_t)(const st_atlas_t *atlas,
 size_t clip_num);
typedef unsigned (*st_atlas_get_clip_height_t)(const st_atlas_t *atlas,
 size_t clip_num);

typedef struct {
    st_atlas_quit_t   quit;
    st_atlas_create_t create;
} st_atlasctx_funcs_t;

typedef struct {
    st_atlas_destroy_t         destroy;
    st_atlas_set_clip_t        set_clip;
    st_atlas_get_texture_t     get_texture;
    st_atlas_get_clips_count_t get_clips_count;
    st_atlas_get_clip_x_t      get_clip_x;
    st_atlas_get_clip_y_t      get_clip_y;
    st_atlas_get_clip_width_t  get_clip_width;
    st_atlas_get_clip_height_t get_clip_height;
} st_atlas_funcs_t;

#define ST_ATLAS_CALL(object, func, ...) \
    ((st_atlas_funcs_t *)((const st_object_t *)object)->funcs)->func(object, ## __VA_ARGS__)
