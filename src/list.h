#ifndef ST_SRC_LIST_H
#define ST_SRC_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/queue.h>

#define ST_SNODE_NEXT next

typedef struct st_node_s {
    void                   *data;
    SLIST_ENTRY(st_node_s) ST_SNODE_NEXT;
} st_node_t;

typedef SLIST_HEAD(st_list_head_s, st_node_s) st_list_head_t;

typedef struct {
    size_t         data_size;
    st_list_head_t head;
} st_list_t;

st_list_t *st_list_create(size_t data_size);
void st_list_destroy(st_list_t *list);
bool st_list_insert_head(st_list_t *list, void *data);
st_node_t *st_list_get_first(const st_list_t *list);
st_node_t *st_list_get_next(const st_node_t *node);
void st_list_remove(st_list_t *list, st_node_t *node);
void st_list_clear(st_list_t *list);
void *st_list_get_data(const st_node_t *node);

#endif /* ST_SRC_LIST_H */