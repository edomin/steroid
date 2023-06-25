#ifndef ST_MODULES_HASH_TABLE_HASH_TABLE_HASH_TABLE_H
#define ST_MODULES_HASH_TABLE_HASH_TABLE_HASH_TABLE_H

#include "config.h" // IWYU pragma: keep
#include "steroids/hash_table.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_hash_table_hash_table_logger_t;

typedef struct {
    st_hash_table_hash_table_logger_t logger;
} st_hash_table_hash_table_t;

st_hash_table_funcs_t st_hash_table_hash_table_funcs = {
    .hash_table_init           = st_hash_table_init,
    .hash_table_quit           = st_hash_table_quit,
    .hash_table_create         = st_hash_table_create,
    .hash_table_destroy        = st_hash_table_destroy,
    .hash_table_insert         = st_hash_table_insert,
    .hash_table_find           = st_hash_table_find,
    .hash_table_remove         = st_hash_table_remove,
    .hash_table_next           = st_hash_table_next,
    .hash_table_get_iter_key   = st_hash_table_get_iter_key,
    .hash_table_get_iter_value = st_hash_table_get_iter_value,
};

#define FUNCS_COUNT 10
st_modfuncstbl_t st_module_hash_table_hash_table_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_hash_table_init"          , st_hash_table_init},
        {"st_hash_table_quit"          , st_hash_table_quit},
        {"st_hash_table_create"        , st_hash_table_create},
        {"st_hash_table_destroy"       , st_hash_table_destroy},
        {"st_hash_table_insert"        , st_hash_table_insert},
        {"st_hash_table_find"          , st_hash_table_find},
        {"st_hash_table_remove"        , st_hash_table_remove},
        {"st_hash_table_next"          , st_hash_table_next},
        {"st_hash_table_get_iter_key"  , st_hash_table_get_iter_key},
        {"st_hash_table_get_iter_value", st_hash_table_get_iter_value},
    }
};

#endif /* ST_MODULES_HASH_TABLE_HASH_TABLE_HASH_TABLE_H */
