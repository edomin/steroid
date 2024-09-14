#pragma once

#include "steroids/types/modules/fnv1a.h"
#include "steroids/types/modules/htable.h"
#include "steroids/types/modules/logger.h"

ST_MODCTX (
    st_fnv1actx_t         *fnv1a_ctx;
    st_htablectx_t        *htable_ctx;
    struct st_loggerctx_s *logger_ctx;
) st_inictx_t;

typedef struct {
    st_inictx_t *ctx;
    st_htable_t *data;
} st_inisection_t;

ST_CLASS (
    st_htable_t *sections;
) st_ini_t;

#define ST_INICTX_T_DEFINED
#define ST_INI_T_DEFINED
