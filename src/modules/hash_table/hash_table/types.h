#ifndef ST_MODULES_HASH_TABLE_HASH_TABLE_TYPES_H
#define ST_MODULES_HASH_TABLE_HASH_TABLE_TYPES_H

#include "steroids/types/modules/logger.h"

typedef void (*st_freefunc_t)(void *ptr);

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_hash_table_hash_table_logger_t;

typedef struct {
    st_hash_table_hash_table_logger_t logger;
} st_hash_table_hash_table_t;

typedef struct {
    st_hash_table_hash_table_t *module;
    struct hash_table          *handle;
    st_freefunc_t               keydelfunc;
    st_freefunc_t               valdelfunc;
} st_hashtable_t;

typedef struct {
    st_hashtable_t    *hash_table;
    struct hash_entry *handle;
} st_htiter_t;

#define ST_FREEFUNC_T_DEFINED
#define ST_HASHTABLE_T_DEFINED
#define ST_HTITER_T_DEFINED

#endif /* ST_MODULES_HASH_TABLE_HASH_TABLE_TYPES_H */