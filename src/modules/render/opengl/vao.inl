#pragma once

#define DEFAULT_VAO_NAMES_NUMBER 1

static void vao_init(st_modctx_t *render_ctx, st_vao_t *vao) {
    st_render_opengl_t *module = render_ctx->data;
    st_glfuncs_t       *gl = &module->gl;

    gl->gen_vertex_arrays(DEFAULT_VAO_NAMES_NUMBER, &vao->handle);
    gl->bind_vertex_array(vao->handle);
    vao->module = render_ctx->data;
}

static void vao_free(st_vao_t *vao) {
    st_glfuncs_t *gl = &vao->module->gl;

    gl->delete_vertex_arrays(DEFAULT_VAO_NAMES_NUMBER, &vao->handle);
}

static void vao_bind(st_vao_t *vao) {
    st_glfuncs_t *gl = &vao->module->gl;

    gl->bind_vertex_array(vao->handle);
}

static void vao_unbind(st_vao_t *vao) {
    st_glfuncs_t *gl = &vao->module->gl;

    gl->bind_vertex_array(0);
}
