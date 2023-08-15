#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <sys/queue.h>

st_list_t *st_list_create(size_t data_size) {
    st_list_t *list = malloc(sizeof(st_list_t));

    if (!list)
        return NULL;

    SLIST_INIT(&list->head); // NOLINT(altera-unroll-loops)

    return list;
}

void st_list_destroy(st_list_t *list) {
    st_list_clear(list);
    free(list);
}

bool st_list_insert_head(st_list_t *list, void *data) {
    st_node_t *node = malloc(sizeof(st_node_t));

    if (!node)
        return false;

    node->data = data;
    SLIST_INSERT_HEAD(&list->head, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)

    return true;
}

st_node_t *st_list_get_first(const st_list_t *list) {
    return SLIST_FIRST(&list->head);
}

st_node_t *st_list_get_next(const st_node_t *node) {
    return SLIST_NEXT(node, ST_SNODE_NEXT);
}

void st_list_remove(st_list_t *list, st_node_t *node) {
    SLIST_REMOVE(&list->head, node, st_node_s, ST_SNODE_NEXT);
    free(node);
}

void st_list_clear(st_list_t *list) {
    while (!SLIST_EMPTY(&list->head)) {
        st_node_t *node = SLIST_FIRST(&list->head);

        SLIST_REMOVE_HEAD(&list->head, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
        free(node);
    }
}

void *st_list_get_data(const st_node_t *node) {
    return node->data;
}