#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/monitor.h"

static st_modctx_t *st_monitor_init(st_modctx_t *logger_ctx);
static void st_monitor_quit(st_modctx_t *monitor_ctx);

static unsigned st_monitor_get_monitors_count(st_modctx_t *monitor_ctx);
static st_monitor_t *st_monitor_open(st_modctx_t *monitor_ctx, unsigned index);
static void st_monitor_release(st_monitor_t *monitor);
static unsigned st_monitor_get_width(st_monitor_t *monitor);
static unsigned st_monitor_get_height(st_monitor_t *monitor);
static void *st_monitor_get_handle(st_monitor_t *monitor);
static void st_monitor_set_userdata(const st_monitor_t *monitor,
 const char *key, uintptr_t value);
static bool st_monitor_get_userdata(const st_monitor_t *monitor, uintptr_t *dst,
 const char *key);
