#ifndef ST_MODULES_FNV1_FNV_HASH_FNV_HASH_H
#define ST_MODULES_FNV1_FNV_HASH_FNV_HASH_H

#include "config.h" // IWYU pragma: keep
#include "steroids/fnv1.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_fnv1_fnv_hash_logger_t;

typedef struct {
    st_fnv1_fnv_hash_logger_t logger;
} st_fnv1_fnv_hash_t;

st_fnv1_funcs_t st_fnv1_fnv_hash_funcs = {
    .fnv1_init                  = st_fnv1_init,
    .fnv1_quit                  = st_fnv1_quit,
    .fnv1_u32hash_string        = st_fnv1_u32hash_string,
    .fnv1_u32hash_bytes         = st_fnv1_u32hash_bytes,
    .fnv1_get_u32hashstr_func   = st_fnv1_get_u32hashstr_func,
    .fnv1_get_u32hashbytes_func = st_fnv1_get_u32hashbytes_func,
};

#define FUNCS_COUNT 6
st_modfuncstbl_t st_module_fnv1_fnv_hash_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_fnv1_init"                 , st_fnv1_init},
        {"st_fnv1_quit"                 , st_fnv1_quit},
        {"st_fnv1_u32hash_string"       , st_fnv1_u32hash_string},
        {"st_fnv1_u32hash_bytes"        , st_fnv1_u32hash_bytes},
        {"st_fnv1_get_u32hashstr_func"  , st_fnv1_get_u32hashstr_func},
        {"st_fnv1_get_u32hashbytes_func", st_fnv1_get_u32hashbytes_func},
    }
};

#endif /* ST_MODULES_FNV1_FNV_HASH_FNV_HASH_H */
