#include "hash_table.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hash_table.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

static st_htablectx_funcs_t htablectx_funcs = {
    .quit   = st_htable_quit,
    .create = st_htable_create,
};

static st_htable_funcs_t htable_funcs = {
    .destroy   = st_htable_destroy,
    .insert    = st_htable_insert,
    .get       = st_htable_get,
    .remove    = st_htable_remove,
    .clear     = st_htable_clear,
    .contains  = st_htable_contains,
    .find      = st_htable_find,
    .get_first = st_htable_first,
};

static st_htiter_funcs_t htiter_funcs = {
    .get_next  = st_htable_next,
    .get_key   = st_htable_get_iter_key,
    .get_value = st_htable_get_iter_value,
};

ST_MODULE_DEF_GET_FUNC(htable_hash_table)
ST_MODULE_DEF_INIT_FUNC(htable_hash_table)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_htable_hash_table_init(modsmgr, modsmgr_funcs);
}
#endif

static const char *st_module_subsystem = "htable";
static const char *st_module_name = "hash_table";

static st_htablectx_t *st_htable_init(struct st_loggerctx_s *logger_ctx) {
    st_htablectx_t *htable_ctx = st_modctx_new(st_module_subsystem,
     st_module_name, sizeof(st_htablectx_t), NULL, &htablectx_funcs);

    if (!htable_ctx) {
        ST_LOGGERCTX_CALL(logger_ctx, error,
         "htable_hash_table: unable to create new htable ctx object");

        return NULL;
    }

    htable_ctx->logger_ctx = logger_ctx;

    ST_LOGGERCTX_CALL(logger_ctx, info,
     "htable_hash_table: hash tables manipulation module context initialized");

    return htable_ctx;
}

static void st_htable_quit(st_htablectx_t *htable_ctx) {
    ST_LOGGERCTX_CALL(htable_ctx->logger_ctx, info,
     "htable_hash_table: hash tables manipulation module context destroyed");
    free(htable_ctx);
}

static st_htable_t *st_htable_create(st_htablectx_t *htable_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc, st_freefunc_t keydelfunc,
 st_freefunc_t valdelfunc) {
    struct hash_table *handle = hash_table_create(hashfunc, keyeqfunc);
    st_htable_t       *htable;

    if (!handle) {
        ST_LOGGERCTX_CALL(htable_ctx->logger_ctx, error,
         "htable_hash_table: Unable to create hash table handle");

        return NULL;
    }

    htable = malloc(sizeof(st_htable_t));
    if (!htable) {
        ST_LOGGERCTX_CALL(htable_ctx->logger_ctx, error,
         "htable_hash_table: Unable allocate memory for hash_table");
        hash_table_destroy(handle, NULL);

        return NULL;
    }

    st_object_make(htable, htable_ctx, &htable_funcs);
    htable->handle = handle;
    htable->keydelfunc = keydelfunc;
    htable->valdelfunc = valdelfunc;

    return htable;
}

static void st_htable_destroy(st_htable_t *htable) {
    if (htable) {
        ST_HTABLE_CALL(htable, clear);
        hash_table_destroy(htable->handle, NULL);
        free(htable);
    }
}

static bool st_htable_insert(st_htable_t *htable, st_htiter_t *iter,
 const void *key, void *value) {
    bool               delete_old;
    const void        *old_key;
    void              *old_value;
    struct hash_entry *entry = hash_table_search(htable->handle, key);

    if (entry) {
        delete_old = true;
        old_key = entry->key;
        old_value = entry->data;
    }

    entry = hash_table_insert(htable->handle, key, value);
    if (!entry)
        return false;

    if (iter) {
        st_object_make(iter, htable, &htiter_funcs);
        iter->handle = entry;
    }

    if (delete_old) {
        if (htable->keydelfunc) { /* keydelfunc fails without braces */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)old_key);
            #pragma GCC diagnostic pop
        }
        if (htable->valdelfunc)
            htable->valdelfunc(old_value);
    }

    return true;
}

static void *st_htable_get(st_htable_t *htable, const void *key) {
    st_htiter_t iter;

    if (ST_HTABLE_CALL(htable, find, &iter, key))
        return iter.handle->data;

    return NULL;
}

static bool st_htable_remove(st_htable_t *htable, const void *key) {
    struct hash_entry *entry = hash_table_search(htable->handle, key);

    if (entry) {
        if (htable->keydelfunc) { /* keydelfunc fails without braces */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)entry->key);
            #pragma GCC diagnostic pop
        }
        if (htable->valdelfunc)
            htable->valdelfunc(entry->data);

        hash_table_remove_entry(htable->handle, entry);
    }

    return !!entry;
}

static void st_htable_clear(st_htable_t *htable) {
    struct hash_entry *entry = hash_table_next_entry(htable->handle, NULL);

    while (entry) {
        if (htable->keydelfunc) { /* keydelfunc fails without braces */
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)entry->key);
            #pragma GCC diagnostic pop
        }
        if (htable->valdelfunc)
            htable->valdelfunc(entry->data);
        hash_table_remove_entry(htable->handle, entry);
        entry = hash_table_next_entry(htable->handle, entry);
    }
}

static bool st_htable_contains(st_htable_t *htable, const void *key) {
    return !!hash_table_search(htable->handle, key);
}

static bool st_htable_find(st_htable_t *htable, st_htiter_t *dst,
 const void *key) {
    struct hash_entry *handle;

    if (!dst)
        return false;

    handle = hash_table_search(htable->handle, key);
    if (!handle)
        return false;

    st_object_make(dst, htable, &htiter_funcs);
    dst->handle = handle;

    return true;
}

static bool st_htable_first_or_next(st_htable_t *htable, st_htiter_t *current,
 st_htiter_t *dst) {
    struct hash_entry *entry;

    if (!htable && !current)
        return false;

    entry = hash_table_next_entry(htable->handle,
     current ? current->handle : NULL);

    if (!entry)
        return false;

    st_object_make(dst, htable, &htiter_funcs);
    dst->handle = entry;

    return true;
}

static bool st_htable_first(st_htable_t *htable, st_htiter_t *dst) {
    return htable ? st_htable_first_or_next(htable, NULL, dst) : false;
}

static bool st_htable_next(st_htiter_t *current, st_htiter_t *dst) {
    return current
        ? st_htable_first_or_next(st_object_get_owner(current), current, dst)
        : false;
}

static const void *st_htable_get_iter_key(const st_htiter_t *iter) {
    return iter->handle->key;
}

static void *st_htable_get_iter_value(const st_htiter_t *iter) {
    return iter->handle->data;
}
