#include "fnv_hash.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fnv_hash.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_fnv1a_fnv_hash_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_fnv1a_fnv_hash_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_fnv1a_fnv_hash_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"fnv1a_fnv_hash\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_fnv1a_fnv_hash_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_fnv1a_fnv_hash_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_fnv1a_import_functions(st_modctx_t *fnv1a_ctx,
 st_modctx_t *logger_ctx) {
    st_fnv1a_fnv_hash_t *module = fnv1a_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "fnv1a_fnh_hash: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("fnv1a_fnv_hash", logger, debug);
    ST_LOAD_FUNCTION("fnv1a_fnv_hash", logger, info);

    return true;
}

static st_modctx_t *st_fnv1a_init(st_modctx_t *logger_ctx) {
    st_modctx_t         *fnv1a_ctx;
    st_fnv1a_fnv_hash_t *fnv1a;

    fnv1a_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_fnv1a_fnv_hash_data, sizeof(st_fnv1a_fnv_hash_t));

    if (fnv1a_ctx == NULL)
        return NULL;

    fnv1a_ctx->funcs = &st_fnv1a_fnv_hash_funcs;

    fnv1a = fnv1a_ctx->data;
    fnv1a->logger.ctx = logger_ctx;

    if (!st_fnv1a_import_functions(fnv1a_ctx, logger_ctx))
        return NULL;

    fnv1a->logger.info(fnv1a->logger.ctx, "%s",
     "fnv1a_fnv_hash: FNV-1a hasher initialized.");

    return fnv1a_ctx;
}

static void st_fnv1a_quit(st_modctx_t *fnv1a_ctx) {
    st_fnv1a_fnv_hash_t *fnv1a = fnv1a_ctx->data;

    fnv1a->logger.info(fnv1a->logger.ctx, "%s",
     "fnv1a_simple: FNV-1a hasher destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, fnv1a_ctx);
}

static uint32_t st_fnv1a_u32hash_string(
 __attribute__((unused)) st_modctx_t *fnv1a_ctx, const char *str) {
    return fnv1_hash_string(str);
}

static uint32_t st_fnv1a_u32hash_bytes(
 __attribute__((unused)) st_modctx_t *fnv1a_ctx, const void *ptr, size_t size) {
    return fnv1_hash_data(ptr, size);
}

static st_u32hashstrfunc_t st_fnv1a_get_u32hashstr_func(
 __attribute__((unused)) st_modctx_t *fnv1a_ctx) {
    return fnv1_hash_string;
}

static st_u32hashbytesfunc_t st_fnv1a_get_u32hashbytes_func(
 __attribute__((unused)) st_modctx_t *fnv1a_ctx) {
    return fnv1_hash_data;
}