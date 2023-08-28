#ifndef ST_STEROIDS_SPRITE_H
#define ST_STEROIDS_SPRITE_H

#include "steroids/module.h"
#include "steroids/types/modules/atlas.h"
#include "steroids/types/modules/sprite.h"
#include "steroids/types/modules/texture.h"

static st_modctx_t *st_sprite_init(st_modctx_t *atlas_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *texture_ctx);
static void st_sprite_quit(st_modctx_t *sprite_ctx);

static st_sprite_t *st_sprite_create(st_modctx_t *sprite_ctx,
 const st_atlas_t *atlas, size_t clip_num);
static st_sprite_t *st_sprite_from_texture(st_modctx_t *sprite_ctx,
 const st_texture_t *texture);
static void st_sprite_destroy(st_sprite_t *sprite);
static const st_texture_t *st_sprite_get_texture(const st_sprite_t *sprite);
static unsigned st_sprite_get_width(const st_sprite_t *sprite);
static unsigned st_sprite_get_height(const st_sprite_t *sprite);
static void st_sprite_export_uv(const st_sprite_t *sprite, st_uv_t *dstuv);

#endif /* ST_STEROIDS_SPRITE_H */
