#pragma once

#define DEFAULT_VBOS_COUNT    1
#define INITIAL_VERTICES_SIZE sizeof(float) * 15
#define INITIAL_VERTICES_DATA (float[]){ \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
}

static void vbo_init(st_modctx_t *render_ctx, unsigned components_per_vertex) {
    st_render_opengl_t *module = render_ctx->data;
    st_vbo_t           *vbo = &module->vbo;
    st_glfuncs_t       *gl = &module->gl;

    vbo->components_per_vertex = components_per_vertex;
    vbo->vertices_size = INITIAL_VERTICES_SIZE;
    vbo->module = module;

    gl->gen_buffers(DEFAULT_VBOS_COUNT, &vbo->handle);
    gl->bind_buffer(GL_ARRAY_BUFFER, vbo->handle);
    gl->buffer_data(GL_ARRAY_BUFFER, INITIAL_VERTICES_SIZE, INITIAL_VERTICES_DATA,
     GL_DYNAMIC_DRAW);
    gl->bind_buffer(GL_ARRAY_BUFFER, 0);
}

static void vbo_free(st_vbo_t *vbo) {
    st_glfuncs_t *gl = &vbo->module->gl;

    gl->delete_buffers(DEFAULT_VBOS_COUNT, &vbo->handle);
}

static void vbo_bind(st_vbo_t *vbo) {
    st_glfuncs_t *gl = &vbo->module->gl;

    gl->bind_buffer(GL_ARRAY_BUFFER, vbo->handle);
}

static void vbo_unbind(st_vbo_t *vbo) {
    st_glfuncs_t *gl = &vbo->module->gl;

    gl->bind_buffer(GL_ARRAY_BUFFER, 0);
}

static void vbo_set_vertices(st_vbo_t *vbo, const st_vertices_t *vertices) {
    st_glfuncs_t *gl = &vbo->module->gl;
    size_t        size = vertices_size(vertices);

    if (size > vbo->vertices_size) {
        gl->buffer_data(GL_ARRAY_BUFFER, (GLsizeiptr)size,
         vertices_get_all(vertices), GL_DYNAMIC_DRAW);
        vbo->vertices_size = size;
    } else {
        gl->buffer_sub_data(GL_ARRAY_BUFFER, 0, (GLsizeiptr)size,
         vertices_get_all(vertices));
    }
}

static unsigned vbo_get_components_per_vertex(const st_vbo_t *vbo) {
    return vbo->components_per_vertex;
}
