#include "inih.h"

#include <inih.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#define ERR_MSG_BUF_SIZE 1024

// typedef struct {
//     st_hash_table_hash_table_t *module;
//     struct hash_table          *handle;
// } st_hashtable_t;

// typedef struct {
//    st_hashtable_t    *hash_table;
//    struct hash_entry *handle;
// } st_htiter_t;

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_ini_inih_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table =
     &st_module_ini_inih_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_ini_inih_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"ini_inih\": %s\n",
         err_msg_buf);

        return NULL;
    }

    return &st_module_ini_inih_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_ini_inih_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_ini_import_functions(st_modctx_t *ini_ctx, st_modctx_t *fnv1_ctx,
 st_modctx_t *hash_table_ctx, st_modctx_t *logger_ctx) {
    st_ini_inih_t         *ini_module = ini_ctx->data;
    st_fnv1_funcs_t       *fnv1_funcs = (st_fnv1_funcs_t *)fnv1_ctx->funcs;
    st_hash_table_funcs_t *ht_funcs =
     (st_hash_table_funcs_t *)hash_table_ctx->funcs;
    st_logger_funcs_t     *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    ini_module->fnv1.get_u32hashstr_func = fnv1_funcs->get_u32hashstr_func;

    ini_module->hash_table.create         = ht_funcs->hash_table_create;
    ini_module->hash_table.destroy        = ht_funcs->hash_table_destroy;
    ini_module->hash_table.insert         = ht_funcs->hash_table_insert;
    ini_module->hash_table.find           = ht_funcs->hash_table_find;
    ini_module->hash_table.remove         = ht_funcs->hash_table_remove;
    ini_module->hash_table.next           = ht_funcs->hash_table_next;
    ini_module->hash_table.get_iter_key   = ht_funcs->hash_table_get_iter_key;
    ini_module->hash_table.get_iter_value = ht_funcs->hash_table_get_iter_value;

    ini_module->logger.debug = logger_funcs->logger_debug;
    ini_module->logger.info  = logger_funcs->logger_info;
    ini_module->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_ini_init(st_modctx_t *fnv1_ctx,
 st_modctx_t *hash_table_ctx, st_modctx_t *logger_ctx) {
    st_modctx_t   *ini_ctx;
    st_ini_inih_t *module;

    ini_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_ini_inih_data, sizeof(st_ini_inih_t));

    if (ini_ctx == NULL)
        return NULL;

    ini_ctx->funcs = &st_ini_inih_funcs;

    st_ini_import_functions(ini_ctx, fnv1_ctx, hash_table_ctx, logger_ctx);
    module = ini_ctx->data;
    module->fnv1.ctx       = fnv1_ctx;
    module->hash_table.ctx = hash_table_ctx;
    module->logger.ctx     = logger_ctx;

    module->logger.info(module->logger.ctx, "ini_inih: module initialized.");

    return ini_ctx;
}

static void st_ini_quit(st_modctx_t *ini_ctx) {
    st_ini_inih_t *module = ini_ctx->data;

    module->logger.info(module->logger.ctx, "ini_inih: module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, ini_ctx);
}

static void *st_hash_table_create(st_modctx_t *hash_table_ctx,
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