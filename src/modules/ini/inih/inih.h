#ifndef ST_MODULES_INI_INIH_INIH_H
#define ST_MODULES_INI_INIH_INIH_H

#include "config.h" // IWYU pragma: keep
#include "steroids/ini.h"
#include "steroids/types/modules/fnv1.h"
#include "steroids/types/modules/hash_table.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t                *ctx;
    st_fnv1_get_u32hashstr_func get_u32hashstr_func;
} st_ini_inih_fnv1_t;

typedef struct {
    st_modctx_t                   *ctx;
    st_hash_table_create_t         create;
    st_hash_table_destroy_t        destroy;
    st_hash_table_insert_t         insert;
    st_hash_table_find_t           find;
    st_hash_table_remove_t         remove;
    st_hash_table_next_t           next;
    st_hash_table_get_iter_key_t   get_iter_key;
    st_hash_table_get_iter_value_t get_iter_value;
} st_ini_inih_hash_table_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_ini_inih_logger_t;

typedef struct {
    st_ini_inih_fnv1_t       fnv1;
    st_ini_inih_hash_table_t hash_table;
    st_ini_inih_logger_t     logger;
} st_ini_inih_t;

st_ini_funcs_t st_ini_inih_funcs = {
    .ini_init           = st_ini_init,
    .ini_quit           = st_ini_quit,
    .ini_create         = st_ini_create,
    .ini_load           = st_ini_load,
    .ini_memload        = st_ini_memload,
    .ini_destroy        = st_ini_destroy,
    .ini_section_exists = st_ini_section_exists,
    .ini_key_exists     = st_ini_key_exists,
    .ini_get_str        = st_ini_get_str,
    .ini_delete_section = st_ini_delete_section,
    .ini_delete_key     = st_ini_delete_key,
    .ini_clear_section  = st_ini_clear_section,
    .ini_add_section    = st_ini_add_section,
    .ini_add_key        = st_ini_add_key,
    .ini_export         = st_ini_export,
    .ini_save           = st_ini_save,
};

#define FUNCS_COUNT 16
st_modfuncstbl_t st_module_ini_inih_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"st_ini_init"          , st_ini_init},
        {"st_ini_quit"          , st_ini_quit},
        {"st_ini_create"        , st_ini_create},
        {"st_ini_load"          , st_ini_load},
        {"st_ini_memload"       , st_ini_memload},
        {"st_ini_destroy"       , st_ini_destroy},
        {"st_ini_section_exists", st_ini_section_exists},
        {"st_ini_key_exists"    , st_ini_key_exists},
        {"st_ini_get_str"       , st_ini_get_str},
        {"st_ini_delete_section", st_ini_delete_section},
        {"st_ini_delete_key"    , st_ini_delete_key},
        {"st_ini_clear_section" , st_ini_clear_section},
        {"st_ini_add_section"   , st_ini_add_section},
        {"st_ini_add_key"       , st_ini_add_key},
        {"st_ini_export"        , st_ini_export},
        {"st_ini_save"          , st_ini_save},
    }
};

#endif /* ST_MODULES_INI_INIH_INIH_H */
