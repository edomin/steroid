#pragma once

#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/pathtools.h"
#include "steroids/types/object.h"

ST_MODCTX (
    struct st_loggerctx_s *logger_ctx;
    st_pathtoolsctx_t     *pathtools_ctx;
) st_fsctx_t;

#define ST_FSCTX_T_DEFINED
