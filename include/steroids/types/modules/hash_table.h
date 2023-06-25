#ifndef ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H
#define ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H

#include <stdint.h>

#include "steroids/module.h"

typedef uint32_t (*st_u32hashfunc_t)(const void *str);
typedef int (*st_keyeqfunc_t)(const void *left, const void *right);
typedef void (*st_freefunc_t)(void *ptr);

typedef st_modctx_t *(*st_hash_table_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_hash_table_quit_t)(st_modctx_t *hash_table_ctx);
typedef void *(*st_hash_table_create_t)(st_modctx_t *hash_table_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc);
typedef void (*st_hash_table_destroy_t)(void *hash_table,
 st_freefunc_t valdelfunc);
typedef bool (*st_hash_table_insert_t)(void *hash_table, const void *key,
 void *value);
typedef void *(*st_hash_table_find_t)(void *hash_table, const void *key);
typedef bool (*st_hash_table_remove_t)(void *hash_table, const void *key);
typedef bool (*st_hash_table_next_t)(void *dst, void *hash_table,
 void *current);
typedef const void *(*st_hash_table_get_iter_key_t)(const void *iter);
typedef void *(*st_hash_table_get_iter_value_t)(const void *iter);

typedef struct {
    st_hash_table_init_t           hash_table_init;
    st_hash_table_quit_t           hash_table_quit;
    st_hash_table_create_t         hash_table_create;
    st_hash_table_destroy_t        hash_table_destroy;
    st_hash_table_insert_t         hash_table_insert;
    st_hash_table_find_t           hash_table_find;
    st_hash_table_remove_t         hash_table_remove;
    st_hash_table_next_t           hash_table_next;
    st_hash_table_get_iter_key_t   hash_table_get_iter_key;
    st_hash_table_get_iter_value_t hash_table_get_iter_value;
} st_hash_table_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H */
