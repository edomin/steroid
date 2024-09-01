#include "stb_image.h"

#include <errno.h>
#include <stdio.h>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <stb/stb_image.h>
#pragma GCC diagnostic pop

#define REQ_CHANNELS   4
#define CODEC_PRIORITY 50

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(bmcodec_stb_image)
ST_MODULE_DEF_INIT_FUNC(bmcodec_stb_image)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_bmcodec_stb_image_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_bmcodec_import_functions(st_modctx_t *bmcodec_ctx,
 st_modctx_t *bitmap_ctx, st_modctx_t *logger_ctx) {
    st_bmcodec_stb_image_t *module = bmcodec_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "bmcodec_stb_image: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("bmcodec_stb_image", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("bmcodec_stb_image", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("bmcodec_stb_image", bitmap, import);

    return true;
}

static st_modctx_t *st_bmcodec_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx) {
    st_modctx_t             *bmcodec_ctx;
    st_bmcodec_stb_image_t  *module;

    bmcodec_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_bmcodec_stb_image_data, sizeof(st_bmcodec_stb_image_t));

    if (!bmcodec_ctx)
        return NULL;

    bmcodec_ctx->funcs = &st_bmcodec_stb_image_funcs;

    module = bmcodec_ctx->data;
    module->bitmap.ctx = bitmap_ctx;
    module->logger.ctx = logger_ctx;

    if (!st_bmcodec_import_functions(bmcodec_ctx, bitmap_ctx, logger_ctx))
        goto fail;

    stbi_set_unpremultiply_on_load(true);
    stbi_convert_iphone_png_to_rgb(true);

    module->logger.info(module->logger.ctx,
     "bmcodec_stb_image: stb_image codec initialized");

    return bmcodec_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, bmcodec_ctx);

    return NULL;
}

static void st_bmcodec_quit(st_modctx_t *bmcodec_ctx) {
    st_bmcodec_stb_image_t *module = bmcodec_ctx->data;

    module->logger.info(module->logger.ctx,
     "bmcodec_stb_image: stb_image codec destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, bmcodec_ctx);
}

static int st_bmcodec_get_priority(st_modctx_t *bmcodec_ctx) {
    return CODEC_PRIORITY;
}

static st_bitmap_t *st_bmcodec_load(st_modctx_t *bmcodec_ctx,
 const char *filename) {
    st_bmcodec_stb_image_t *module = bmcodec_ctx->data;
    st_bitmap_t            *result = NULL;
    int                     width;
    int                     height;
    void                   *buffer = stbi_load(filename, &width, &height, NULL,
     REQ_CHANNELS);

    if (!buffer)
        return NULL;

    result = module->bitmap.import(module->bitmap.ctx, buffer, (unsigned)width,
     (unsigned)height, PF_RGBA);

    stbi_image_free(buffer);

    return result;
}

static st_bitmap_t *st_bmcodec_memload(st_modctx_t *bmcodec_ctx,
 const void *data, size_t size) {
    st_bmcodec_stb_image_t *module = bmcodec_ctx->data;
    st_bitmap_t            *result = NULL;
    int                     width;
    int                     height;
    void                   *buffer = stbi_load_from_memory(data, (int)size,
     &width, &height, NULL, REQ_CHANNELS);

    if (!buffer)
        return NULL;

    result = module->bitmap.import(module->bitmap.ctx, buffer, (unsigned)width,
     (unsigned)height,
     PF_RGBA);

    stbi_image_free(buffer);

    return result;
}

static bool st_bmcodec_save(__attribute__((unused)) st_modctx_t *bmcodec_ctx,
 __attribute__((unused)) const st_bitmap_t *bitmap,
 __attribute__((unused)) const char *filename,
 __attribute__((unused)) const char *format) {
    return false; /* Not supported by implementation */
}

static bool st_bmcodec_memsave(__attribute__((unused)) st_modctx_t *bmcodec_ctx,
 __attribute__((unused)) void *dst,
 __attribute__((unused)) size_t *size,
 __attribute__((unused)) const st_bitmap_t *bitmap,
 __attribute__((unused)) const char *format) {
    return false; /* Not supported by implementation */
}
