#ifndef ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H
#define ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H

#include <stdint.h>

#include "steroids/module.h"

#ifndef ST_HASHTABLE_T_DEFINED
    typedef struct st_hashtable_s st_hashtable_t;
#endif
#ifndef ST_HTITER_T_DEFINED
    typedef struct st_htiter_s st_htiter_t;
#endif

typedef uint32_t (*st_u32hashfunc_t)(const void *str);
typedef int (*st_keyeqfunc_t)(const void *left, const void *right);
typedef void (*st_freefunc_t)(void *ptr);

typedef st_modctx_t *(*st_hash_table_init_t)(st_modctx_t *logger_ctx);
typedef void (*st_hash_table_quit_t)(st_modctx_t *hash_table_ctx);
typedef st_hashtable_t *(*st_hash_table_create_t)(st_modctx_t *hash_table_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc);
typedef void (*st_hash_table_destroy_t)(st_hashtable_t *hash_table,
 st_freefunc_t valdelfunc);
typedef bool (*st_hash_table_insert_t)(st_hashtable_t *hash_table,
 const void *key, void *value);
typedef void *(*st_hash_table_get_t)(st_hashtable_t *hash_table,
 const void *key);
typedef bool (*st_hash_table_remove_t)(st_hashtable_t *hash_table,
 const void *key);
typedef bool (*st_hash_table_find_t)(st_hashtable_t *hash_table,
 st_htiter_t *dst, const void *key);
typedef bool (*st_hash_table_next_t)(st_hashtable_t *hash_table,
 st_htiter_t *dst, st_htiter_t *current);
typedef const void *(*st_hash_table_get_iter_key_t)(const st_htiter_t *iter);
typedef void *(*st_hash_table_get_iter_value_t)(const st_htiter_t *iter);

typedef struct {
    st_hash_table_init_t           hash_table_init;
    st_hash_table_quit_t           hash_table_quit;
    st_hash_table_create_t         hash_table_create;
    st_hash_table_destroy_t        hash_table_destroy;
    st_hash_table_insert_t         hash_table_insert;
    st_hash_table_get_t            hash_table_get;
    st_hash_table_remove_t         hash_table_remove;
    st_hash_table_find_t           hash_table_find;
    st_hash_table_next_t           hash_table_next;
    st_hash_table_get_iter_key_t   hash_table_get_iter_key;
    st_hash_table_get_iter_value_t hash_table_get_iter_value;
} st_hash_table_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_HASH_TABLE_H */
