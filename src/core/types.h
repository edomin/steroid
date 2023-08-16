#ifndef ST_SRC_CORE_TYPES_H
#define ST_SRC_CORE_TYPES_H

#include "slist.h"

typedef struct {
    st_slist_t *modules_data;
} st_modsmgr_t;

#define ST_MODSMGR_T_DEFINED

#endif /* ST_SRC_CORE_TYPES_H */
