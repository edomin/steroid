#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/object.h"

typedef void (*st_freefunc_t)(void *ptr);

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_htable_hash_table_logger_t;

typedef struct {
    st_htable_hash_table_logger_t logger;
} st_htable_hash_table_t;

ST_CLASS (
    struct hash_table *handle;
    st_freefunc_t      keydelfunc;
    st_freefunc_t      valdelfunc;
) st_htable_t;

ST_CLASS (
    struct hash_entry *handle;
) st_htiter_t;

#define ST_FREEFUNC_T_DEFINED
#define ST_HTABLE_T_DEFINED
#define ST_HTITER_T_DEFINED
