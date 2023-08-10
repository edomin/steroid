#ifndef ST_STEROIDS_BITMAP_H
#define ST_STEROIDS_BITMAP_H

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"

static st_modctx_t *st_bitmap_init(st_modctx_t *logger_ctx);
static void st_bitmap_quit(st_modctx_t *bitmap_ctx);

static void st_bitmap_add_codec(st_modctx_t *bitmap_ctx,
 st_modctx_t *codec_ctx);
static st_bitmap_t *st_bitmap_load(st_modctx_t *bitmap_ctx,
 const char *filename);
static st_bitmap_t *st_bitmap_memload(st_modctx_t *bitmap_ctx, const void *data,
 size_t size);
static bool st_bitmap_save(const st_bitmap_t *bitmap, const char *filename);
static const char *st_bitmap_memsave(void *dst, size_t *size,
 const st_bitmap_t *bitmap);
static st_bitmap_t *st_bitmap_import(st_modctx_t *bitmap_ctx, const void *data,
 unsigned width, unsigned height, st_pxfmt_t pixel_format);
static void st_bitmap_destroy(st_bitmap_t *bitmap);
static const void *st_bitmap_get_data(const st_bitmap_t *bitmap);
static unsigned st_bitmap_get_width(const st_bitmap_t *bitmap);
static unsigned st_bitmap_get_height(const st_bitmap_t *bitmap);
static st_pxfmt_t st_bitmap_get_pixel_format(const st_bitmap_t *bitmap);

#endif /* ST_STEROIDS_BITMAP_H */
