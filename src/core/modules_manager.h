#ifndef ST_SRC_CORE_MODULES_MANAGER_H
#define ST_SRC_CORE_MODULES_MANAGER_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#include <xmempool.h>
#pragma GCC diagnostic pop

#include "steroids/types/list.h"

typedef struct {
    st_slist_t         modules_data;
    xmem_pool_handle   ctx_pool;
} st_modsmgr_t;

st_modsmgr_t *st_modsmgr_init(void);
void st_modsmgr_destroy(st_modsmgr_t *modsmgr);
void *st_modsmgr_get_function(const void *modsmgr, const char *subsystem,
 const char *module_name, const char *func_name);

#endif
