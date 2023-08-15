#ifndef ST_SRC_SLIST_H
#define ST_SRC_SLIST_H

#include <stdbool.h>
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
    size_t      data_size;
    st_slhead_t head;
} st_slist_t;

st_slist_t *st_slist_create(size_t data_size);
void st_slist_destroy(st_slist_t *list);
bool st_slist_insert_head(st_slist_t *list, void *data);
st_slnode_t *st_slist_get_first(const st_slist_t *list);
st_slnode_t *st_slist_get_next(const st_slnode_t *node);
void st_slist_remove(st_slnode_t *node);
void st_slist_remove_head(st_slist_t *list);
void st_slist_clear(st_slist_t *list);
void *st_slist_get_data(const st_slnode_t *node);
bool st_slist_empty(const st_slist_t *list);

#endif /* ST_SRC_SLIST_H */
