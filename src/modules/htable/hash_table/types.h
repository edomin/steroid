#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/object.h"

typedef void (*st_freefunc_t)(void *ptr);

ST_MODCTX (
    struct st_loggerctx_s *logger_ctx;
) st_htablectx_t;

ST_CLASS (
    struct hash_table *handle;
    st_freefunc_t      keydelfunc;
    st_freefunc_t      valdelfunc;
) st_htable_t;

ST_CLASS (
    struct hash_entry *handle;
) st_htiter_t;

#define ST_HTABLECTX_T_DEFINED
#define ST_FREEFUNC_T_DEFINED
#define ST_HTABLE_T_DEFINED
#define ST_HTITER_T_DEFINED
