#include "simple.h"

#include <dlfcn.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_str_lib.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

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
    global_modsmgr = modsmgr;
    if (memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t)) != 0) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, errno);
        fprintf(stderr, "Unable to init module \"so_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

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

    so->logger.info(so->logger.ctx, "%s", "so_simple: So initialized.");

    return so_ctx;
}

static void st_so_quit(st_modctx_t *so_ctx) {
    st_so_simple_t *so = so_ctx->data;

    so->logger.info(so->logger.ctx, "%s", "so_simple: So destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, so_ctx);
}

static st_so *st_so_open(st_modctx_t *so_ctx, const char *filename) {
    st_so_simple_t *so = so_ctx->data;
    st_so *opened_so;

    opened_so = dlopen(filename, RTLD_LAZY);

    if (!opened_so)
        so->logger.info(so->logger.ctx, "so_simple: So file \"%s\" opened",
         filename);
    else
        so->logger.error(so->logger.ctx,
         "so_simple: Unable to open so file \"%s\": %s", filename, dlerror());

    return opened_so;
}

static st_so *st_so_memopen(st_modctx_t *so_ctx,
 __attribute__((unused)) const void *data, __attribute__((unused))size_t size) {
    st_so_simple_t *so = so_ctx->data;

    so->logger.error(so->logger.ctx, "%s",
     "so_simple: Unable to open so from memory. Not implemented yet");

    return NULL;
}

static void st_so_close(st_modctx_t *so_ctx, st_so *so) {
    st_so_simple_t *so_simple = so_ctx->data;

    if (dlclose(so) != 0) {
        so_simple->logger.error(so_simple->logger.ctx,
         "so_simple: Unable to close so file. %s", dlerror());
    }

    so_simple->logger.info(so_simple->logger.ctx, "%s",
     "so_simple: So file closed");
}

static void *st_so_load_symbol(st_modctx_t *so_ctx, st_so *so,
 const char *name) {
    st_so_simple_t *so_simple = so_ctx->data;
    void *symbol = dlsym(so, name);

    if (symbol)
        so_simple->logger.info(so_simple->logger.ctx,
         "so_simple: Symbol loaded \"%s\"", name);
    else
        so_simple->logger.error(so_simple->logger.ctx,
         "so_simple: Unable to load symbol \"%s\"", name);

    return symbol;
}