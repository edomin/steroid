#include "hash_table.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hash_table.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

ST_MODULE_DEF_GET_FUNC(htable_hash_table)

st_moddata_t *st_module_htable_hash_table_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"htable_hash_table\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_htable_hash_table_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_htable_hash_table_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_htable_import_functions(st_modctx_t *htable_ctx,
 st_modctx_t *logger_ctx) {
    st_htable_hash_table_t *module = htable_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "htable_hash_table: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("htable_hash_table", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("htable_hash_table", logger, info);

    return true;
}

static st_modctx_t *st_htable_init(st_modctx_t *logger_ctx) {
    st_modctx_t            *htable_ctx;
    st_htable_hash_table_t *module;

    htable_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_htable_hash_table_data, sizeof(st_htable_hash_table_t));

    if (!htable_ctx)
        return NULL;

    htable_ctx->funcs = &st_htable_hash_table_funcs;

    module = htable_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_htable_import_functions(htable_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, htable_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "htable_hash_table: Module initialized.");

    return htable_ctx;
}

static void st_htable_quit(st_modctx_t *htable_ctx) {
    st_htable_hash_table_t *module = htable_ctx->data;

    module->logger.info(module->logger.ctx,
     "htable_hash_table: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, htable_ctx);
}

static st_htable_t *st_htable_create(st_modctx_t *htable_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc, st_freefunc_t keydelfunc,
 st_freefunc_t valdelfunc) {
    st_htable_hash_table_t *module = htable_ctx->data;
    struct hash_table          *handle = hash_table_create(hashfunc, keyeqfunc);
    st_htable_t             *htable;

    if (!handle) {
        module->logger.error(module->logger.ctx,
         "htable_hash_table: Unable to create hash table handle");

        return NULL;
    }

    htable = malloc(sizeof(st_htable_t));
    if (!htable) {
        module->logger.error(module->logger.ctx,
         "htable_hash_table: Unable allocate memory for hash_table");
        hash_table_destroy(handle, NULL);

        return NULL;
    }

    htable->module = module;
    htable->handle = handle;
    htable->keydelfunc = keydelfunc;
    htable->valdelfunc = valdelfunc;

    return htable;
}

static void st_htable_destroy(st_htable_t *htable) {
    st_htable_clear(htable);
    hash_table_destroy(htable->handle, NULL);
}

static bool st_htable_insert(st_htable_t *htable, st_htiter_t *iter,
 const void *key, void *value) {
    struct hash_entry *entry = hash_table_search(htable->handle, key);

    if (entry) {
        if (key != entry->key && htable->keydelfunc)
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)entry->key);
            #pragma GCC diagnostic pop
        if (value != entry->data && htable->valdelfunc)
            htable->valdelfunc(entry->data);
    }

    entry = hash_table_insert(htable->handle, key, value);
    if (!entry)
        return false;

    if (iter) {
        iter->htable = htable;
        iter->handle = entry;
    }

    return true;
}

static void *st_htable_get(st_htable_t *htable, const void *key) {
    st_htiter_t iter;

    if (st_htable_find(htable, &iter, key))
        return iter.handle->data;

    return NULL;
}

static bool st_htable_remove(st_htable_t *htable, const void *key) {
    struct hash_entry *entry = hash_table_search(htable->handle, key);

    if (entry) {
        if (htable->keydelfunc)
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)entry->key);
            #pragma GCC diagnostic pop
        if (htable->valdelfunc)
            htable->valdelfunc(entry->data);

        hash_table_remove_entry(htable->handle, entry);
    }

    return !!entry;
}

static void st_htable_clear(st_htable_t *htable) {
    struct hash_entry *entry = hash_table_next_entry(htable->handle, NULL);

    while (entry) {
        if (htable->keydelfunc)
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-qual"
            htable->keydelfunc((void *)entry->key);
            #pragma GCC diagnostic pop
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

    dst->handle = handle;

    return true;
}

static bool st_htable_next(st_htable_t *htable, st_htiter_t *dst,
 st_htiter_t *current) {
    struct hash_entry *entry = hash_table_next_entry(htable->handle,
     current ? current->handle : NULL);

    if (!entry)
        return false;

    dst->htable = htable;
    dst->handle = entry;

    return true;
}

static const void *st_htable_get_iter_key(const st_htiter_t *iter) {
    return iter->handle->key;
}

static void *st_htable_get_iter_value(const st_htiter_t *iter) {
    return iter->handle->data;
}