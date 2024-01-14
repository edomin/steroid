#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/render.h"
#include "steroids/types/modules/sprite.h"

static st_modctx_t *st_render_init(st_modctx_t *drawq_ctx,
 st_modctx_t *dynarr_ctx, st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx,
 st_modctx_t *texture_ctx, st_gfxctx_t *gfxctx);
static void st_render_quit(st_modctx_t *render_ctx);

static void st_render_put_sprite(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float hscale,
 float vscale, float pivot_x, float pivot_y);
static void st_render_put_sprite_angled(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float hscale,
 float vscale, float angle, float pivot_x, float pivot_y);
static void st_render_process(st_modctx_t *render_ctx);
