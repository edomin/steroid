#include "simple.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ERRMSGBUF_SIZE 128
#define CODEC_NAME_SIZE 32
#define CODECS_COUNT   256

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

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

static bool st_bitmap_add_codec(st_modctx_t *bitmap_ctx,
 st_bitmap_simple_codec_t *codec) {
    st_bitmap_simple_t *module = bitmap_ctx->data;

    codec->load = global_modsmgr_funcs.get_function_from_ctx(global_modsmgr,
     codec->ctx, "load");
    if (!codec->load ) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"load\" from codec");

        return false;
    }

    codec->memload = global_modsmgr_funcs.get_function_from_ctx(global_modsmgr,
     codec->ctx, "memload");
    if (!codec->memload) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"memload\" from codec");

        return false;
    }

    codec->save = global_modsmgr_funcs.get_function_from_ctx(global_modsmgr,
     codec->ctx, "save");
    if (!codec->save) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"save\" from codec");

        return false;
    }

    codec->memsave = global_modsmgr_funcs.get_function_from_ctx(global_modsmgr,
     codec->ctx, "memsave");
    if (!codec->memsave) {
        module->logger.error(module->logger.ctx,
         "bitmap_simple: Unable to load function \"memsave\" from codec");

        return false;
    }

    return true;
}

static void st_bitmap_init_codecs(st_modctx_t *logger_ctx,
 st_modctx_t *bitmap_ctx) {
    st_bitmap_simple_t *module = bitmap_ctx->data;
    char                codecs_names[CODEC_NAME_SIZE][CODECS_COUNT] = {0};
    char               *pcodecsnames[CODECS_COUNT];
    char               *codec_name;

    for (size_t i = 0; i < CODECS_COUNT; i++)
        pcodecsnames[i] = codecs_names[i];

    module->codecs = st_slist_create(sizeof(st_bitmap_simple_codec_t));
    if (!module->codecs) {
        module->logger.info(module->logger.ctx,
         "bitmap_simple: Unable to create list of bitmap codecs");

        return;
    }

    module->logger.info(module->logger.ctx,
     "bitmap_simple: Searching bitmap codecs");

    global_modsmgr_funcs.get_module_names(global_modsmgr, pcodecsnames,
     CODECS_COUNT, CODEC_NAME_SIZE, "bmcodec");

    for (size_t i = 0; i < CODECS_COUNT; i++) {
        st_bmcodec_init_t         init_func;
        st_bmcodec_quit_t         quit_func;
        st_modctx_t              *ctx;
        st_bitmap_simple_codec_t *codec;

        codec_name = pcodecsnames[i];

        if (!*codec_name)
            break;

        module->logger.info(module->logger.ctx,
         "bitmap_simple: Found module \"bmcodec_%s\"", codec_name);

        init_func = global_modsmgr_funcs.get_function(global_modsmgr, "bmcodec",
         codec_name, "init");
        if (!init_func) {
            module->logger.error(module->logger.ctx,
             "bitmap_simple: Unable to get function \"init\" from module "
             "\"bmcodec_%s\"", codec_name);

            continue;
        }

        quit_func = global_modsmgr_funcs.get_function(global_modsmgr,
         "bmcodec", codec_name, "quit");
        if (!quit_func) {
            module->logger.error(module->logger.ctx,
             "bitmap_simple: Unable to get function \"quit\" from module "
             "\"bmcodec_%s\"", codec_name);

            continue;
        }

        ctx = init_func(bitmap_ctx, logger_ctx);

        codec = malloc(sizeof(st_bitmap_simple_codec_t));
        if (!codec) {
            char errbuf[ERRMSGBUF_SIZE];

            if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
                module->logger.error(module->logger.ctx,
                 "bitmap_simple: Unable to allocate memory for codec entry of "
                 "module \"bmcodec_%s\": %s", codec_name, errbuf);

            quit_func(ctx);

            continue;
        }
        codec->ctx = ctx;
        codec->quit = quit_func;

        if (!st_bitmap_add_codec(bitmap_ctx, codec)
         || !st_slist_insert_head(module->codecs, codec)) {
            module->logger.error(module->logger.ctx,
             "bitmap_simple: Unable to create entry node for module "
             "\"bmcodec_%s\"", codec_name);
            free(codec);
            quit_func(ctx);

            continue;
        }
    }
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

    st_bitmap_init_codecs(logger_ctx, bitmap_ctx);

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

static st_bitmap_t *st_bitmap_load(st_modctx_t *bitmap_ctx,
 const char *filename) {
    st_bitmap_simple_t *module = bitmap_ctx->data;
    st_slnode_t        *node = st_slist_get_first(module->codecs);

    while (node) {
        st_bitmap_simple_codec_t *codec = st_slist_get_data(node);
        st_bitmap_t              *bitmap = codec->load(codec->ctx, filename);

        if (bitmap)
            return bitmap;

        node = st_slist_get_next(node);
    }

    module->logger.error(module->logger.ctx,
     "bitmap_simple: No suitable codec for loading bitmap \"%s\"", filename);

    return NULL;
}

static st_bitmap_t *st_bitmap_memload(st_modctx_t *bitmap_ctx, const void *data,
 size_t size) {
    st_bitmap_simple_t *module = bitmap_ctx->data;
    st_slnode_t        *node = st_slist_get_first(module->codecs);

    while (node) {
        st_bitmap_simple_codec_t *codec = st_slist_get_data(node);
        st_bitmap_t              *bitmap = codec->memload(codec->ctx, data,
         size);

        if (bitmap)
            return bitmap;

        node = st_slist_get_next(node);
    }

    module->logger.error(module->logger.ctx,
     "bitmap_simple: No suitable codec for loading bitmap");

    return NULL;
}

static bool st_bitmap_save(const st_bitmap_t *bitmap, const char *filename,
 const char *format) {
    st_bitmap_simple_t *module = bitmap->module;
    st_slnode_t        *node = st_slist_get_first(module->codecs);

    while (node) {
        st_bitmap_simple_codec_t *codec = st_slist_get_data(node);

        if (codec->save(codec->ctx, bitmap, filename, format))
            return true;

        node = st_slist_get_next(node);
    }

    return false;
}

static bool st_bitmap_memsave(void *dst, size_t *size,
 const st_bitmap_t *bitmap, const char *format) {
    st_bitmap_simple_t *module = bitmap->module;
    st_slnode_t        *node = st_slist_get_first(module->codecs);

    while (node) {
        st_bitmap_simple_codec_t *codec = st_slist_get_data(node);

        if (codec->memsave(codec->ctx, dst, size, bitmap, format))
            return true;

        node = st_slist_get_next(node);
    }

    return false;
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
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "bitmap_simple: Unable to allocate memory for bitmap pixels: %s",
             errbuf);

        return NULL;
    }

    memcpy(bitmap->data, data, data_size);
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
