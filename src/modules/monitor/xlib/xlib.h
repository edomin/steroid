#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"  // IWYU pragma: keep
#include "steroids/monitor.h"

static st_monitorctx_funcs_t st_monitor_xlib_funcs = {
    .monitor_init               = st_monitor_init,
    .monitor_quit               = st_monitor_quit,
    .monitor_get_monitors_count = st_monitor_get_monitors_count,
    .monitor_open               = st_monitor_open,
};

static st_modfuncentry_t st_module_monitor_xlib_funcs[] = {
    {"init"              , st_monitor_init},
    {"quit"              , st_monitor_quit},
    {"get_monitors_count", st_monitor_get_monitors_count},
    {"open"              , st_monitor_open},
    {"release"           , st_monitor_release},
    {"get_width"         , st_monitor_get_width},
    {"get_height"        , st_monitor_get_height},
    {"get_handle"        , st_monitor_get_handle},
    {"set_userdata"      , st_monitor_set_userdata},
    {"get_userdata"      , st_monitor_get_userdata},
    {NULL, NULL},
};
