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

ST_MODULE_DEF_GET_FUNC(sprite_simple)
ST_MODULE_DEF_INIT_FUNC(sprite_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_sprite_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_sprite_import_functions(st_modctx_t *sprite_ctx,
 st_modctx_t *atlas_ctx, st_modctx_t *logger_ctx, st_modctx_t *texture_ctx) {
    st_sprite_simple_t *module = sprite_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "sprite_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", atlas, get_texture);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", atlas, get_clip_x);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", atlas, get_clip_y);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", atlas, get_clip_width);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", atlas, get_clip_height);

    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", texture, get_width);
    ST_LOAD_FUNCTION_FROM_CTX("sprite_simple", texture, get_height);

    return true;
}

static st_modctx_t *st_sprite_init(st_modctx_t *atlas_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *texture_ctx) {
    st_modctx_t        *sprite_ctx;
    st_sprite_simple_t *module;

    sprite_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_sprite_simple_data, sizeof(st_sprite_simple_t));

    if (!sprite_ctx)
        return NULL;

    sprite_ctx->funcs = &st_sprite_simple_funcs;

    module = sprite_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_sprite_import_functions(sprite_ctx, atlas_ctx, logger_ctx,
     texture_ctx))
        goto fail;

    module->logger.info(module->logger.ctx,
     "sprite_simple: Sprites mgr initialized");

    return sprite_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, sprite_ctx);

    return NULL;
}

static void st_sprite_quit(st_modctx_t *sprite_ctx) {
    st_sprite_simple_t *module = sprite_ctx->data;

    module->logger.info(module->logger.ctx,
     "sprite_simple: Sprites mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, sprite_ctx);
}

static st_sprite_t *st_sprite_create(st_modctx_t *sprite_ctx,
 const st_atlas_t *atlas, size_t clip_num) {
    st_sprite_simple_t *module = sprite_ctx->data;
    unsigned            texture_width;
    unsigned            texture_height;
    unsigned            clip_x;
    unsigned            clip_y;
    unsigned            clip_width;
    unsigned            clip_height;
    st_sprite_t        *sprite = malloc(sizeof(st_sprite_t));

    if (!sprite) {
        module->logger.error(module->logger.ctx,
         "sprite_simple: Unable to allocate memory for new sprite: %s",
         strerror(errno));

        return NULL;
    }

    sprite->module = module;
    sprite->texture = module->atlas.get_texture(atlas);

    texture_width = module->texture.get_width(sprite->texture);
    texture_height = module->texture.get_height(sprite->texture);
    clip_x = module->atlas.get_clip_x(atlas, clip_num);
    clip_y = module->atlas.get_clip_y(atlas, clip_num);
    clip_width = module->atlas.get_clip_width(atlas, clip_num);
    clip_height = module->atlas.get_clip_height(atlas, clip_num);

    sprite->uv.upper_left.u = (float)clip_x / (float)texture_width;
    sprite->uv.upper_left.v = (float)clip_y / (float)texture_height;
    sprite->uv.upper_right.u = (float)(clip_x + clip_width) /
     (float)texture_width;
    sprite->uv.upper_right.v = (float)clip_y / (float)texture_height;
    sprite->uv.lower_left.u = (float)clip_x / (float)texture_width;
    sprite->uv.lower_left.v = (float)(clip_y + clip_height) /
     (float)texture_height;
    sprite->uv.lower_right.u = (float)(clip_x + clip_width) /
     (float)texture_width;
    sprite->uv.lower_right.v = (float)(clip_y + clip_height) /
     (float)texture_height;

    return sprite;
}

static void st_sprite_destroy(st_sprite_t *sprite) {
    free(sprite);
}

static const st_texture_t *st_sprite_get_texture(const st_sprite_t *sprite) {
    return sprite->texture;
}

static void st_sprite_export_uv(const st_sprite_t *sprite, st_uv_t *dstuv) {
    errno_t err = memcpy_s(dstuv, sizeof(st_uv_t), &sprite->uv,
     sizeof(st_uv_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        sprite->module->logger.error(sprite->module->logger.ctx,
         "sprite_simple: Unable to fill sprite uv-coords: %s", err_msg_buf);
    }
}
