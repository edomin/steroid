#include "simple.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define OFFSET_BASIS 0x811C9dC5u
#define PRIME        0x01000193u

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(fnv1a_simple)
ST_MODULE_DEF_INIT_FUNC(fnv1a_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_fnv1a_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_fnv1a_import_functions(st_modctx_t *fnv1a_ctx,
 st_modctx_t *logger_ctx) {
    st_fnv1a_simple_t *module = fnv1a_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "fnv1a_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("fnv1a_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("fnv1a_simple", logger, info);

    return true;
}

static st_modctx_t *st_fnv1a_init(st_modctx_t *logger_ctx) {
    st_modctx_t       *fnv1a_ctx;
    st_fnv1a_simple_t *fnv1a;

    fnv1a_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_fnv1a_simple_data, sizeof(st_fnv1a_simple_t));

    if (fnv1a_ctx == NULL)
        return NULL;

    fnv1a_ctx->funcs = &st_fnv1a_simple_funcs;

    fnv1a = fnv1a_ctx->data;
    fnv1a->logger.ctx = logger_ctx;

    if (!st_fnv1a_import_functions(fnv1a_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, fnv1a_ctx);

        return NULL;
    }

    fnv1a->logger.info(fnv1a->logger.ctx,
     "fnv1a_simple: FNV-1a hasher initialized.");

    return fnv1a_ctx;
}

static uint32_t fnv1_hash_data(const void *ptr, size_t size) {
    uint32_t hash = OFFSET_BASIS;

    for (size_t i = 0; i < size; i++) {
        hash ^= ((const uint8_t *)ptr)[i];
        hash *= PRIME;
    }

    return hash;
}

static uint32_t fnv1_hash_string(const char *str) {
    return fnv1_hash_data(str, strlen(str));
}

static void st_fnv1a_quit(st_modctx_t *fnv1a_ctx) {
    st_fnv1a_simple_t *fnv1a = fnv1a_ctx->data;

    fnv1a->logger.info(fnv1a->logger.ctx,
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
