#pragma once

#include <stddef.h>
#include <sys/queue.h>

#define ST_SNODE_NEXT next

typedef struct st_slnode_s {
    void                    *data;
    struct st_slist_s       *list;
    SLIST_ENTRY(st_slnode_s) ST_SNODE_NEXT;
} st_slnode_t;

typedef SLIST_HEAD(st_slhead_s, st_slnode_s) st_slhead_t;

typedef struct st_slist_s {
    size_t       data_size;
    st_slhead_t  head;
    st_slnode_t *tail;
} st_slist_t;
