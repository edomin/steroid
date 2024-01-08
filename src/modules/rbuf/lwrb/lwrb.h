#pragma once

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

st_modfuncentry_t st_module_rbuf_lwrb_funcs[] = {
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
    {NULL, NULL},
};
