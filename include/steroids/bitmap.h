#ifndef ST_STEROIDS_BITMAP_H
#define ST_STEROIDS_BITMAP_H

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"

static st_modctx_t *st_bitmap_init(st_modctx_t *logger_ctx);
static void st_bitmap_quit(st_modctx_t *bitmap_ctx);

static st_bitmap_t *st_bitmap_import(st_modctx_t *bitmap_ctx, const void *data,
 unsigned width, unsigned height, st_pxfmt_t pixel_format);
static void st_bitmap_destroy(st_bitmap_t *bitmap);
static void *st_bitmap_get_data(st_bitmap_t *bitmap);
static unsigned st_bitmap_get_width(st_bitmap_t *bitmap);
static unsigned st_bitmap_get_height(st_bitmap_t *bitmap);
static st_pxfmt_t st_bitmap_get_pixel_format(st_bitmap_t *bitmap);

#endif /* ST_STEROIDS_BITMAP_H */
