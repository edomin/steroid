#ifndef ST_MODULES_WINDOW_XLIB_TYPES_H
#define ST_MODULES_WINDOW_XLIB_TYPES_H

#include <X11/Xlib.h>

#include "steroids/types/modules/events.h"
#include "steroids/types/modules/logger.h"

#include "slist.h"

typedef enum {
    EV_FOCUS_IN = 0,
    EV_FOCUS_OUT,
    EV_RESIZE,
    EV_PLACE_ON_TOP,
    EV_PLACE_ON_BOTTOM,
    EV_CREATE,
    EV_DESTROY,
    // EV_MOVE,
    EV_SHOW,
    EV_HIDE,

    EV_MAX,
} evtype_index_t;

typedef struct {
    st_modctx_t              *ctx;
    st_events_register_type_t register_type;
    st_events_push_t          push;
} st_window_xlib_events_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_window_xlib_logger_t;

typedef struct {
    st_modctx_t *ctx;
} st_window_xlib_monitor_t;

typedef struct {
    st_window_xlib_events_t  events;
    st_window_xlib_logger_t  logger;
    st_window_xlib_monitor_t monitor;
    st_slist_t              *windows;
    st_evtypeid_t            evtypes[EV_MAX];
} st_window_xlib_t;

typedef struct {
    void    *private;
    Display *handle;
    Window   root_window;
} st_monitor_t;

typedef struct {
    st_window_xlib_t *module;
    Window            handle;
    st_monitor_t     *monitor;
    Atom              wm_delete_msg;
    bool              xed;
} st_window_t;

#define ST_WINDOW_T_DEFINED
#define ST_MONITOR_T_DEFINED

#endif /* ST_MODULES_WINDOW_XLIB_TYPES_H */