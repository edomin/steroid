#ifndef ST_STEROIDS_TYPES_MODULES_ATLAS_H
#define ST_STEROIDS_TYPES_MODULES_ATLAS_H

#include "steroids/module.h"
#include "steroids/types/modules/texture.h"

#ifndef ST_ATLAS_T_DEFINED
    typedef struct st_atlas_s st_atlas_t;
#endif

typedef st_modctx_t *(*st_atlas_init_t)(st_modctx_t *logger_ctx,
 st_modctx_t *texture_ctx);
typedef void (*st_atlas_quit_t)(st_modctx_t *atlas_ctx);

typedef st_atlas_t *(*st_atlas_create_t)(st_modctx_t *atlas_ctx,
 st_texture_t *texture, size_t clips_count);
typedef void (*st_atlas_destroy_t)(st_atlas_t *atlas);
typedef bool (*st_atlas_set_clip_t)(st_atlas_t *atlas, size_t clip_num,
 unsigned x, unsigned y, unsigned width, unsigned height);
typedef st_texture_t *(*st_atlas_get_texture_t)(st_atlas_t *atlas);
typedef size_t (*st_atlas_get_clips_count_t)(st_atlas_t *atlas);
typedef unsigned (*st_atlas_get_clip_x_t)(st_atlas_t *atlas, size_t clip_num);
typedef unsigned (*st_atlas_get_clip_y_t)(st_atlas_t *atlas, size_t clip_num);
typedef unsigned (*st_atlas_get_clip_width_t)(st_atlas_t *atlas,
 size_t clip_num);
typedef unsigned (*st_atlas_get_clip_height_t)(st_atlas_t *atlas,
 size_t clip_num);

typedef struct {
    st_atlas_init_t            atlas_init;
    st_atlas_quit_t            atlas_quit;
    st_atlas_create_t          atlas_create;
    st_atlas_destroy_t         atlas_destroy;
    st_atlas_set_clip_t        atlas_set_clip;
    st_atlas_get_texture_t     atlas_get_texture;
    st_atlas_get_clips_count_t atlas_get_clips_count;
    st_atlas_get_clip_x_t      atlas_get_clip_x;
    st_atlas_get_clip_y_t      atlas_get_clip_y;
    st_atlas_get_clip_width_t  atlas_get_clip_width;
    st_atlas_get_clip_height_t atlas_get_clip_height;
} st_atlas_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_ATLAS_H */
