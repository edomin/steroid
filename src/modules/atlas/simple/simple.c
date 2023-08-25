#include "simple.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(atlas_simple)
ST_MODULE_DEF_INIT_FUNC(atlas_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_atlas_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_atlas_import_functions(st_modctx_t *atlas_ctx,
 st_modctx_t *logger_ctx) {
    st_atlas_simple_t *module = atlas_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "atlas_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("atlas_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("atlas_simple", logger, info);

    return true;
}

static st_modctx_t *st_atlas_init(st_modctx_t *logger_ctx,
 st_modctx_t *texture_ctx) {
    st_modctx_t          *atlas_ctx;
    st_atlas_simple_t *module;

    atlas_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_atlas_simple_data, sizeof(st_atlas_simple_t));

    if (!atlas_ctx)
        return NULL;

    atlas_ctx->funcs = &st_atlas_simple_funcs;

    module = atlas_ctx->data;
    module->logger.ctx = logger_ctx;
    module->texture.ctx = texture_ctx;

    if (!st_atlas_import_functions(atlas_ctx, logger_ctx))
        goto fail;

    module->logger.info(module->logger.ctx,
     "atlas_simple: Texture atlas mgr initialized");

    return atlas_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, atlas_ctx);

    return NULL;
}

static void st_atlas_quit(st_modctx_t *atlas_ctx) {
    st_atlas_simple_t *module = atlas_ctx->data;

    module->logger.info(module->logger.ctx,
     "atlas_simple: Texture atlas mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, atlas_ctx);
}

static st_atlas_t *st_atlas_create(st_modctx_t *atlas_ctx,
 st_texture_t *texture, size_t clips_count) {
    st_atlas_simple_t *module = atlas_ctx->data;
    st_atlas_t        *atlas = malloc(
     sizeof(st_atlas_t) + sizeof(st_texclip_t) * clips_count);

    if (!atlas) {
        module->logger.info(module->logger.ctx,
         "atlas_simple: Unable to allocate memory for texture atlas");

        return NULL;
    }

    atlas->module = module;
    atlas->texture = texture;
    atlas->clips_count = clips_count;

    return atlas;
}

static void st_atlas_destroy(st_atlas_t *atlas) {
    free(atlas);
}

static bool st_atlas_set_clip(st_atlas_t *atlas, size_t clip_num, unsigned x,
 unsigned y, unsigned width, unsigned height) {
    if (!atlas || clip_num >= atlas->clips_count)
        return false;

    atlas->clips[clip_num].x = x;
    atlas->clips[clip_num].y = y;
    atlas->clips[clip_num].width = width;
    atlas->clips[clip_num].height = height;

    return true;
}

static const st_texture_t *st_atlas_get_texture(const st_atlas_t *atlas) {
    return atlas->texture;
}

static size_t st_atlas_get_clips_count(const st_atlas_t *atlas) {
    return atlas->clips_count;
}

static unsigned st_atlas_get_clip_x(const st_atlas_t *atlas, size_t clip_num) {
    return atlas->clips[clip_num].x;
}

static unsigned st_atlas_get_clip_y(const st_atlas_t *atlas, size_t clip_num) {
    return atlas->clips[clip_num].y;
}

static unsigned st_atlas_get_clip_width(const st_atlas_t *atlas,
 size_t clip_num) {
    return atlas->clips[clip_num].width;
}

static unsigned st_atlas_get_clip_height(const st_atlas_t *atlas,
 size_t clip_num) {
    return atlas->clips[clip_num].height;
}
