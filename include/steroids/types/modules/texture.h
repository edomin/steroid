#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/gfxctx.h"

#ifndef ST_TEXTURE_T_DEFINED
    typedef struct st_texture_s st_texture_t;
#endif

typedef st_modctx_t *(*st_texture_init_t)(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx, st_gfxctx_t *gfxctx);
typedef void (*st_texture_quit_t)(st_modctx_t *texture_ctx);

typedef st_texture_t *(*st_texture_load_t)(st_modctx_t *texture_ctx,
 const char *filename);
typedef st_texture_t *(*st_texture_memload_t)(st_modctx_t *texture_ctx,
 const void *data, size_t size);
typedef void (*st_texture_destroy_t)(st_texture_t *texture);
typedef bool (*st_texture_bind_t)(const st_texture_t *texture, unsigned unit);
typedef unsigned (*st_texture_get_width_t)(const st_texture_t *texture);
typedef unsigned (*st_texture_get_height_t)(const st_texture_t *texture);

typedef struct {
    st_texture_init_t       texture_init;
    st_texture_quit_t       texture_quit;
    st_texture_load_t       texture_load;
    st_texture_memload_t    texture_memload;
    st_texture_destroy_t    texture_destroy;
    st_texture_bind_t       texture_bind;
    st_texture_get_width_t  texture_get_width;
    st_texture_get_height_t texture_get_height;
} st_texture_funcs_t;
