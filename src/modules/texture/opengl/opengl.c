#include "opengl.h"

#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void (*glGenerateMipmap)(GLenum target);

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(texture_opengl)
ST_MODULE_DEF_INIT_FUNC(texture_opengl)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_texture_opengl_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_texture_import_functions(st_modctx_t *texture_ctx,
 st_modctx_t *bitmap_ctx, st_modctx_t *logger_ctx) {
    st_texture_opengl_t *module = texture_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "texture_opengl: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("texture_opengl", glloader, NULL, get_proc_address);

    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, load);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, memload);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, destroy);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, get_data);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, get_width);
    ST_LOAD_FUNCTION_FROM_CTX("texture_opengl", bitmap, get_height);

    return true;
}

static st_modctx_t *st_texture_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx, st_gapi_t gapi) {
    st_modctx_t         *texture_ctx;
    st_texture_opengl_t *module;

    texture_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_texture_opengl_data, sizeof(st_texture_opengl_t));

    if (!texture_ctx)
        return NULL;

    texture_ctx->funcs = &st_texture_opengl_funcs;

    module = texture_ctx->data;
    module->bitmap.ctx = bitmap_ctx;
    module->logger.ctx = logger_ctx;

    if (!st_texture_import_functions(texture_ctx, bitmap_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, texture_ctx);

        return NULL;
    }

    glGenerateMipmap = module->glloader.get_proc_address(NULL,
     "glGenerateMipmap");

    module->logger.info(module->logger.ctx,
     "texture_opengl: Texture mgr initialized.");

    return texture_ctx;
}

static void st_texture_quit(st_modctx_t *texture_ctx) {
    st_texture_opengl_t *module = texture_ctx->data;

    module->logger.info(module->logger.ctx,
     "texture_opengl: Texture mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, texture_ctx);
}

static bool glapi_least(st_modctx_t *texture_ctx, st_gapi_t api) {
    st_texture_opengl_t *module = texture_ctx->data;

    if (api < ST_GAPI_GL11 || api > ST_GAPI_GL46)
        return false;

    return module->api >= api;
}

static st_texture_t *st_texture_load(st_modctx_t *texture_ctx,
 const char *filename) {
    st_texture_opengl_t *module = texture_ctx->data;
    st_bitmap_t         *bitmap;
    GLenum               error;
    st_texture_t        *texture = malloc(sizeof(st_texture_t));

    if (!texture) {
        module->logger.error(module->logger.ctx,
         "texture_opengl: Unable to allocate memory for texture struct \"%s\": "
         "%s", filename, strerror(errno));

        return NULL;
    }

    bitmap = module->bitmap.load(module->bitmap.ctx, filename);
    if (!bitmap)
        goto bitmap_load_fail;

    texture->module = module;
    texture->width = module->bitmap.get_width(bitmap);
    texture->height = module->bitmap.get_height(bitmap);

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    if (glapi_least(texture_ctx, ST_GAPI_GL3) && glGenerateMipmap) {
        float mip_max = log2f(
         ((float)texture->width > (float)texture->height)
          ? (float)texture->width
          : (float)texture->height
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)mip_max);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, (GLsizei)texture->width,
     (GLsizei)texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
     module->bitmap.get_data(bitmap));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    if (glapi_least(texture_ctx, ST_GAPI_GL3) && glGenerateMipmap)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
         GL_NEAREST_MIPMAP_NEAREST);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
         GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        module->logger.error(module->logger.ctx,
         "texture_opengl: Unable to load texture \"%s\": %s", filename,
         gluErrorString(error));

        goto teximage2d_fail;
    }

    module->bitmap.destroy(bitmap);

    return texture;

teximage2d_fail:
    glDeleteTextures(1, &texture->id);
    module->bitmap.destroy(bitmap);

bitmap_load_fail:
    free(texture);

    return NULL;
}

static st_texture_t *st_texture_memload(st_modctx_t *texture_ctx,
 const void *data, size_t size) {
    st_texture_opengl_t *module = texture_ctx->data;
    st_bitmap_t         *bitmap;
    GLenum               error;
    st_texture_t        *texture = malloc(sizeof(st_texture_t));

    if (!texture) {
        module->logger.error(module->logger.ctx,
         "texture_opengl: Unable to allocate memory for texture struct: %s",
         strerror(errno));

        return NULL;
    }

    bitmap = module->bitmap.memload(module->bitmap.ctx, data, size);
    if (!bitmap)
        goto bitmap_load_fail;

    texture->module = module;
    texture->width = module->bitmap.get_width(bitmap);
    texture->height = module->bitmap.get_height(bitmap);

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
     GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)texture->width,
     (GLsizei)texture->height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, data);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        module->logger.error(module->logger.ctx,
         "texture_opengl: Unable to load texture: %s", gluErrorString(error));

        goto teximage2d_fail;
    }

    if (glapi_least(texture_ctx, ST_GAPI_GL3))
        glGenerateMipmap(GL_TEXTURE_2D);

    module->bitmap.destroy(bitmap);

    return texture;

teximage2d_fail:
    glDeleteTextures(1, &texture->id);
    module->bitmap.destroy(bitmap);

bitmap_load_fail:
    free(texture);

    return NULL;
}

static void st_texture_destroy(st_texture_t *texture) {
    glDeleteTextures(1, &texture->id);
    free(texture);
}

static bool st_texture_bind(const st_texture_t *texture, unsigned unit) {
    GLenum error;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    error = glGetError();
    if (error != GL_NO_ERROR) {
        texture->module->logger.error(texture->module->logger.ctx,
         "texture_opengl: Unable to bind texture: %s", gluErrorString(error));

        return false;
    }

    return true;
}

static unsigned st_texture_get_width(const st_texture_t *texture) {
    return texture->width;
}

static unsigned st_texture_get_height(const st_texture_t *texture) {
    return texture->height;
}
