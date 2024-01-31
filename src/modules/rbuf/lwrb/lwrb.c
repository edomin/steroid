#include "lwrb.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <lwrb.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(rbuf_lwrb)
ST_MODULE_DEF_INIT_FUNC(rbuf_lwrb)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_rbuf_lwrb_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_rbuf_import_functions(st_modctx_t *rbuf_ctx,
 st_modctx_t *logger_ctx) {
    st_rbuf_lwrb_t *module = rbuf_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "rbuf_lwrb: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("rbuf_lwrb", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("rbuf_lwrb", logger, info);

    return true;
}

static st_modctx_t *st_rbuf_init(st_modctx_t *logger_ctx) {
    st_modctx_t    *rbuf_ctx;
    st_rbuf_lwrb_t *module;

    rbuf_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_rbuf_lwrb_data, sizeof(st_rbuf_lwrb_t));

    if (!rbuf_ctx)
        return NULL;

    rbuf_ctx->funcs = &st_rbuf_lwrb_funcs;

    module = rbuf_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_rbuf_import_functions(rbuf_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, rbuf_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "rbuf_lwrb: Module initialized");

    return rbuf_ctx;
}

static void st_rbuf_quit(st_modctx_t *rbuf_ctx) {
    st_rbuf_lwrb_t *module = rbuf_ctx->data;

    module->logger.info(module->logger.ctx, "rbuf_lwrb: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, rbuf_ctx);
}

static st_rbuf_t *st_rbuf_create(st_modctx_t *rbuf_ctx, size_t size) {
    st_rbuf_lwrb_t *module = rbuf_ctx->data;
    st_rbuf_t      *rbuf = malloc(sizeof(st_rbuf_t));

    if (!rbuf) {
        module->logger.error(module->logger.ctx,
         "rbuf_lwrb: Unable to allocate memory for ring buffer structure: %s",
         strerror(errno));

        return NULL;
    }

    rbuf->data = malloc(size);
    if (!rbuf->data) {
        module->logger.error(module->logger.ctx,
         "rbuf_lwrb: Unable to allocate memory for ring buffer data: %s",
         strerror(errno));

        goto malloc_data_fail;
    }

    if (!lwrb_init(&rbuf->handle, rbuf->data, size)) {
        module->logger.error(module->logger.ctx,
         "rbuf_lwrb: Unable to init ring buffer");

        goto init_fail;
    }

    rbuf->ctx = rbuf_ctx;

    return rbuf;

init_fail:
    free(rbuf->data);
malloc_data_fail:
    free(rbuf);

    return NULL;
}

static void st_rbuf_destroy(st_rbuf_t *rbuf) {
    lwrb_free(&rbuf->handle);
}

static bool st_rbuf_push(st_rbuf_t *rbuf, const void *data, size_t size) {
    if (lwrb_get_free(&rbuf->handle) < size)
        return false;

    return lwrb_write(&rbuf->handle, data, size) == size;
}

static bool st_rbuf_peek(const st_rbuf_t *rbuf, void *data, size_t size) {
    return lwrb_peek(&rbuf->handle, 0, data, size) == size;
}

static bool st_rbuf_pop(st_rbuf_t *rbuf, void *data, size_t size) {
    return lwrb_read(&rbuf->handle, data, size) == size;
}

static bool st_rbuf_drop(st_rbuf_t *rbuf, size_t size) {
    return lwrb_skip(&rbuf->handle, size) == size;
}

static bool st_rbuf_clear(st_rbuf_t *rbuf) {
    while (lwrb_get_full(&rbuf->handle)) {
        if (lwrb_skip(&rbuf->handle, 1) != 1)
            return false;
    }

    return true;
}

static size_t st_rbuf_get_free_space(const st_rbuf_t *rbuf) {
    return lwrb_get_free(&rbuf->handle);
}

static bool st_rbuf_is_empty(const st_rbuf_t *rbuf) {
    return lwrb_get_full(&rbuf->handle) == 0;
}
