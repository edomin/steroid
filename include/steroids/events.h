#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/events.h"

static st_modctx_t *st_events_init(st_modctx_t *logger_ctx);
static void st_events_quit(st_modctx_t *events_ctx);

static st_evtypeid_t st_events_register_type(st_modctx_t *events_ctx,
 const char *type_name, size_t size);
static st_evtypeid_t st_events_get_type_id(st_modctx_t *events_ctx,
 const char *type_name);
static st_evq_t *st_events_create_queue(st_modctx_t *events_ctx,
 size_t pool_size);
static void st_events_destroy_queue(st_evq_t *queue);
static bool st_events_subscribe(st_evq_t *queue, st_evtypeid_t type_id);
static void st_events_unsubscribe(st_evq_t *queue, st_evtypeid_t type_id);
static void st_events_unsubscribe_all(st_evq_t *queue);
static void st_events_suspend(st_evq_t *queue, bool clear);
static void st_events_resume(st_evq_t *queue);
static void st_events_push(st_modctx_t *events_ctx, st_evtypeid_t type_id,
 const void *data);
static bool st_events_is_empty(const st_evq_t *queue);
static st_evtypeid_t st_events_peek_type(const st_evq_t *queue);
static bool st_events_pop(st_evq_t *queue, void *data);
static bool st_events_drop(st_evq_t *queue);
static bool st_events_clear(st_evq_t *queue);
