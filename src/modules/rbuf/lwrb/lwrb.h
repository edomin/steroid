#ifndef ST_MODULES_RBUF_LWRB_LWRB_H
#define ST_MODULES_RBUF_LWRB_LWRB_H

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/rbuf.h"

st_rbuf_funcs_t st_rbuf_lwrb_funcs = {
    .rbuf_init           = st_rbuf_init,
    .rbuf_quit           = st_rbuf_quit,
    .rbuf_create         = st_rbuf_create,
    .rbuf_destroy        = st_rbuf_destroy,
    .rbuf_push           = st_rbuf_push,
    .rbuf_peek           = st_rbuf_peek,
    .rbuf_pop            = st_rbuf_pop,
    .rbuf_drop           = st_rbuf_drop,
    .rbuf_clear          = st_rbuf_clear,
    .rbuf_get_free_space = st_rbuf_get_free_space,
    .rbuf_is_empty       = st_rbuf_is_empty,
};

#define FUNCS_COUNT 11
st_modfuncstbl_t st_module_rbuf_lwrb_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init"          , st_rbuf_init},
        {"quit"          , st_rbuf_quit},
        {"create"        , st_rbuf_create},
        {"destroy"       , st_rbuf_destroy},
        {"push"          , st_rbuf_push},
        {"peek"          , st_rbuf_peek},
        {"pop"           , st_rbuf_pop},
        {"drop"          , st_rbuf_drop},
        {"clear"         , st_rbuf_clear},
        {"get_free_space", st_rbuf_get_free_space},
        {"is_empty"      , st_rbuf_is_empty},
    }
};

#endif /* ST_MODULES_RBUF_LWRB_LWRB_H */
