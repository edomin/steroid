#ifndef ST_MODULES_MONITOR_XLIB_TYPES_H
#define ST_MODULES_MONITOR_XLIB_TYPES_H

#include <X11/Xlib.h>

#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_monitor_xlib_logger_t;

typedef struct {
    st_monitor_xlib_logger_t logger;
} st_monitor_xlib_t;

typedef struct {
    st_monitor_xlib_t *module;
    Display           *display;
    Window             root_window;
} st_monitor_t;

#define ST_MONITOR_T_DEFINED

#endif /* ST_MODULES_MONITOR_XLIB_TYPES_H */