#pragma once

static bool vertattr_init(st_modctx_t *render_ctx, st_vertattr_t *vertattr,
 const st_vbo_t *vbo, const st_shdprog_t *shdprog, const char *name,
 unsigned components_count, unsigned offset) {
    st_render_opengl_t *module = render_ctx->data;
    GLenum              error;
    st_glfuncs_t       *gl = &module->gl;

    vertattr->module = module;
    vertattr->handle = gl->get_attrib_location(shdprog->handle, name);

    if (vertattr->handle == -1) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to get attribute \"%s\" location in shader "
         "program: %s",
         name, gluErrorString(glGetError()));

        return false;
    }

    gl->enable_vertex_attrib_array((GLuint)vertattr->handle);
    vbo_bind(&module->vbo);
    gl->vertex_attrib_pointer((GLuint)vertattr->handle, (GLint)components_count,
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
    gl->disable_vertex_attrib_array((GLuint)vertattr->handle);

    if (error != GL_NO_ERROR)
        vertattr->handle = -1;

    return error == GL_NO_ERROR;
}

static void vertattr_free(st_vertattr_t *vertattr) {
    vertattr->handle = -1;
}

static void vertattr_enable(const st_vertattr_t *vertattr) {
    st_glfuncs_t *gl = &vertattr->module->gl;

    if (vertattr->handle != -1)
        gl->enable_vertex_attrib_array((GLuint)vertattr->handle);
}

static void vertattr_disable(const st_vertattr_t *vertattr) {
    st_glfuncs_t *gl = &vertattr->module->gl;

    if (vertattr->handle != -1)
        gl->disable_vertex_attrib_array((GLuint)vertattr->handle);
}
