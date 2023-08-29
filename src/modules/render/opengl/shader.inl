#ifndef ST_MODULES_RENDER_OPENGL_SHADER_INL
#define ST_MODULES_RENDER_OPENGL_SHADER_INL

#define VERTEX_SHADER_SOURCE_GL33                         \
    "#version 330 core\n"                                 \
    "attribute vec3 pos;\n"                               \
    "attribute vec2 vert_tex_coord;\n"                    \
    "out       vec2 frag_tex_coord;\n"                    \
    "void main() {\n"                                     \
    "    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n" \
    "    frag_tex_coord = vert_tex_coord;\n"              \
    "}\n"

#define FRAGMENT_SHADER_SOURCE_GL33                      \
    "#version 330 core\n"                                \
    "in  vec2 frag_tex_coord;\n"                         \
    "out vec4 color;\n"                                  \
    "uniform sampler2D in_texture;"                      \
    "void main() {\n"                                    \
    "    color = texture(in_texture, frag_tex_coord);\n" \
    "}\n"

#define SHD_SOURCES_COUNT 1
#define SHD_LOG_SIZE      1024

typedef enum {
    SHD_VERTEX = GL_VERTEX_SHADER,
    SHD_FRAGMENT = GL_FRAGMENT_SHADER,
} st_shader_type_t;

static bool shader_init(st_modctx_t *render_ctx, st_shader_t *shader,
 st_shader_type_t type, const char *source) {
    st_render_opengl_t *module = render_ctx->data;
    GLint               compiled;
    GLchar              log[SHD_LOG_SIZE];

    *shader = glCreateShader(type);
    if (!*shader) {
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable create shader: %s",
         gluErrorString(glGetError()));

        return false;
    }

    glShaderSource(*shader, SHD_SOURCES_COUNT, &source, NULL);
    glCompileShader(*shader);

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        glGetShaderInfoLog(*shader, SHD_LOG_SIZE, NULL, log);
        module->logger.error(module->logger.ctx,
         "render_opengl: Unable to compile shader: %s", log);
        *shader = 0;

        return false;
    }

    return true;
}

static void shader_free(st_shader_t *shader) {
    if (shader && *shader) {
        glDeleteShader(*shader);
        *shader = 0;
    }
}

#endif /* ST_MODULES_RENDER_OPENGL_SHADER_INL */
