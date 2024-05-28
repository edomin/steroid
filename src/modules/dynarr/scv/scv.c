#include "scv.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <scv.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

static st_dynarr_funcs_t dynarr_funcs = {
    .destroy            = st_dynarr_destroy,
    .append             = st_dynarr_append,
    .set                = st_dynarr_set,
    .clear              = st_dynarr_clear,
    .sort               = st_dynarr_sort,
    .extract            = st_dynarr_extract,
    .get                = st_dynarr_get,
    .get_all            = st_dynarr_get_all,
    .get_elements_count = st_dynarr_get_elements_count,
    .is_empty           = st_dynarr_is_empty,
};

ST_MODULE_DEF_GET_FUNC(dynarr_scv)
ST_MODULE_DEF_INIT_FUNC(dynarr_scv)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_dynarr_scv_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_dynarr_import_functions(st_modctx_t *dynarr_ctx,
 st_modctx_t *logger_ctx) {
    st_dynarr_scv_t *module = dynarr_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "dynarr_scv: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("dynarr_scv", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("dynarr_scv", logger, info);

    return true;
}

static st_modctx_t *st_dynarr_init(st_modctx_t *logger_ctx) {
    st_modctx_t     *dynarr_ctx;
    st_dynarr_scv_t *module;

    dynarr_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_dynarr_scv_data, sizeof(st_dynarr_scv_t));

    if (!dynarr_ctx)
        return NULL;

    dynarr_ctx->funcs = &st_dynarr_scv_funcs;

    module = dynarr_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_dynarr_import_functions(dynarr_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, dynarr_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "dynarr_scv: Module initialized");

    return dynarr_ctx;
}

static void st_dynarr_quit(st_modctx_t *dynarr_ctx) {
    st_dynarr_scv_t *module = dynarr_ctx->data;

    module->logger.info(module->logger.ctx, "dynarr_scv: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, dynarr_ctx);
}

static st_dynarr_t *st_dynarr_create(st_modctx_t *dynarr_ctx,
 size_t data_size, size_t initial_capacity) {
    st_dynarr_scv_t *module = dynarr_ctx->data;
    st_dynarr_t     *dynarr = malloc(sizeof(st_dynarr_t));

    if (!dynarr) {
        module->logger.error(module->logger.ctx,
         "dynarr_scv: Unable to allocate memory for dynamic array object");

        return NULL;
    }

    dynarr->handle = scv_new(data_size, initial_capacity);
    if (!dynarr->handle) {
        module->logger.error(module->logger.ctx,
         "dynarr_scv: Unable to create new dynamic array");

        return NULL;
    }

    st_object_make(dynarr, dynarr_ctx, &dynarr_funcs);

    return dynarr;
}

static void st_dynarr_destroy(st_dynarr_t *dynarr) {
    scv_delete(dynarr->handle);
    free(dynarr);
}

static bool st_dynarr_append(st_dynarr_t *dynarr, const void *data) {
    return scv_push_back(dynarr->handle, data) == SCV_OK;
}

static bool st_dynarr_set(st_dynarr_t *dynarr, size_t index, const void *data) {
    return scv_replace(dynarr->handle, index, index + 1, data, 1) == SCV_OK;
}

static bool st_dynarr_clear(st_dynarr_t *dynarr) {
    return scv_clear(dynarr->handle) == SCV_OK;
}

static bool st_dynarr_sort(st_dynarr_t *dynarr,
 int (*cmpfunc)(const void *, const void *, void *), void *userptr) {
    qsort_r(scv_data(dynarr->handle), scv_size(dynarr->handle),
     scv_objsize(dynarr->handle), cmpfunc, userptr);

    return true;
}

static bool st_dynarr_extract(const st_dynarr_t *dynarr, void *dst,
 size_t index) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-qual"
        memcpy(dst, scv_at(dynarr->handle, index),
         scv_objsize(dynarr->handle));
    #pragma GCC diagnostic pop

    return true;
}

static const void *st_dynarr_get(st_dynarr_t *dynarr, size_t index) {
    return scv_at(dynarr->handle, index);
}

static const void *st_dynarr_get_all(st_dynarr_t *dynarr) {
    return scv_data(dynarr->handle);
}

static size_t st_dynarr_get_elements_count(const st_dynarr_t *dynarr) {
    return scv_size(dynarr->handle);
}

static bool st_dynarr_is_empty(const st_dynarr_t *dynarr) {
    return scv_empty(dynarr->handle);
}
