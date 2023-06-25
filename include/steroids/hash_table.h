#ifndef ST_STEROIDS_HASH_TABLE_H
#define ST_STEROIDS_HASH_TABLE_H

#include "steroids/module.h"
#include "steroids/types/modules/hash_table.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_hash_table_init(st_modctx_t *logger_ctx);
ST_VISIBILITY void st_hash_table_quit(st_modctx_t *hash_table_ctx);

ST_VISIBILITY st_hashtable_t *st_hash_table_create(st_modctx_t *hash_table_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc);
ST_VISIBILITY void st_hash_table_destroy(st_hashtable_t *hash_table,
 st_freefunc_t valdelfunc);
ST_VISIBILITY bool st_hash_table_insert(st_hashtable_t *hash_table,
 const void *key, void *value);
ST_VISIBILITY void *st_hash_table_find(st_hashtable_t *hash_table,
 const void *key);
ST_VISIBILITY bool st_hash_table_remove(st_hashtable_t *hash_table,
 const void *key);
ST_VISIBILITY bool st_hash_table_next(st_htiter_t *dst,
 st_hashtable_t *hash_table, st_htiter_t *current);
ST_VISIBILITY const void *st_hash_table_get_iter_key(const st_htiter_t *iter);
ST_VISIBILITY void *st_hash_table_get_iter_value(const st_htiter_t *iter);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_HASH_TABLE_H */
