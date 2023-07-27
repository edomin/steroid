#ifndef ST_MODULES_MONITOR_XLIB_XLIB_H
#define ST_MODULES_MONITOR_XLIB_XLIB_H

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/monitor.h"

st_monitor_funcs_t st_monitor_xlib_funcs = {
    .monitor_init               = st_monitor_init,
    .monitor_quit               = st_monitor_quit,
    .monitor_get_monitors_count = st_monitor_get_monitors_count,
    .monitor_open               = st_monitor_open,
    .monitor_release            = st_monitor_release,
    .monitor_get_width          = st_monitor_get_width,
    .monitor_get_height         = st_monitor_get_height,
    .monitor_get_handle         = st_monitor_get_handle,
};

#define FUNCS_COUNT 8
st_modfuncstbl_t st_module_monitor_xlib_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init"              , st_monitor_init},
        {"quit"              , st_monitor_quit},
        {"get_monitors_count", st_monitor_get_monitors_count},
        {"open"              , st_monitor_open},
        {"release"           , st_monitor_release},
        {"get_width"         , st_monitor_get_width},
        {"get_height"        , st_monitor_get_height},
        {"get_handle"        , st_monitor_get_handle},
    }
};

#endif /* ST_MODULES_MONITOR_XLIB_XLIB_H */
