#pragma once

#include <stddef.h>

struct st_dlist_s;

typedef struct st_dlnode_s {
    struct st_dlist_s  *list;
    struct st_dlnode_s *prev;
    struct st_dlnode_s *next;
    char                data[];
} st_dlnode_t;

typedef struct st_dlist_s {
    st_dlnode_t *head;
    st_dlnode_t *tail;
    size_t       data_size;
    size_t       elems_count;
    void       (*elem_free_func)(void *);
} st_dlist_t;
