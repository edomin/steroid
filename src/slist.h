#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct st_slnode_s st_slnode_t;
typedef struct st_slist_s st_slist_t;

st_slist_t *st_slist_create(size_t data_size);
void st_slist_destroy(st_slist_t *list);
bool st_slist_insert_head(st_slist_t *list, void *data);
bool st_slist_insert_tail(st_slist_t *list, void *data);
bool st_slist_insert_after(st_slnode_t *node, void *data);
st_slnode_t *st_slist_get_first(const st_slist_t *list);
st_slnode_t *st_slist_get_next(const st_slnode_t *node);
void st_slist_remove_head(st_slist_t *list);
void st_slist_clear(st_slist_t *list);
void *st_slist_get_data(const st_slnode_t *node);
bool st_slist_empty(const st_slist_t *list);
