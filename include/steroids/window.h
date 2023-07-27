#ifndef ST_STEROIDS_WINDOW_H
#define ST_STEROIDS_WINDOW_H

#include "steroids/module.h"
#include "steroids/types/modules/monitor.h"
#include "steroids/types/modules/window.h"

static st_modctx_t *st_window_init(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *monitor_ctx);
static void st_window_quit(st_modctx_t *window_ctx);

static st_window_t *st_window_create(st_modctx_t *window_ctx,
 st_monitor_t *monitor, int x, int y, unsigned width, unsigned height,
 bool fullscreen, const char *title);
static void st_window_destroy(st_window_t *window);
static void st_window_process(st_modctx_t *window_ctx);
static st_monitor_t *st_window_get_monitor(st_window_t *window);
static void *st_window_get_handle(st_window_t *window);

#endif /* ST_STEROIDS_WINDOW_H */
