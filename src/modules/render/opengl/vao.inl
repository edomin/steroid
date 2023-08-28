#ifndef ST_MODULES_RENDER_OPENGL_VAO_INL
#define ST_MODULES_RENDER_OPENGL_VAO_INL

#define DEFAULT_VAO_NAMES_NUMBER 1

static void vao_init(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;
    st_vao_t           *vao = &module->vao;

    glGenVertexArrays(DEFAULT_VAO_NAMES_NUMBER, vao);
    glBindVertexArray(*vao);
}

static void vao_free(st_vao_t *vao) {
    glDeleteVertexArrays(DEFAULT_VAO_NAMES_NUMBER, vao);
}

#endif /* ST_MODULES_RENDER_OPENGL_VAO_INL */
