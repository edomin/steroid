#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/modctx.h"
#include "steroids/types/object.h"

#include "dlist.h"

ST_MODCTX (
    struct st_loggerctx_s *logger_ctx;
    st_dlist_t            *opened_handles;
) st_soctx_t;

ST_CLASS (
    void *handle;
) st_so_t;

#define ST_SOCTX_T_DEFINED
#define ST_SO_T_DEFINED
