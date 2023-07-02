#ifndef ST_MODULES_INI_INIH_TYPES_H
#define ST_MODULES_INI_INIH_TYPES_H

#include "steroids/types/modules/fnv1a.h"
#include "steroids/types/modules/hash_table.h"
#include "steroids/types/modules/logger.h"

typedef struct {
    st_modctx_t                   *ctx;
    st_fnv1a_get_u32hashstr_func_t get_u32hashstr_func;
} st_ini_inih_fnv1a_t;

typedef struct {
    st_modctx_t                   *ctx;
    st_hash_table_create_t         create;
    st_hash_table_destroy_t        destroy;
    st_hash_table_insert_t         insert;
    st_hash_table_get_t            get;
    st_hash_table_remove_t         remove;
    st_hash_table_clear_t          clear;
    st_hash_table_contains_t       contains;
    st_hash_table_find_t           find;
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
    st_ini_inih_fnv1a_t      fnv1a;
    st_ini_inih_hash_table_t hash_table;
    st_ini_inih_logger_t     logger;
} st_ini_inih_t;

typedef struct {
    st_ini_inih_t  *module;
    st_hashtable_t *data;
} st_inisection_t;

typedef struct {
    st_ini_inih_t  *module;
    st_hashtable_t *sections;
} st_ini_t;

#define ST_INI_T_DEFINED

#endif /* ST_MODULES_INI_INIH_TYPES_H */