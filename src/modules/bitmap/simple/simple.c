#include "simple.h"

#include <stdbool.h>
#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE      1024

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(bitmap_simple)
ST_MODULE_DEF_INIT_FUNC(bitmap_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_bitmap_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_bitmap_import_functions(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx) {
    st_bitmap_simple_t *module = bitmap_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "bitmap_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("bitmap_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("bitmap_simple", logger, info);

    return true;
}

static st_modctx_t *st_bitmap_init(st_modctx_t *logger_ctx) {
    st_modctx_t        *bitmap_ctx;
    st_bitmap_simple_t *module;

    bitmap_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_bitmap_simple_data, sizeof(st_bitmap_simple_t));

    if (!bitmap_ctx)
        return NULL;

    bitmap_ctx->funcs = &st_bitmap_simple_funcs;

    module = bitmap_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_bitmap_import_functions(bitmap_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, bitmap_ctx);

        return NULL;
    }

    module->codecs_count = 0;

    module->logger.info(module->logger.ctx,
     "bitmap_simple: Bitmaps mgr initialized.");

    return bitmap_ctx;
}

static void st_bitmap_quit(st_modctx_t *bitmap_ctx) {
    st_bitmap_simple_t *module = bitmap_ctx->data;

    module->logger.info(module->logger.ctx,
     "bitmap_simple: Bitmaps mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, bitmap_ctx);
}

static void st_bitmap_add_codec(st_modctx_t *bitmap_ctx,
 st_modctx_t *codec_ctx) {
    st_bitmap_simple_t       *module = bitmap_ctx->data;
    st_bitmap_simple_codec_t *codec = &module->codecs[module->codecs_count++];

    codec->load = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, codec_ctx, "load");
    if (!codec->load ) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"load\" from codec");

        return;
    }

    codec->memload = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, codec_ctx, "memload");
    if (!codec->memload) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"memload\" from codec");

        return;
    }

    codec->save = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, codec_ctx, "save");
    if (!codec->save) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"save\" from codec");

        return;
    }

    codec->memsave = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, codec_ctx, "memsave");
    if (!codec->memsave) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"memsave\" from codec");

        return;
    }

    codec->ctx = codec_ctx;
}

static st_bitmap_t *st_bitmap_load(st_modctx_t *bitmap_ctx,
 const char *filename) {
    st_bitmap_simple_t *module = bitmap_ctx->data;

    for (size_t i = 0; i < module->codecs_count; i++) {
        st_bitmap_simple_codec_t *codec = &module->codecs[i];
        st_bitmap_t              *bitmap = codec->load(codec->ctx, filename);

        if (bitmap)
            return bitmap;
    }

    module->logger.error(module->logger.ctx,
     "bitmap_simple: No suitable codec for loading bitmap \"%s\"", filename);

    return NULL;
}

static st_bitmap_t *st_bitmap_memload(st_modctx_t *bitmap_ctx, const void *data,
 size_t size) {
    st_bitmap_simple_t *module = bitmap_ctx->data;

    for (size_t i = 0; i < module->codecs_count; i++) {
        st_bitmap_simple_codec_t *codec = &module->codecs[i];
        st_bitmap_t              *bitmap = codec->memload(codec->ctx, data,
         size);

        if (bitmap)
            return bitmap;
    }

    module->logger.error(module->logger.ctx,
     "bitmap_simple: No suitable codec for loading bitmap");

    return NULL;
}

static bool st_bitmap_save(const st_bitmap_t *bitmap, const char *filename) {
    st_bitmap_simple_t *module = bitmap->module;

    for (size_t i = 0; i < module->codecs_count; i++) {
        st_bitmap_simple_codec_t *codec = &module->codecs[i];

        if (codec->save(codec->ctx, bitmap, filename))
            return true;
    }

    return false;
}

static const char *st_bitmap_memsave(void *dst, size_t *size,
 const st_bitmap_t *bitmap) {
    st_bitmap_simple_t *module = bitmap->module;

    for (size_t i = 0; i < module->codecs_count; i++) {
        st_bitmap_simple_codec_t *codec = &module->codecs[i];
        const char               *fmtname = codec->memsave(codec->ctx, dst,
         size, bitmap);

        if (fmtname)
            return fmtname;
    }

    return NULL;
}

static unsigned pxfmt_to_bytes_per_pixel(st_pxfmt_t pixel_format) {
    switch (pixel_format) {
        case PF_RGBA:
            return 4;
        case PF_UNKNOWN:
        default:
            return 0;
    }
}

static st_bitmap_t *st_bitmap_import(st_modctx_t *bitmap_ctx, const void *data,
 unsigned width, unsigned height, st_pxfmt_t pixel_format) {
    st_bitmap_simple_t *module = bitmap_ctx->data;
    unsigned            bytes_per_pixel;
    size_t              data_size;
    st_bitmap_t        *bitmap;
    errno_t             err;

    if (width == 0 || height == 0) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to create bitmap. Incorrect sizes");

        return NULL;
    }

    bytes_per_pixel = pxfmt_to_bytes_per_pixel(pixel_format);
    if (bytes_per_pixel == 0) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to create bitmap. Unsupported pixel format");

        return NULL;
    }

    data_size = bytes_per_pixel * width * height;

    bitmap = malloc(sizeof(st_bitmap_t) + data_size);
    if (!bitmap) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to allocate memory for bitmap pixels: %s",
         strerror(errno));

        return NULL;
    }

    err = memcpy_s(bitmap->data, data_size, data, data_size);
    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to copy bitmap pixels: %s", err_msg_buf);
        free(bitmap);

        return NULL;
    }

    bitmap->module = bitmap_ctx->data;
    bitmap->width = width;
    bitmap->height = height;
    bitmap->pixel_format = (int)pixel_format;

    return bitmap;
}

static void st_bitmap_destroy(st_bitmap_t *bitmap) {
    free(bitmap);
}

static const void *st_bitmap_get_data(const st_bitmap_t *bitmap) {
    return bitmap ? bitmap->data : NULL;
}

static unsigned st_bitmap_get_width(const st_bitmap_t *bitmap) {
    return bitmap ? bitmap->width : 0;
}

static unsigned st_bitmap_get_height(const st_bitmap_t *bitmap) {
    return bitmap ? bitmap->height : 0;
}

static st_pxfmt_t st_bitmap_get_pixel_format(const st_bitmap_t *bitmap) {
    return bitmap ? (st_pxfmt_t)bitmap->pixel_format : PF_UNKNOWN;
}
