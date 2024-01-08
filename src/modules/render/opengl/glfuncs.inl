#pragma once

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

/* Shader program */
static GLuint (*glCreateProgram)(void);
static void (*glDeleteProgram)(GLuint program);
static void (*glAttachShader)(GLuint program, GLuint shader);
static void (*glLinkProgram)(GLuint program);
static void (*glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
static void (*glGetProgramInfoLog)(GLuint program, GLsizei maxLength,
 GLsizei *length, GLchar *infoLog);
static void (*glUseProgram)(GLuint program);

/* VBO */
static void (*glGenBuffers)(GLsizei n, GLuint *buffers);
static void (*glDeleteBuffers)(GLsizei n, const GLuint *buffers);
static void (*glBindBuffer)(GLenum target, GLuint buffer);
static void (*glBufferData)(GLenum target, GLsizeiptr size, const void *data,
 GLenum usage);
static void (*glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size,
 const void *data);

/* Vertex attribute */
static GLint (*glGetAttribLocation)(GLuint program, const GLchar *name);
static void (*glVertexAttribPointer)(GLuint index, GLint size, GLenum type,
 GLboolean normalized, GLsizei stride, const void *pointer);
static void (*glEnableVertexAttribArray)(GLuint index);
static void (*glDisableVertexAttribArray)(GLuint index);

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

        /* Shader program */
        glCreateProgram = module->glloader.get_proc_address(NULL,
         "glCreateProgram");
        glDeleteProgram = module->glloader.get_proc_address(NULL,
         "glDeleteProgram");
        glAttachShader = module->glloader.get_proc_address(NULL,
         "glAttachShader");
        glLinkProgram = module->glloader.get_proc_address(NULL,
         "glLinkProgram");
        glGetProgramiv = module->glloader.get_proc_address(NULL,
         "glGetProgramiv");
        glGetProgramInfoLog = module->glloader.get_proc_address(NULL,
         "glGetProgramInfoLog");
        glUseProgram = module->glloader.get_proc_address(NULL, "glUseProgram");

        /* VBO */
        glGenBuffers = module->glloader.get_proc_address(NULL, "glGenBuffers");
        glDeleteBuffers = module->glloader.get_proc_address(NULL,
         "glDeleteBuffers");
        glBindBuffer = module->glloader.get_proc_address(NULL, "glBindBuffer");
        glBufferData = module->glloader.get_proc_address(NULL, "glBufferData");
        glBufferSubData = module->glloader.get_proc_address(NULL,
         "glBufferSubData");

        /* Vertex attribute */
        glGetAttribLocation = module->glloader.get_proc_address(NULL,
         "glGetAttribLocation");
        glVertexAttribPointer = module->glloader.get_proc_address(NULL,
         "glVertexAttribPointer");
        glEnableVertexAttribArray = module->glloader.get_proc_address(NULL,
         "glEnableVertexAttribArray");
        glDisableVertexAttribArray = module->glloader.get_proc_address(NULL,
         "glDisableVertexAttribArray");
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
