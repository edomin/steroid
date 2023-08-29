#ifndef ST_MODULES_RENDER_OPENGL_SHDPROG_INL
#define ST_MODULES_RENDER_OPENGL_SHDPROG_INL

#define SHDPROG_LOG_SIZE 1024

static bool shdprog_init(st_modctx_t *render_ctx, st_shader_t *vert,
 st_shader_t *frag) {
    st_render_opengl_t *module = render_ctx->data;
    st_shdprog_t       *shdprog = &module->shdprog;
    GLint               linked;
    GLchar              log[SHDPROG_LOG_SIZE];

    *shdprog = glCreateProgram();
    if (!*shdprog) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable create shader program: %s",
         gluErrorString(glGetError()));

        return false;
    }

    glAttachShader(*shdprog, *vert);
    glAttachShader(*shdprog, *frag);
    glLinkProgram(*shdprog);

    glGetProgramiv(*shdprog, GL_LINK_STATUS, &linked);
    if(!linked) {
        glGetProgramInfoLog(*shdprog, SHDPROG_LOG_SIZE, NULL, log);
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to link shader program: %s", log);
        *shdprog = 0;

        return false;
    }

    return true;
}

static void shdprog_free(st_shdprog_t *shdprog) {
    if (shdprog && *shdprog) {
        glDeleteProgram(*shdprog);
        *shdprog = 0;
    }
}

static void shdprog_use(const st_shdprog_t *shdprog) {
    glUseProgram(*shdprog);
}

static void shdprog_unuse(
 __attribute__((unused)) const st_shdprog_t *shdprog) {
    glUseProgram(0);
}

#endif /* ST_MODULES_RENDER_OPENGL_SHDPROG_INL */
