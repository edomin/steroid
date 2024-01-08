#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"

#define ST_EVTYPE_ID_NONE -1

typedef int st_evtypeid_t;

#ifndef ST_EVQ_T_DEFINED
    typedef struct st_evq_s st_evq_t;
#endif

typedef st_modctx_t *(*st_events_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_events_quit_t)(st_modctx_t *events_ctx);

typedef st_evtypeid_t (*st_events_register_type_t)(st_modctx_t *events_ctx,
 const char *type_name, size_t size);
typedef st_evtypeid_t (*st_events_get_type_id_t)(st_modctx_t *events_ctx,
 const char *type_name);
typedef st_evq_t *(*st_events_create_queue_t)(st_modctx_t *events_ctx,
 size_t pool_size);
typedef void (*st_events_destroy_queue_t)(st_evq_t *queue);
typedef bool (*st_events_subscribe_t)(st_evq_t *queue, st_evtypeid_t type_id);
typedef void (*st_events_unsubscribe_t)(st_evq_t *queue, st_evtypeid_t type_id);
typedef void (*st_events_unsubscribe_all_t)(st_evq_t *queue);
typedef void (*st_events_suspend_t)(st_evq_t *queue, bool clear);
typedef void (*st_events_resume_t)(st_evq_t *queue);
typedef void (*st_events_push_t)(st_modctx_t *events_ctx, st_evtypeid_t type_id,
 const void *data);
typedef bool (*st_events_is_empty_t)(const st_evq_t *queue);
typedef st_evtypeid_t (*st_events_peek_type_t)(const st_evq_t *queue);
typedef bool (*st_events_pop_t)(st_evq_t *queue, void *data);
typedef bool (*st_events_drop_t)(st_evq_t *queue);
typedef bool (*st_events_clear_t)(st_evq_t *queue);

typedef struct {
    st_events_init_t            events_init;
    st_events_quit_t            events_quit;
    st_events_register_type_t   events_register_type;
    st_events_get_type_id_t     events_get_type_id;
    st_events_create_queue_t    events_create_queue;
    st_events_destroy_queue_t   events_destroy_queue;
    st_events_subscribe_t       events_subscribe;
    st_events_unsubscribe_t     events_unsubscribe;
    st_events_unsubscribe_all_t events_unsubscribe_all;
    st_events_suspend_t         events_suspend;
    st_events_resume_t          events_resume;
    st_events_push_t            events_push;
    st_events_is_empty_t        events_is_empty;
    st_events_peek_type_t       events_peek_type;
    st_events_pop_t             events_pop;
    st_events_drop_t            events_drop;
    st_events_clear_t           events_clear;
} st_events_funcs_t;
