#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/bitmap.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/texture.h"

static st_modctx_t *st_texture_init(st_modctx_t *bitmap_ctx,
 st_modctx_t *logger_ctx, st_gapi_t api);
static void st_texture_quit(st_modctx_t *texture_ctx);

static st_texture_t *st_texture_load(st_modctx_t *texture_ctx,
 const char *filename);
static st_texture_t *st_texture_memload(st_modctx_t *texture_ctx,
 const void *data, size_t size);
static void st_texture_destroy(st_texture_t *texture);
static bool st_texture_bind(const st_texture_t *texture);
static unsigned st_texture_get_width(const st_texture_t *texture);
static unsigned st_texture_get_height(const st_texture_t *texture);
