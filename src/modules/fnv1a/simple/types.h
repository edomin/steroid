#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/object.h"

ST_MODCTX (
    struct st_loggerctx_s *logger_ctx;
) st_fnv1actx_t;

#define ST_FNV1ACTX_T_DEFINED
