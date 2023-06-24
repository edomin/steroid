#include "fnv_hash.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <fnv_hash.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_fnv1_fnv_hash_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_fnv1_fnv_hash_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_fnv1_fnv_hash_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"fnv1_fnv_hash\": %s\n",
         err_msg_buf);

        return NULL;
    }

    return &st_module_fnv1_fnv_hash_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_fnv1_fnv_hash_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_fnv1_import_functions(st_modctx_t *fnv1_ctx,
 st_modctx_t *logger_ctx) {
    st_fnv1_fnv_hash_t *fnv1 = fnv1_ctx->data;
    st_logger_funcs_t  *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    fnv1->logger.debug = logger_funcs->logger_debug;
    fnv1->logger.info  = logger_funcs->logger_info;
    fnv1->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_fnv1_init(st_modctx_t *logger_ctx) {
    st_modctx_t        *fnv1_ctx;
    st_fnv1_fnv_hash_t *fnv1;

    fnv1_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_fnv1_fnv_hash_data, sizeof(st_fnv1_fnv_hash_t));

    if (fnv1_ctx == NULL)
        return NULL;

    fnv1_ctx->funcs = &st_fnv1_fnv_hash_funcs;

    st_fnv1_import_functions(fnv1_ctx, logger_ctx);
    fnv1 = fnv1_ctx->data;
    fnv1->logger.ctx = logger_ctx;

    fnv1->logger.info(fnv1->logger.ctx, "%s",
     "fnv1_fnv_hash: FNV-1 initialized.");

    return fnv1_ctx;
}

static void st_fnv1_quit(st_modctx_t *fnv1_ctx) {
    st_fnv1_fnv_hash_t *fnv1 = fnv1_ctx->data;

    fnv1->logger.info(fnv1->logger.ctx, "%s", "fnv1_simple: FNV-1 destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, fnv1_ctx);
}

static uint32_t st_fnv1_u32hash_string(
 __attribute__((unused)) st_modctx_t *so_ctx, const char *str) {
    return fnv1_hash_string(str);
}

static uint32_t st_fnv1_u32hash_bytes(
 __attribute__((unused)) st_modctx_t *so_ctx, const void *ptr, size_t size) {
    return fnv1_hash_data(ptr, size);
}

static st_u32hashstrfunc_t st_fnv1_get_u32hashstr_func(
 __attribute__((unused)) st_modctx_t *so_ctx) {
    return fnv1_hash_string;
}

static st_u32hashbytesfunc_t st_fnv1_get_u32hashbytes_func(
 __attribute__((unused)) st_modctx_t *so_ctx) {
    return fnv1_hash_data;
}