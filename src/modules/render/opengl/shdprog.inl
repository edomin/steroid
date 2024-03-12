#pragma once

#define SHDPROG_LOG_SIZE 1024

static bool shdprog_init(st_modctx_t *render_ctx, st_shader_t *vert,
 st_shader_t *frag) {
    st_render_opengl_t *module = render_ctx->data;
    st_glfuncs_t       *gl = &module->gl;
    st_shdprog_t       *shdprog = &module->shdprog;
    GLint               linked;
    GLchar              log[SHDPROG_LOG_SIZE];

    shdprog->module = module;
    shdprog->handle = gl->create_program();
    if (!shdprog->handle) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable create shader program: %s",
         module->gldebug.get_error_msg(module->gldebug.ctx, glGetError()));

        return false;
    }

    gl->attach_shader(shdprog->handle, vert->handle);
    gl->attach_shader(shdprog->handle, frag->handle);
    gl->link_program(shdprog->handle);

    gl->get_program_iv(shdprog->handle, GL_LINK_STATUS, &linked);
    if(!linked) {
        gl->get_program_info_log(shdprog->handle, SHDPROG_LOG_SIZE, NULL, log);
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to link shader program: %s", log);
        shdprog->handle = 0;

        return false;
    }

    return true;
}

static void shdprog_free(st_shdprog_t *shdprog) {
    st_glfuncs_t *gl = &shdprog->module->gl;

    if (shdprog && shdprog->handle) {
        gl->delete_program(shdprog->handle);
        shdprog->handle = 0;
    }
}

static void shdprog_use(const st_shdprog_t *shdprog) {
    st_glfuncs_t *gl = &shdprog->module->gl;

    gl->use_program(shdprog->handle);
}

static void shdprog_unuse(const st_shdprog_t *shdprog) {
    st_glfuncs_t *gl = &shdprog->module->gl;

    gl->use_program(0);
}
