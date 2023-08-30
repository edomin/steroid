#ifndef ST_MODULES_RENDER_OPENGL_VBO_INL
#define ST_MODULES_RENDER_OPENGL_VBO_INL

#define DEFAULT_VBOS_COUNT    1
#define INITIAL_VERTICES_SIZE sizeof(float) * 15
#define INITIAL_VERTICES_DATA (float[]){ \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f,        \
}

static void vbo_init(st_modctx_t *render_ctx, size_t components_per_vertex) {
    st_render_opengl_t *module = render_ctx->data;
    st_vbo_t           *vbo = &module->vbo;

    vbo->components_per_vertex = components_per_vertex;
    vbo->vertices_size = INITIAL_VERTICES_SIZE;

    glGenBuffers(DEFAULT_VBOS_COUNT, &vbo->handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->handle);
    glBufferData(GL_ARRAY_BUFFER, INITIAL_VERTICES_SIZE, INITIAL_VERTICES_DATA,
     GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void vbo_free(st_vbo_t *vbo) {
    glDeleteBuffers(DEFAULT_VBOS_COUNT, &vbo->handle);
}

static void vbo_bind(st_vbo_t *vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo->handle);
}

static void vbo_unbind(__attribute__((unused)) st_vbo_t *vbo) {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void vbo_set_vertices(st_vbo_t *vbo, const st_vertices_t *vertices) {
    size_t size = vertices_size(vertices);

    if (size > vbo->vertices_size) {
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size,
         vertices_get_all(vertices), GL_DYNAMIC_DRAW);
        vbo->vertices_size = size;
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)size,
         vertices_get_all(vertices));
    }
}

// static size_t vbo_get_components_per_vertex(const st_vbo_t *vbo) {
//     return vbo->components_per_vertex;
// }

#endif /* ST_MODULES_RENDER_OPENGL_VBO_INL */
