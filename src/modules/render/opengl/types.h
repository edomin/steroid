#ifndef ST_MODULES_RENDER_OPENGL_TYPES_H
#define ST_MODULES_RENDER_OPENGL_TYPES_H

#include <GL/gl.h>

#include "steroids/types/modules/drawq.h"
#include "steroids/types/modules/dynarr.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/glloader.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/sprite.h"
#include "steroids/types/modules/texture.h"
#include "steroids/types/modules/window.h"

typedef struct {
    st_modctx_t            *ctx;
    st_drawq_create_t       create;
    st_drawq_destroy_t      destroy;
    st_drawq_len_t          len;
    st_drawq_get_all_t      get_all;
    st_drawq_add_t          add;
    st_drawq_sort_t         sort;
    st_drawq_clear_t        clear;
    st_drawq_t             *handle;
} st_render_opengl_drawq_t;

typedef struct {
    st_modctx_t                   *ctx;
    st_dynarr_create_t             create;
    st_dynarr_destroy_t            destroy;
    st_dynarr_append_t             append;
    st_dynarr_clear_t              clear;
    st_dynarr_get_t                get;
    st_dynarr_get_all_t            get_all;
    st_dynarr_get_elements_count_t get_elements_count;
} st_render_opengl_dynarr_t;

typedef struct {
    st_gfxctx_make_current_t make_current;
    st_gfxctx_swap_buffers_t swap_buffers;
    st_gfxctx_get_window_t   get_window;
    st_gfxctx_get_api_t      get_api;
    st_gfxctx_t             *handle;
} st_render_opengl_gfxctx_t;

typedef struct {
    st_glloader_get_proc_address_t get_proc_address;
} st_render_opengl_glloader_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_render_opengl_logger_t;

typedef struct {
    st_sprite_get_texture_t get_texture;
    st_sprite_get_width_t   get_width;
    st_sprite_get_height_t  get_height;
    st_sprite_export_uv_t   export_uv;
} st_render_opengl_sprite_t;

typedef struct {
    st_texture_bind_t bind;
} st_render_opengl_texture_t;

typedef struct {
    st_window_get_width_t  get_width;
    st_window_get_height_t get_height;
    st_window_t           *handle;
} st_render_opengl_window_t;

typedef struct {
    struct st_render_opengl_s *module;
    st_dynarr_t               *handle;
} st_vertices_t;

typedef struct {
    const st_texture_t *texture;
    size_t              first_vertex_index;
    size_t              vertices_count;
} st_batch_entry_t;

typedef struct {
    struct st_render_opengl_s *module;
    st_dynarr_t               *entries;
    const st_texture_t        *current_texture;
    size_t                     current_first_vertex_index;
    size_t                     current_vertex_index;
} st_batcher_t;

typedef GLuint st_vao_t;
typedef GLuint st_shader_t;
typedef GLuint st_shdprog_t;

typedef struct {
    GLuint handle;
    size_t components_per_vertex;
    size_t vertices_size;
} st_vbo_t;

typedef struct st_render_opengl_s {
    st_render_opengl_drawq_t    drawq;
    st_render_opengl_dynarr_t   dynarr;
    st_render_opengl_gfxctx_t   gfxctx;
    st_render_opengl_glloader_t glloader;
    st_render_opengl_logger_t   logger;
    st_render_opengl_sprite_t   sprite;
    st_render_opengl_texture_t  texture;
    st_render_opengl_window_t   window;

    st_drawq_t                 *queue;
    st_vertices_t               vertices;
    st_batcher_t                batcher;
    st_vao_t                    vao;
    st_vbo_t                    vbo;
    st_shdprog_t                shdprog;
} st_render_opengl_t;

#endif /* ST_MODULES_RENDER_OPENGL_TYPES_H */
