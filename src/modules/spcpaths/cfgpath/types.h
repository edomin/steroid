#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/modctx.h"

ST_MODCTX (
    struct st_loggerctx_s *logger_ctx;
) st_spcpathsctx_t;

#define ST_SPCPATHSCTX_T_DEFINED
