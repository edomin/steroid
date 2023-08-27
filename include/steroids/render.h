#ifndef ST_STEROIDS_RENDER_H
#define ST_STEROIDS_RENDER_H

#include "steroids/module.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/render.h"
#include "steroids/types/modules/sprite.h"

static st_modctx_t *st_render_init(st_modctx_t *drawq_ctx,
 st_modctx_t *dynarr_ctx, st_modctx_t *gfxctx_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *sprite_ctx, st_modctx_t *texture_ctx, st_modctx_t *window_ctx);
static void st_render_quit(st_modctx_t *render_ctx);

static void st_render_put_sprite(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float angle);
static void st_render_process(st_modctx_t *render_ctx);

#endif /* ST_STEROIDS_RENDER_H */
