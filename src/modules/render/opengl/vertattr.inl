#ifndef ST_MODULES_RENDER_OPENGL_VERTATTR_INL
#define ST_MODULES_RENDER_OPENGL_VERTATTR_INL

static bool vertattr_init(st_modctx_t *render_ctx, st_vertattr_t *vertattr,
 const st_vbo_t *vbo, const st_shdprog_t *shdprog, const char *name,
 unsigned components_count, unsigned offset) {
    st_render_opengl_t *module = render_ctx->data;
    GLenum              error;

    vertattr->shdprog = shdprog;
    vertattr->handle = glGetAttribLocation(*shdprog, name);

    if (vertattr->handle == -1) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to get attribute \"%s\" location in shader "
         "program: %s",
         name, gluErrorString(glGetError()));

        return false;
    }

    glEnableVertexAttribArray((GLuint)vertattr->handle);
    vbo_bind(&module->vbo);
    glVertexAttribPointer((GLuint)vertattr->handle, (GLint)components_count,
     GL_FLOAT, GL_FALSE,
     (GLsizei)(sizeof(float) * vbo_get_components_per_vertex(&module->vbo)),
     (void *)(uintptr_t)offset);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to init vertex attribute: %s",
         gluErrorString(error));
    }

    vbo_unbind(&module->vbo);
    glDisableVertexAttribArray((GLuint)vertattr->handle);

    if (error != GL_NO_ERROR)
        vertattr->handle = -1;

    return error != GL_NO_ERROR;
}

static void vertattr_free(st_vertattr_t *vertattr) {
    vertattr->handle = -1;
}

static void vertattr_enable(const st_vertattr_t *vertattr) {
    if (vertattr->handle != -1)
        glEnableVertexAttribArray((GLuint)vertattr->handle);
}

static void vertattr_disable(const st_vertattr_t *vertattr) {
    if (vertattr->handle != -1)
        glDisableVertexAttribArray((GLuint)vertattr->handle);
}

#endif /* ST_MODULES_RENDER_OPENGL_VERTATTR_INL */
