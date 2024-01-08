#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/sprite.h"

typedef st_modctx_t *(*st_render_init_t)(st_modctx_t *drawq_ctx,
 st_modctx_t *dynarr_ctx, st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx,
 st_modctx_t *texture_ctx, st_gfxctx_t *gfxctx);
typedef void (*st_render_quit_t)(st_modctx_t *render_ctx);

typedef void (*st_render_put_sprite_t)(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float hscale,
 float vscale, float pivot_x, float pivot_y);
typedef void (*st_render_put_sprite_angled_t)(st_modctx_t *render_ctx,
 const st_sprite_t *sprite, float x, float y, float z, float hscale,
 float vscale, float angle, float pivot_x, float pivot_y);
typedef void (*st_render_process_t)(st_modctx_t *render_ctx);

typedef struct {
    st_render_init_t              render_init;
    st_render_quit_t              render_quit;
    st_render_put_sprite_t        render_put_sprite;
    st_render_put_sprite_angled_t render_put_sprite_angled;
    st_render_process_t           render_process;
} st_render_funcs_t;
