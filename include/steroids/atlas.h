#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/atlas.h"
#include "steroids/types/modules/texture.h"

static st_modctx_t *st_atlas_init(st_modctx_t *logger_ctx,
 st_modctx_t *texture_ctx);
static void st_atlas_quit(st_modctx_t *atlas_ctx);

static st_atlas_t *st_atlas_create(st_modctx_t *atlas_ctx,
 st_texture_t *texture, size_t clips_count);
static void st_atlas_destroy(st_atlas_t *atlas);
static bool st_atlas_set_clip(st_atlas_t *atlas, size_t clip_num, unsigned x,
 unsigned y, unsigned width, unsigned height);
static const st_texture_t *st_atlas_get_texture(const st_atlas_t *atlas);
static size_t st_atlas_get_clips_count(const st_atlas_t *atlas);
static unsigned st_atlas_get_clip_x(const st_atlas_t *atlas, size_t clip_num);
static unsigned st_atlas_get_clip_y(const st_atlas_t *atlas, size_t clip_num);
static unsigned st_atlas_get_clip_width(const st_atlas_t *atlas,
 size_t clip_num);
static unsigned st_atlas_get_clip_height(const st_atlas_t *atlas,
 size_t clip_num);
