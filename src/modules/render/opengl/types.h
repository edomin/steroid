#pragma once

#include <GL/gl.h>

#include "glfuncs.h"
#include "modules_data.h"


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

typedef struct {
    struct st_render_opengl_s *module;
    GLuint                     handle;
} st_vao_t;

typedef struct {
    struct st_render_opengl_s *module;
    GLuint                     handle;
} st_shader_t;

typedef struct {
    struct st_render_opengl_s *module;
    GLuint                     handle;
} st_shdprog_t;

typedef struct {
    struct st_render_opengl_s *module;
    GLuint                     handle;
    unsigned                   components_per_vertex;
    size_t                     vertices_size;
} st_vbo_t;

typedef struct {
    struct st_render_opengl_s *module;
    GLint                      handle;
} st_vertattr_t;

typedef struct st_render_opengl_s {
    st_render_opengl_angle_t     angle;
    st_render_opengl_drawq_t     drawq;
    st_render_opengl_dynarr_t    dynarr;
    st_render_opengl_gfxctx_t    gfxctx;
    st_render_opengl_glloader_t  glloader;
    st_render_opengl_logger_t    logger;
    st_render_opengl_matrix3x3_t matrix3x3;
    st_render_opengl_sprite_t    sprite;
    st_render_opengl_texture_t   texture;
    st_render_opengl_vec2_t      vec2;
    st_render_opengl_window_t    window;

    st_gapi_t                    gapi;
    st_glfuncs_t                 gl;
    st_glsupported_t             glsupported;

    st_drawq_t                  *queue;
    st_vertices_t                vertices;
    st_batcher_t                 batcher;
    st_vao_t                     vao;
    st_vbo_t                     vbo;
    st_shdprog_t                 shdprog;
    st_vertattr_t                posattr;
    st_vertattr_t                texcrdattr;
} st_render_opengl_t;
