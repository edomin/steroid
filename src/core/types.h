#ifndef ST_SRC_CORE_TYPES_H
#define ST_SRC_CORE_TYPES_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#include <xmempool.h>
#pragma GCC diagnostic pop

#include "steroids/types/list.h"

typedef struct {
    st_slist_t       modules_data;
    xmem_pool_handle ctx_pool;
} st_modsmgr_t;

#define ST_MODSMGR_T_DEFINED

#endif /* ST_SRC_CORE_TYPES_H */