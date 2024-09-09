#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/object.h"

#include "dlist.h"

typedef struct {
    st_modctx_t        *ctx;
    st_logger_debug_t   debug;
    st_logger_info_t    info;
    st_logger_warning_t warning;
    st_logger_error_t   error;
} st_so_simple_logger_t;

typedef struct {
    st_so_simple_logger_t logger;
    st_dlist_t           *opened_handles;
} st_so_simple_t;

ST_CLASS (
    void *handle;
) st_so_t;

#define ST_SO_T_DEFINED
