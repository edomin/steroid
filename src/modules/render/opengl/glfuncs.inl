#ifndef ST_MODULES_RENDER_OPENGL_GLFUNCS_INL
#define ST_MODULES_RENDER_OPENGL_GLFUNCS_INL

static st_gapi_t current_gapi;

/* VAO */
static void (*glGenVertexArrays)(GLsizei n, GLuint *arrays);
static void (*glDeleteVertexArrays)(GLsizei n, const GLuint *arrays);
static void (*glBindVertexArray)(GLuint array);

static bool glapi_least(st_gapi_t api) {
    if (api < ST_GAPI_GL11 || api > ST_GAPI_GL46)
        return false;

    return current_gapi >= api;
}

static void glfuncs_load_all(st_modctx_t *render_ctx) {
    st_render_opengl_t *module = render_ctx->data;

    current_gapi = module->gfxctx.get_api(module->gfxctx.handle);

    if (glapi_least(ST_GAPI_GL3)) {
        glGenVertexArrays = module->glloader.get_proc_address(NULL,
         "glGenVertexArrays");
        glDeleteVertexArrays = module->glloader.get_proc_address(NULL,
         "glDeleteVertexArrays");
        glBindVertexArray = module->glloader.get_proc_address(NULL,
         "glBindVertexArray");
    }
}

#endif /* ST_MODULES_RENDER_OPENGL_GLFUNCS_INL */
