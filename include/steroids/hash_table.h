#ifndef ST_STEROIDS_HASH_TABLE_H
#define ST_STEROIDS_HASH_TABLE_H

#include "steroids/module.h"
#include "steroids/types/modules/hash_table.h"

static st_modctx_t *st_hash_table_init(st_modctx_t *logger_ctx);
static void st_hash_table_quit(st_modctx_t *hash_table_ctx);

static st_hashtable_t *st_hash_table_create(st_modctx_t *hash_table_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc, st_freefunc_t keydelfunc,
 st_freefunc_t valdelfunc);
static void st_hash_table_destroy(st_hashtable_t *hash_table);
static bool st_hash_table_insert(st_hashtable_t *hash_table, st_htiter_t *iter,
 const void *key, void *value);
static void *st_hash_table_get(st_hashtable_t *hash_table, const void *key);
static bool st_hash_table_remove(st_hashtable_t *hash_table, const void *key);
static void st_hash_table_clear(st_hashtable_t *hash_table);
static bool st_hash_table_contains(st_hashtable_t *hash_table, const void *key);
static bool st_hash_table_find(st_hashtable_t *hash_table, st_htiter_t *dst,
 const void *key);
static bool st_hash_table_next(st_hashtable_t *hash_table, st_htiter_t *dst,
 st_htiter_t *current);
static const void *st_hash_table_get_iter_key(const st_htiter_t *iter);
static void *st_hash_table_get_iter_value(const st_htiter_t *iter);

#endif /* ST_STEROIDS_HASH_TABLE_H */
