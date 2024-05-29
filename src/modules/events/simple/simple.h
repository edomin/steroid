#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/events.h"

static st_eventsctx_funcs_t st_events_simple_funcs = {
    .quit          = st_events_quit,
    .register_type = st_events_register_type,
    .get_type_id   = st_events_get_type_id,
    .create_queue  = st_events_create_queue,
    .push          = st_events_push,
};

static st_modfuncentry_t st_module_events_simple_funcs[] = {
    {"init",            st_events_init},
    {"quit",            st_events_quit},
    {"register_type",   st_events_register_type},
    {"get_type_id",     st_events_get_type_id},
    {"create_queue",    st_events_create_queue},
    {"destroy_queue",   st_events_destroy_queue},
    {"subscribe",       st_events_subscribe},
    {"unsubscribe",     st_events_unsubscribe},
    {"unsubscribe_all", st_events_unsubscribe_all},
    {"suspend",         st_events_suspend},
    {"resume",          st_events_resume},
    {"push",            st_events_push},
    {"is_empty",        st_events_is_empty},
    {"peek_type",       st_events_peek_type},
    {"pop",             st_events_pop},
    {"drop",            st_events_drop},
    {"clear",           st_events_clear},
    {NULL, NULL},
};
