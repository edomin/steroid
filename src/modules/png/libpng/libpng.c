#include "libpng.h"

#include <png.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(png_libpng)
ST_MODULE_DEF_INIT_FUNC(png_libpng)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_png_libpng_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_png_import_functions(st_modctx_t *png_ctx,
 st_modctx_t *bitmap_ctx, st_modctx_t *logger_ctx) {
    st_png_libpng_t *module = png_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "png_libpng: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", logger, warning);

    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", bitmap, import);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", bitmap, get_data);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", bitmap, get_width);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", bitmap, get_height);
    ST_LOAD_FUNCTION_FROM_CTX("png_libpng", bitmap, get_pixel_format);

    return true;
}

static st_modctx_t *st_png_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx) {
    st_modctx_t          *png_ctx;
    st_png_libpng_t      *module;
    st_bitmap_add_codec_t add_codec;

    png_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_png_libpng_data, sizeof(st_png_libpng_t));

    if (!png_ctx)
        return NULL;

    png_ctx->funcs = &st_png_libpng_funcs;

    module = png_ctx->data;
    module->bitmap.ctx = bitmap_ctx;
    module->logger.ctx = logger_ctx;

    add_codec = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, bitmap_ctx, "add_codec");
    if (!add_codec) {
        fprintf(stderr,
         "png_libpng: Unable to load function \"add_codec\" from module "
         "\"bitmap\"\n");

        goto fail;
    }

    if (!st_png_import_functions(png_ctx, bitmap_ctx, logger_ctx))
        goto fail;

    add_codec(bitmap_ctx, png_ctx);

    module->logger.info(module->logger.ctx,
     "png_libpng: PNG codec initialized.");

    return png_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, png_ctx);

    return NULL;
}

static void st_png_quit(st_modctx_t *png_ctx) {
    st_png_libpng_t *module = png_ctx->data;

    module->logger.info(module->logger.ctx, "png_libpng: PNG codec destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, png_ctx);
}

static st_bitmap_t *st_png_load(st_modctx_t *png_ctx, const char *filename) {
    st_png_libpng_t *module = png_ctx->data;
    png_image        image = {0};
    png_bytep        buffer;
    st_bitmap_t     *result = NULL;

    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_file(&image, filename))
        return NULL;

    image.format = PNG_FORMAT_RGBA;

    buffer = malloc(PNG_IMAGE_SIZE(image)); // NOLINT(hicpp-signed-bitwise)
    if (!buffer) {
        module->logger.info(module->logger.ctx,
         "png_libpng: Unable to allocate memory for read buffer: %s",
         strerror(errno));

        goto malloc_fail;
    }

    if (!png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
        module->logger.warning(module->logger.ctx,
         "png_libpng: Unable to decode PNG file \"%s\": %s", filename,
         strerror(errno));

        goto read_fail;
    }

    result = module->bitmap.import(module->bitmap.ctx, buffer, image.width,
     image.height, PF_RGBA);

read_fail:
    free(buffer);
malloc_fail:
    png_image_free(&image);

    return result;
}

static st_bitmap_t *st_png_memload(st_modctx_t *png_ctx, const void *data,
 size_t size) {
    st_png_libpng_t *module = png_ctx->data;
    png_image        image = {0};
    png_bytep        buffer;
    st_bitmap_t     *result = NULL;

    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&image, data, size))
        return NULL;

    image.format = PNG_FORMAT_RGBA;

    buffer = malloc(PNG_IMAGE_SIZE(image)); // NOLINT(hicpp-signed-bitwise)
    if (!buffer) {
        module->logger.info(module->logger.ctx,
         "png_libpng: Unable to allocate memory for read buffer: %s",
         strerror(errno));

        goto malloc_fail;
    }

    if (!png_image_finish_read(&image, NULL, buffer, 0, NULL)) {
        module->logger.warning(module->logger.ctx,
         "png_libpng: Unable to decode PNG data: %s", strerror(errno));

        goto read_fail;
    }

    result = module->bitmap.import(module->bitmap.ctx, buffer, image.width,
     image.height, PF_RGBA);

read_fail:
    free(buffer);
malloc_fail:
    png_image_free(&image);

    return result;
}

static bool st_png_save(st_modctx_t *png_ctx, const st_bitmap_t *bitmap,
 const char *filename) {
    _Static_assert(PF_MAX == PF_RGBA + 1, "New pixel format available");

    st_png_libpng_t *module = png_ctx->data;
    png_image        image = {
        .version = PNG_IMAGE_VERSION,
        .opaque = NULL,
        .width = module->bitmap.get_width(bitmap),
        .height = module->bitmap.get_height(bitmap),
        .format = PNG_FORMAT_RGBA,
        .flags = 0,
        .colormap_entries = 0,
    };

    return png_image_write_to_file(&image, filename, true,
     module->bitmap.get_data(bitmap), 0, NULL);
}

static const char *st_png_memsave(st_modctx_t *png_ctx, void *dst, size_t *size,
 const st_bitmap_t *bitmap) {
    _Static_assert(PF_MAX == PF_RGBA + 1, "New pixel format available");

    st_png_libpng_t *module = png_ctx->data;
    png_image        image = {
        .version = PNG_IMAGE_VERSION,
        .opaque = NULL,
        .width = module->bitmap.get_width(bitmap),
        .height = module->bitmap.get_height(bitmap),
        .format = PNG_FORMAT_RGBA,
        .flags = 0,
        .colormap_entries = 0,
    };

    if (png_image_write_to_memory (&image, dst, size, true,
     module->bitmap.get_data(bitmap), 0, NULL))
        return "png";

    return NULL;
}

