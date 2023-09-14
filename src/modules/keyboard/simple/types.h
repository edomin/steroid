#ifndef ST_MODULES_KEYBOARD_SIMPLE_TYPES_H
#define ST_MODULES_KEYBOARD_SIMPLE_TYPES_H

#include "steroids/types/modules/events.h"
#include "steroids/types/modules/fnv1a.h"
#include "steroids/types/modules/htable.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/keyboard.h"

#define INPUT_SIZE 4

typedef enum {
    EV_KEY_PRESS = 0,
    EV_KEY_RELEASE,
    EV_KEY_INPUT,

    EV_MAX,
} evtype_index_t;

typedef struct {
    st_modctx_t                *ctx;
    st_events_get_type_id_t     get_type_id;
    st_events_create_queue_t    create_queue;
    st_events_destroy_queue_t   destroy_queue;
    st_events_subscribe_t       subscribe;
    st_events_is_empty_t        is_empty;
    st_events_peek_type_t       peek_type;
    st_events_pop_t             pop;
} st_keyboard_simple_events_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_keyboard_simple_logger_t;

typedef struct {
    st_fnv1a_get_u32hashstr_func_t get_u32hashstr_func;
} st_keyboard_simple_fnv1a_t;

typedef struct {
    st_modctx_t               *ctx;
    st_htable_init_t           init;
    st_htable_quit_t           quit;
    st_htable_create_t         create;
    st_htable_destroy_t        destroy;
    st_htable_insert_t         insert;
    st_htable_contains_t       contains;
    st_htable_get_t            get;
    st_htable_next_t           next;
    st_htable_get_iter_key_t   get_iter_key;
    st_htable_get_iter_value_t get_iter_value;
} st_keyboard_simple_htable_t;

typedef struct {
    st_keyboard_simple_events_t events;
    st_keyboard_simple_logger_t logger;
    st_keyboard_simple_fnv1a_t  fnv1a;
    st_keyboard_simple_htable_t htable;
    st_evtypeid_t               evtypes[EV_MAX];
    st_evq_t                   *evq;
    st_htable_t                *prev_state;
    st_htable_t                *cur_state;
    char                        input[INPUT_SIZE];
} st_keyboard_simple_t;

#endif /* ST_MODULES_KEYBOARD_SIMPLE_TYPES_H */