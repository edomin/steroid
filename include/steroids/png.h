#ifndef ST_STEROIDS_PNG_H
#define ST_STEROIDS_PNG_H

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/png.h"

static st_modctx_t *st_png_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx);
static void st_png_quit(st_modctx_t *png_ctx);

static st_bitmap_t *st_png_load(st_modctx_t *png_ctx, const char *filename);
static st_bitmap_t *st_png_memload(st_modctx_t *png_ctx, const void *data,
 size_t size);
static bool st_png_save(st_modctx_t *png_ctx, const st_bitmap_t *bitmap,
 const char *filename);
static const char *st_png_memsave(st_modctx_t *png_ctx, void *dst, size_t *size,
 const st_bitmap_t *bitmap);

#endif /* ST_STEROIDS_PNG_H */
