#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/bmcodec.h"

static st_modctx_t *st_bmcodec_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx);
static void st_bmcodec_quit(st_modctx_t *bmcodec_ctx);

static int st_bmcodec_get_priority(st_modctx_t *bmcodec_ctx);
static st_bitmap_t *st_bmcodec_load(st_modctx_t *bmcodec_ctx,
 const char *filename);
static st_bitmap_t *st_bmcodec_memload(st_modctx_t *bmcodec_ctx,
 const void *data, size_t size);
static bool st_bmcodec_save(st_modctx_t *bmcodec_ctx, const st_bitmap_t *bitmap,
 const char *filename, const char *format);
static bool st_bmcodec_memsave(st_modctx_t *bmcodec_ctx, void *dst,
 size_t *size, const st_bitmap_t *bitmap, const char *format);
