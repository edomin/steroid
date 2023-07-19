#ifndef ST_SRC_MODULES_EVENTS_SIMPLE_SIMPLE_H
#define ST_SRC_MODULES_EVENTS_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/events.h"

st_events_funcs_t st_events_simple_funcs = {
    .events_init            = st_events_init,
    .events_quit            = st_events_quit,
    .events_register_type   = st_events_register_type,
    .events_get_type_id     = st_events_get_type_id,
    .events_create_queue    = st_events_create_queue,
    .events_destroy_queue   = st_events_destroy_queue,
    .events_subscribe       = st_events_subscribe,
    .events_unsubscribe     = st_events_unsubscribe,
    .events_unsubscribe_all = st_events_unsubscribe_all,
    .events_suspend         = st_events_suspend,
    .events_resume          = st_events_resume,
    .events_push            = st_events_push,
    .events_is_empty        = st_events_is_empty,
    .events_peek_type       = st_events_peek_type,
    .events_pop             = st_events_pop,
    .events_drop            = st_events_drop,
    .events_clear           = st_events_clear,
};

#define FUNCS_COUNT 17
st_modfuncstbl_t st_module_events_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
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
    }
};

#endif /* ST_SRC_MODULES_EVENTS_SIMPLE_SIMPLE_H */
