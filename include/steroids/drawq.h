#ifndef ST_STEROIDS_DRAWQ_H
#define ST_STEROIDS_DRAWQ_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/drawq.h"

static st_modctx_t *st_drawq_init(st_modctx_t *dynarr_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *sprite_ctx);
static void st_drawq_quit(st_modctx_t *drawq_ctx);

static st_drawq_t *st_drawq_create(const st_modctx_t *drawq_ctx);
static void st_drawq_destroy(st_drawq_t *drawq);
static bool st_drawq_empty(const st_drawq_t *drawq);
static bool st_drawq_export_entry(const st_drawq_t *drawq,
 st_drawrec_t *drawrec, size_t index);
static bool st_drawq_add(st_drawq_t *drawq, st_sprite_t *sprite, float x,
 float y, float z, float scale, float angle);
static bool st_drawq_sort(st_drawq_t *drawq);
static bool st_drawq_clear(st_drawq_t *drawq);

#endif /* ST_STEROIDS_DRAWQ_H */
