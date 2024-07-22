#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h" // IWYU pragma: keep
#include "steroids/htable.h"

static st_htablectx_funcs_t st_htable_hash_table_funcs = {
    .htable_init   = st_htable_init,
    .htable_quit   = st_htable_quit,
    .htable_create = st_htable_create,
};

static st_modfuncentry_t st_module_htable_hash_table_funcs[] = {
    {"init"          , st_htable_init},
    {"quit"          , st_htable_quit},
    {"create"        , st_htable_create},
    {"destroy"       , st_htable_destroy},
    {"insert"        , st_htable_insert},
    {"get"           , st_htable_get},
    {"remove"        , st_htable_remove},
    {"clear"         , st_htable_clear},
    {"contains"      , st_htable_contains},
    {"find"          , st_htable_find},
    {"first"         , st_htable_first},
    {"next"          , st_htable_next},
    {"get_iter_key"  , st_htable_get_iter_key},
    {"get_iter_value", st_htable_get_iter_value},
    {NULL, NULL},
};
