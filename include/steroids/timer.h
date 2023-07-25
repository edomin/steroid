#ifndef ST_STEROIDS_TIMER_H
#define ST_STEROIDS_TIMER_H

#include "steroids/module.h"
#include "steroids/types/modules/timer.h"

static st_modctx_t *st_timer_init(st_modctx_t *logger_ctx);
static void st_timer_quit(st_modctx_t *timer_ctx);

static uint64_t st_timer_start(st_modctx_t *timer_ctx);
static unsigned st_timer_get_elapsed(st_modctx_t *timer_ctx, uint64_t start);
static void st_timer_sleep(st_modctx_t *timer_ctx, unsigned ms);
static void st_timer_sleep_for_fps(st_modctx_t *timer_ctx, unsigned fps);

#endif /* ST_STEROIDS_TIMER_H */
