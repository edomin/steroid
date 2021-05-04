#ifndef ST_INCLUDE_STEROIDS_TYPES_LIST_H
#define ST_INCLUDE_STEROIDS_TYPES_LIST_H

#include "steroids/stdlib/sys/queue.h"

#define ST_SNODE_NEXT next

typedef struct st_snode_t {
    void                   *data;
    SLIST_ENTRY(st_snode_t) ST_SNODE_NEXT;
} st_snode_t;

typedef SLIST_HEAD(st_slist_t, st_snode_t) st_slist_t;

#endif
