#include "slist.h"

#include <stdbool.h>
#include <stdlib.h>
#include <sys/queue.h>

bool st_slist_insert_after(st_slnode_t *node, void *data);
st_slnode_t *st_slist_get_next(const st_slnode_t *node);
void st_slist_clear(st_slist_t *list);

st_slist_t *st_slist_create(size_t data_size) {
    st_slist_t *list = malloc(sizeof(st_slist_t));

    if (!list)
        return NULL;

    SLIST_INIT(&list->head); // NOLINT(altera-unroll-loops)
    list->tail = NULL;

    return list;
}

void st_slist_destroy(st_slist_t *list) {
    st_slist_clear(list);
    free(list);
}

bool st_slist_insert_head(st_slist_t *list, void *data) {
    st_slnode_t *node = malloc(sizeof(st_slnode_t));

    if (!node)
        return false;

    node->data = data;
    node->list = list;
    SLIST_INSERT_HEAD(&list->head, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)

    if (!list->tail)
        list->tail = node;

    return true;
}

bool st_slist_insert_tail(st_slist_t *list, void *data) {
    return st_slist_insert_after(list->tail, data);
}

bool st_slist_insert_after(st_slnode_t *node, void *data) {
    st_slnode_t *new_node = malloc(sizeof(st_slnode_t));
    st_slnode_t *next;
    st_slist_t  *list = node->list;

    if (!new_node)
        return false;

    new_node->data = data;
    new_node->list = list;

    SLIST_INSERT_AFTER(node, new_node, ST_SNODE_NEXT);

    next = st_slist_get_next(node);
    if (!next)
        list->tail = new_node;

    return true;
}

st_slnode_t *st_slist_get_first(const st_slist_t *list) {
    return list ? SLIST_FIRST(&list->head) : NULL;
}

st_slnode_t *st_slist_get_next(const st_slnode_t *node) {
    return SLIST_NEXT(node, ST_SNODE_NEXT);
}

void st_slist_remove_head(st_slist_t *list) {
    st_slnode_t *node = SLIST_FIRST(&list->head);

    SLIST_REMOVE_HEAD(&list->head, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
    if (node == list->tail)
        list->tail = NULL;
    free(node);
}

void st_slist_clear(st_slist_t *list) {
    while (!SLIST_EMPTY(&list->head)) {
        st_slnode_t *node = SLIST_FIRST(&list->head);

        SLIST_REMOVE_HEAD(&list->head, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
        free(node);
    }
}

void *st_slist_get_data(const st_slnode_t *node) {
    return node->data;
}

bool st_slist_empty(const st_slist_t *list) {
    return !list || SLIST_EMPTY(&list->head);
}
