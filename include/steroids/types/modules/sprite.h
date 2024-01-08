#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/atlas.h"
#include "steroids/types/modules/texture.h"

#ifndef ST_SPRITE_T_DEFINED
    typedef struct st_sprite_s st_sprite_t;
#endif

#ifndef ST_UV_T_DEFINED
    typedef struct {
        float u;
        float v;
    } st_uvcoord_t;

    typedef struct {
        st_uvcoord_t upper_left;
        st_uvcoord_t upper_right;
        st_uvcoord_t lower_left;
        st_uvcoord_t lower_right;
    } st_uv_t;
#endif

typedef st_modctx_t *(*st_sprite_init_t)(st_modctx_t *atlas_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *texture_ctx);
typedef void (*st_sprite_quit_t)(st_modctx_t *sprite_ctx);

typedef st_sprite_t *(*st_sprite_create_t)(st_modctx_t *sprite_ctx,
 const st_atlas_t *atlas, size_t clip_num);
typedef st_sprite_t *(*st_sprite_from_texture_t)(st_modctx_t *sprite_ctx,
 const st_texture_t *texture);
typedef void (*st_sprite_destroy_t)(st_sprite_t *sprite);
typedef const st_texture_t *(*st_sprite_get_texture_t)(
 const st_sprite_t *sprite);
typedef unsigned (*st_sprite_get_width_t)(const st_sprite_t *sprite);
typedef unsigned (*st_sprite_get_height_t)(const st_sprite_t *sprite);
typedef void (*st_sprite_export_uv_t)(const st_sprite_t *sprite, st_uv_t *dstuv);

typedef struct {
    st_sprite_init_t         sprite_init;
    st_sprite_quit_t         sprite_quit;
    st_sprite_create_t       sprite_create;
    st_sprite_from_texture_t sprite_from_texture;
    st_sprite_destroy_t      sprite_destroy;
    st_sprite_get_texture_t  sprite_get_texture;
    st_sprite_get_width_t    sprite_get_width;
    st_sprite_get_height_t   sprite_get_height;
    st_sprite_export_uv_t    sprite_export_uv;
} st_sprite_funcs_t;
