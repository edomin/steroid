#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/gfxctx.h"

static st_modctx_t *st_gfxctx_init(st_modctx_t *logger_ctx,
 st_modctx_t *monitor_ctx);
static void st_gfxctx_quit(st_modctx_t *gfxctx_ctx);

static st_gfxctx_t *st_gfxctx_create(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gapi_t api);
static st_gfxctx_t *st_gfxctx_create_shared(st_modctx_t *gfxctx_ctx,
 st_monitor_t *monitor, st_window_t *window, st_gfxctx_t *other);
static bool st_gfxctx_make_current(st_gfxctx_t *gfxctx);
static bool st_gfxctx_swap_buffers(st_gfxctx_t *gfxctx);
static st_window_t *st_gfxctx_get_window(st_gfxctx_t *gfxctx);
static st_gapi_t st_gfxctx_get_api(st_gfxctx_t *gfxctx);
static unsigned st_gfxctx_get_shared_index(const st_gfxctx_t *gfxctx);
static void st_gfxctx_destroy(st_gfxctx_t *gfxctx);
static bool st_gfxctx_debug_enabled(const st_gfxctx_t *gfxctx);
static void st_gfxctx_set_userdata(const st_gfxctx_t *gfxctx, const char *key,
 uintptr_t value);
static bool st_gfxctx_get_userdata(const st_gfxctx_t *gfxctx, uintptr_t *dst,
 const char *key);
