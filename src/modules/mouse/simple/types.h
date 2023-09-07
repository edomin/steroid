#ifndef ST_MODULES_MOUSE_SIMPLE_TYPES_H
#define ST_MODULES_MOUSE_SIMPLE_TYPES_H

#include "steroids/types/modules/events.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/mouse.h"
#include "steroids/types/modules/window.h"

typedef enum {
    EV_MOUSE_PRESS = 0,
    EV_MOUSE_RELEASE,
    EV_MOUSE_WHEEL,
    EV_MOUSE_MOVE,
    EV_MOUSE_ENTER,
    EV_MOUSE_LEAVE,

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
} st_mouse_simple_events_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_mouse_simple_logger_t;

typedef struct {
    st_mouse_simple_events_t events;
    st_mouse_simple_logger_t logger;
    st_evtypeid_t            evtypes[EV_MAX];
    st_evq_t                *evq;
    unsigned                 x;
    unsigned                 y;
    bool                     prev_mbstate[ST_MB_MAX];
    bool                     curr_mbstate[ST_MB_MAX];
    int                      wheel;
    bool                     move;
    bool                     enter;
    bool                     leave;
    const st_window_t       *current_window;
} st_mouse_simple_t;

#endif /* ST_MODULES_MOUSE_SIMPLE_TYPES_H */