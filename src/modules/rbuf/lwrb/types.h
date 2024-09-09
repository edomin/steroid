#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#include <lwrb.h>
#pragma GCC diagnostic pop

#include "steroids/types/modules/logger.h"
#include "steroids/types/object.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_rbuf_lwrb_logger_t;

typedef struct {
    st_rbuf_lwrb_logger_t logger;
} st_rbuf_lwrb_t;

ST_CLASS (
    lwrb_t   handle;
    bool     can_grow;
    uint8_t *data;
) st_rbuf_t;

#define ST_RBUF_T_DEFINED
