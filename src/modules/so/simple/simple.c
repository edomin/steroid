#include "simple.h"

#include <dlfcn.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

ST_MODULE_DEF_GET_FUNC(so_simple)
ST_MODULE_DEF_INIT_FUNC(so_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_so_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_so_import_functions(st_modctx_t *so_ctx,
 st_modctx_t *logger_ctx) {
    st_so_simple_t *module = so_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "so_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("so_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("so_simple", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("so_simple", logger, warning);

    return true;
}

static st_modctx_t *st_so_init(st_modctx_t *logger_ctx) {
    st_modctx_t    *so_ctx;
    st_so_simple_t *so;

    so_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_so_simple_data, sizeof(st_so_simple_t));

    if (so_ctx == NULL)
        return NULL;

    so_ctx->funcs = &st_so_simple_funcs;

    so = so_ctx->data;
    so->logger.ctx = logger_ctx;

    if (!st_so_import_functions(so_ctx, logger_ctx))
        goto fail;

    so->opened_handles = st_slist_create(sizeof(st_so_t));
    if (!so->opened_handles) {
        so->logger.error(so->logger.ctx,
         "so_simple: Unable to create list of so file entries");

        goto fail;
    }

    so->logger.info(so->logger.ctx, "%s", "so_simple: So initialized");

    return so_ctx;

fail:
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, so_ctx);

    return NULL;
}

static void st_so_quit(st_modctx_t *so_ctx) {
    st_so_simple_t *module = so_ctx->data;

    while (!st_slist_empty(module->opened_handles)) {
        st_slnode_t *node = st_slist_get_first(module->opened_handles);
        st_so_t     *so = st_slist_get_data(node);

        st_slist_remove_head(module->opened_handles);
        if (dlclose(so->handle) != 0)
            module->logger.warning(module->logger.ctx,
             "so_simple: Unable to close so: %s", dlerror());
        free(so);
    }

    st_slist_destroy(module->opened_handles);

    module->logger.info(module->logger.ctx, "so_simple: So mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, so_ctx);
}

static st_so_t *st_so_open(st_modctx_t *so_ctx, const char *filename) {
    st_so_simple_t *module = so_ctx->data;
    void           *handle = dlopen(filename, RTLD_LAZY);
    st_so_t        *so;

    if (handle) {
        module->logger.info(module->logger.ctx,
         "so_simple: So file \"%s\" opened", filename);
    } else {
        module->logger.error(module->logger.ctx,
         "so_simple: Unable to open so file \"%s\": %s", filename, dlerror());

        return NULL;
    }

    so = malloc(sizeof(st_so_t));
    if (!so) {
        module->logger.error(module->logger.ctx,
         "so_simple: Unable to allocate memory for so entry while opening "
         "file \"%s\": %s", filename, strerror(errno));
        dlclose(handle);

        return NULL;
    }
    so->module = module;
    so->handle = handle;

    if (!st_slist_insert_head(module->opened_handles, so)) {
        module->logger.error(module->logger.ctx,
         "so_simple: Unable to create node for so file entry: \"%s\"",
         filename);
        free(so);
        dlclose(handle);

        return NULL;
    }

    return so;
}

static st_so_t *st_so_memopen(st_modctx_t *so_ctx,
 __attribute__((unused)) const void *data, __attribute__((unused))size_t size) {
    st_so_simple_t *so = so_ctx->data;

    so->logger.error(so->logger.ctx, "%s",
     "so_simple: Unable to open so from memory. Not implemented yet");

    return NULL;
}

static void st_so_close(st_so_t *so) {
    st_so_simple_t *module = so->module;

    while (!st_slist_empty(module->opened_handles)) {
        st_slnode_t *node = st_slist_get_first(module->opened_handles);

        if (st_slist_get_data(node) == so) {
            st_slist_remove_head(module->opened_handles);
            if (dlclose(so->handle) != 0)
                module->logger.warning(module->logger.ctx,
                 "so_simple: Unable to close so file. %s", dlerror());
            free(so);

            break;
        }
    }

    module->logger.info(module->logger.ctx, "%s", "so_simple: So file closed");
}

static void *st_so_load_symbol(st_so_t *so, const char *name) {
    st_so_simple_t *module = so->module;
    void           *symbol = dlsym(so->handle, name);

    if (symbol)
        module->logger.info(module->logger.ctx,
         "so_simple: Symbol loaded \"%s\"", name);
    else
        module->logger.error(module->logger.ctx,
         "so_simple: Unable to load symbol \"%s\"", name);

    return symbol;
}
