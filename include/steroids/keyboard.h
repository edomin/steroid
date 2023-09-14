#ifndef ST_STEROIDS_KEYBOARD_H
#define ST_STEROIDS_KEYBOARD_H

#include "steroids/module.h"
#include "steroids/types/modules/keyboard.h"
#include "steroids/types/modules/window.h"

static st_modctx_t *st_keyboard_init(st_modctx_t *events_ctx,
 st_modctx_t *logger_ctx);
static void st_keyboard_quit(st_modctx_t *keyboard_ctx);

static void st_keyboard_process(st_modctx_t *keyboard_ctx);
static bool st_keyboard_press(const st_modctx_t *keyboard_ctx, st_key_t key);
static bool st_keyboard_release(const st_modctx_t *keyboard_ctx, st_key_t key);
static bool st_keyboard_pressed(const st_modctx_t *keyboard_ctx, st_key_t key);
static const char *st_keyboard_get_input(const st_modctx_t *keyboard_ctx);

#endif /* ST_STEROIDS_KEYBOARD_H */
