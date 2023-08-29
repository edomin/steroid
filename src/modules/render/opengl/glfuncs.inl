#ifndef ST_MODULES_RENDER_OPENGL_GLFUNCS_INL
#define ST_MODULES_RENDER_OPENGL_GLFUNCS_INL

static st_gapi_t current_gapi;

/* GL 2.0 */
/* Shader */
static GLuint (*glCreateShader)(GLenum shaderType);
static void (*glShaderSource)(GLuint shader, GLsizei count,
 const GLchar **string, const GLint *length);
static void (*glCompileShader)(GLuint shader);
static void (*glDeleteShader)(GLuint shader);
static void (*glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
static void (*glGetShaderInfoLog)(GLuint shader, GLsizei maxLength,
 GLsizei *length, GLchar *infoLog);

/* GL 3.0 */
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

    if (glapi_least(ST_GAPI_GL2)) {
        /* Shader */
        glCreateShader = module->glloader.get_proc_address(NULL,
         "glCreateShader");
        glShaderSource = module->glloader.get_proc_address(NULL,
         "glShaderSource");
        glCompileShader = module->glloader.get_proc_address(NULL,
         "glCompileShader");
        glDeleteShader = module->glloader.get_proc_address(NULL,
         "glDeleteShader");
        glGetShaderiv = module->glloader.get_proc_address(NULL,
         "glGetShaderiv");
        glGetShaderInfoLog = module->glloader.get_proc_address(NULL,
         "glGetShaderInfoLog");
    }

    if (glapi_least(ST_GAPI_GL3)) {
        /* VAO */
        glGenVertexArrays = module->glloader.get_proc_address(NULL,
         "glGenVertexArrays");
        glDeleteVertexArrays = module->glloader.get_proc_address(NULL,
         "glDeleteVertexArrays");
        glBindVertexArray = module->glloader.get_proc_address(NULL,
         "glBindVertexArray");
    }
}

#endif /* ST_MODULES_RENDER_OPENGL_GLFUNCS_INL */
