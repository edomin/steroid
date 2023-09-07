#ifndef ST_STEROIDS_MOUSE_H
#define ST_STEROIDS_MOUSE_H

#include "steroids/module.h"
#include "steroids/types/modules/mouse.h"
#include "steroids/types/modules/window.h"

static st_modctx_t *st_mouse_init(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx);
static void st_mouse_quit(st_modctx_t *mouse_ctx);

static void st_mouse_process(st_modctx_t *mouse_ctx);
static bool st_mouse_press(const st_modctx_t *mouse_ctx, st_mbutton_t button);
static bool st_mouse_release(const st_modctx_t *mouse_ctx, st_mbutton_t button);
static bool st_mouse_pressed(const st_modctx_t *mouse_ctx, st_mbutton_t button);
static int st_mouse_get_wheel_relative(const st_modctx_t *mouse_ctx);
static bool st_mouse_moved(const st_modctx_t *mouse_ctx);
static bool st_mouse_entered(const st_modctx_t *mouse_ctx);
static bool st_mouse_leaved(const st_modctx_t *mouse_ctx);
static unsigned st_mouse_get_x(const st_modctx_t *mouse_ctx);
static unsigned st_mouse_get_y(const st_modctx_t *mouse_ctx);
static const st_window_t *st_mouse_get_window(const st_modctx_t *mouse_ctx);

#endif /* ST_STEROIDS_MOUSE_H */
