#include "simple.h"

#include <dlfcn.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

typedef struct st_sohandle_s st_sohandle_t;

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

static void st_so_close(st_modctx_t *so_ctx, st_sohandle_t *handle);

void *st_module_so_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_so_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_so_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"so_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_so_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_so_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_so_import_functions(st_modctx_t *so_ctx,
 st_modctx_t *logger_ctx) {
    st_so_simple_t    *so = so_ctx->data;
    st_logger_funcs_t *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    so->logger.debug = logger_funcs->logger_debug;
    so->logger.info  = logger_funcs->logger_info;
    so->logger.error = logger_funcs->logger_error;
}

static st_modctx_t *st_so_init(st_modctx_t *logger_ctx) {
    st_modctx_t    *so_ctx;
    st_so_simple_t *so;

    so_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_so_simple_data, sizeof(st_so_simple_t));

    if (so_ctx == NULL)
        return NULL;

    so_ctx->funcs = &st_so_simple_funcs;

    st_so_import_functions(so_ctx, logger_ctx);
    so = so_ctx->data;
    so->logger.ctx = logger_ctx;

    SLIST_INIT(&so->opened_handles); // NOLINT(altera-unroll-loops)

    so->logger.info(so->logger.ctx, "%s", "so_simple: So initialized.");

    return so_ctx;
}

static void st_so_quit(st_modctx_t *so_ctx) {
    st_so_simple_t *so = so_ctx->data;

    while (!SLIST_EMPTY(&so->opened_handles)) {
        st_snode_t *node = SLIST_FIRST(&so->opened_handles);

        SLIST_REMOVE_HEAD(&so->opened_handles, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
        st_so_close(so_ctx, node->data);
        free(node);
    }

    so->logger.info(so->logger.ctx, "%s", "so_simple: So destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, so_ctx);
}

static st_sohandle_t *st_so_open(st_modctx_t *so_ctx, const char *filename) {
    st_so_simple_t *so = so_ctx->data;
    st_snode_t     *node;
    st_sohandle_t  *handle = dlopen(filename, RTLD_LAZY);

    if (handle) {
        so->logger.info(so->logger.ctx, "so_simple: So file \"%s\" opened",
         filename);
    } else {
        so->logger.error(so->logger.ctx,
         "so_simple: Unable to open so file \"%s\": %s", filename, dlerror());

        return NULL;
    }

    node = malloc(sizeof(st_snode_t));
    if (!node) {
        so->logger.error(so->logger.ctx,"Error occured while allocating memory"
         "for so handle entry: \"%s\": %s. Module skipped.\n", filename,
         strerror(errno));
        dlclose(handle);

        return NULL;
    }
    node->data = handle;
    SLIST_INSERT_HEAD(&so->opened_handles, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)

    return handle;
}

static st_sohandle_t *st_so_memopen(st_modctx_t *so_ctx,
 __attribute__((unused)) const void *data, __attribute__((unused))size_t size) {
    st_so_simple_t *so = so_ctx->data;

    so->logger.error(so->logger.ctx, "%s",
     "so_simple: Unable to open so from memory. Not implemented yet");

    return NULL;
}

static void st_so_close(st_modctx_t *so_ctx, st_sohandle_t *handle) {
    st_so_simple_t *so_simple = so_ctx->data;

    if (dlclose(handle) != 0) {
        so_simple->logger.error(so_simple->logger.ctx,
         "so_simple: Unable to close so file. %s", dlerror());
    }

    so_simple->logger.info(so_simple->logger.ctx, "%s",
     "so_simple: So file closed");
}

static void *st_so_load_symbol(st_modctx_t *so_ctx, st_sohandle_t *handle,
 const char *name) {
    st_so_simple_t *so_simple = so_ctx->data;
    void *symbol = dlsym(handle, name);

    if (symbol)
        so_simple->logger.info(so_simple->logger.ctx,
         "so_simple: Symbol loaded \"%s\"", name);
    else
        so_simple->logger.error(so_simple->logger.ctx,
         "so_simple: Unable to load symbol \"%s\"", name);

    return symbol;
}

static bool st_so_load_module(st_modctx_t *so_ctx, const char *filename) {
    st_modinitfunc_t modinit_func;
    st_sohandle_t   *handle = st_so_open(so_ctx, filename);

    if (!handle)
        return false;

    modinit_func = st_so_load_symbol(so_ctx, handle, "st_module_init");
    if (!modinit_func)
        return false;

    return global_modsmgr_funcs.load_module(global_modsmgr, modinit_func);
}

static bool st_so_memload_module(st_modctx_t *so_ctx, const void *data,
 size_t size) {
    st_modinitfunc_t modinit_func;
    st_sohandle_t   *handle = st_so_memopen(so_ctx, data, size);

    if (!handle)
        return false;

    modinit_func = st_so_load_symbol(so_ctx, handle, "st_module_init");
    if (!modinit_func)
        return false;

    return global_modsmgr_funcs.load_module(global_modsmgr, modinit_func);
}