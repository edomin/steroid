#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"

typedef st_modctx_t *(*st_png_init_t)(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx);
typedef void (*st_png_quit_t)(st_modctx_t *png_ctx);

typedef st_bitmap_t *(*st_png_load_t)(st_modctx_t *png_ctx,
 const char *filename);
typedef st_bitmap_t *(*st_png_memload_t)(st_modctx_t *png_ctx,
 const void *data, size_t size);
typedef bool (*st_png_save_t)(st_modctx_t *png_ctx, const st_bitmap_t *bitmap,
 const char *filename);
typedef const char *(*st_png_memsave_t)(st_modctx_t *png_ctx, void *dst,
 size_t *size, const st_bitmap_t *bitmap);

typedef struct {
    st_png_init_t    png_init;
    st_png_quit_t    png_quit;
    st_png_load_t    png_load;
    st_png_memload_t png_memload;
    st_png_save_t    png_save;
    st_png_memsave_t png_memsave;
} st_png_funcs_t;
