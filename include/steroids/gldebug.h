#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/gfxctx.h"
#include "steroids/types/modules/gldebug.h"

static st_modctx_t *st_gldebug_init(st_modctx_t *logger_ctx,
 st_gfxctx_t *gfxctx);
static void st_gldebug_quit(st_modctx_t *gldebug_ctx);

static void st_gldebug_label_buffer(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label);
static void st_gldebug_label_shader(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label);
static void st_gldebug_label_shdprog(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label);
static void st_gldebug_label_vao(const st_modctx_t *gldebug_ctx, unsigned id,
 const char *label);
static void st_gldebug_label_pipeline(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label);
static void st_gldebug_label_texture(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label);
static void st_gldebug_label_framebuffer(const st_modctx_t *gldebug_ctx,
 unsigned id, const char *label);
static void st_gldebug_unlabel_buffer(const st_modctx_t *gldebug_ctx,
 unsigned id);
static void st_gldebug_unlabel_shader(const st_modctx_t *gldebug_ctx,
 unsigned id);
static void st_gldebug_unlabel_shdprog(const st_modctx_t *gldebug_ctx,
 unsigned id);
static void st_gldebug_unlabel_vao(const st_modctx_t *gldebug_ctx, unsigned id);
static void st_gldebug_unlabel_pipeline(const st_modctx_t *gldebug_ctx,
 unsigned id);
static void st_gldebug_unlabel_texture(const st_modctx_t *gldebug_ctx,
 unsigned id);
static void st_gldebug_unlabel_framebuffer(const st_modctx_t *gldebug_ctx,
 unsigned id);
static const char *st_gldebug_get_error_msg(const st_modctx_t *gldebug_ctx);
