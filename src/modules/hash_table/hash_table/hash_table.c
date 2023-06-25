#include "hash_table.h"

#include <errno.h>
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

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_hash_table_hash_table_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table =
     &st_module_hash_table_hash_table_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_hash_table_hash_table_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"hash_table_hash_table\": %s\n",
         err_msg_buf);

        return NULL;
    }

    return &st_module_hash_table_hash_table_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_hash_table_hash_table_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_hash_table_import_functions(st_modctx_t *hash_table_ctx,
 st_modctx_t *logger_ctx) {
    st_hash_table_hash_table_t *ht_module = hash_table_ctx->data;
    st_logger_funcs_t *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    ht_module->logger.debug = logger_funcs->logger_debug;
    ht_module->logger.info  = logger_funcs->logger_info;
    ht_module->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_hash_table_init(st_modctx_t *logger_ctx) {
    st_modctx_t                *hash_table_ctx;
    st_hash_table_hash_table_t *module;

    hash_table_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_hash_table_hash_table_data, sizeof(st_hash_table_hash_table_t));

    if (hash_table_ctx == NULL)
        return NULL;

    hash_table_ctx->funcs = &st_hash_table_hash_table_funcs;

    st_hash_table_import_functions(hash_table_ctx, logger_ctx);
    module = hash_table_ctx->data;
    module->logger.ctx = logger_ctx;

    module->logger.info(module->logger.ctx,
     "hash_table_hash_table: module initialized.");

    return hash_table_ctx;
}

static void st_hash_table_quit(st_modctx_t *hash_table_ctx) {
    st_hash_table_hash_table_t *module = hash_table_ctx->data;

    module->logger.info(module->logger.ctx,
     "hash_table_hash_table: module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, hash_table_ctx);
}

static st_hashtable_t *st_hash_table_create(st_modctx_t *hash_table_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc) {
    st_hash_table_hash_table_t *module = hash_table_ctx->data;
    struct hash_table          *handle = hash_table_create(hashfunc, keyeqfunc);
    st_hashtable_t             *hash_table;

    if (!handle) {
        module->logger.error(module->logger.ctx,
         "hash_table_hash_table: unable to create hash table handle");

        return NULL;
    }

    hash_table = malloc(sizeof(st_hashtable_t));
    if (!hash_table) {
        module->logger.error(module->logger.ctx,
         "hash_table_hash_table: unable allocate memory for hash_table");
        hash_table_destroy(handle, NULL);

        return NULL;
    }

    hash_table->module = module;
    hash_table->handle = handle;

    return hash_table;
}

static void st_hash_table_destroy(st_hashtable_t *hash_table,
 st_freefunc_t valdelfunc) {
    struct hash_entry *entry = hash_table_next_entry(hash_table->handle, NULL);

    while (entry) {
        if (valdelfunc)
            valdelfunc(entry->data);
        hash_table_remove_entry(hash_table->handle, entry);
        hash_table_next_entry(hash_table->handle, NULL);
    }

    hash_table_destroy(hash_table->handle, NULL);
}

static bool st_hash_table_insert(st_hashtable_t *hash_table, const void *key,
 void *value) {
    return hash_table_insert(hash_table->handle, key, value) != NULL;
}

static void *st_hash_table_find(st_hashtable_t *hash_table, const void *key) {
    struct hash_entry *entry = hash_table_search(hash_table->handle, key);

    return entry ? entry->data : NULL;
}

static bool st_hash_table_remove(st_hashtable_t *hash_table, const void *key) {
    struct hash_entry *entry = hash_table_search(hash_table->handle, key);

    if (entry)
        hash_table_remove_entry(hash_table->handle, entry);

    return !!entry;
}

static bool st_hash_table_next(st_htiter_t *dst, st_hashtable_t *hash_table,
 st_htiter_t *current) {
    struct hash_entry *entry = hash_table_next_entry(hash_table->handle,
     current ? current->handle : NULL);

    if (!entry)
        return false;

    dst->hash_table = hash_table;
    dst->handle = entry;

    return true;
}

static const void *st_hash_table_get_iter_key(const st_htiter_t *iter) {
    return iter->handle->key;
}

static void *st_hash_table_get_iter_value(const st_htiter_t *iter) {
    return iter->handle->data;
}