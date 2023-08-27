#ifndef ST_MODULES_RENDER_OPENGL_VERTICES_INC
#define ST_MODULES_RENDER_OPENGL_VERTICES_INC

#define INITIAL_VERTICES_CAPACITY 1048576

static bool vertices_init(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;
    st_vertices_t      *vertices = &module->vertices;

    vertices->handle = module->dynarr.create(module->dynarr.ctx,
     sizeof(float), INITIAL_VERTICES_CAPACITY);

    if (!vertices->handle) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to create dynamic array for vertices data");

        return false;
    }

    vertices->module = module;

    return true;
}

static void vertices_free(st_vertices_t *vertices) {
    vertices->module->dynarr.destroy(vertices->handle);
}

static bool vertices_add(st_vertices_t *vertices, float x, float y, float z,
 float u, float v) {
    st_render_opengl_t *module = vertices->module;

    return module->dynarr.append(vertices->handle, &x)
        && module->dynarr.append(vertices->handle, &y)
        && module->dynarr.append(vertices->handle, &z)
        && module->dynarr.append(vertices->handle, &u)
        && module->dynarr.append(vertices->handle, &v);
}

static bool vertices_clear(st_vertices_t *vertices) {
    return vertices->module->dynarr.clear(vertices->handle);
}

#endif /* ST_MODULES_RENDER_OPENGL_VERTICES_INC */