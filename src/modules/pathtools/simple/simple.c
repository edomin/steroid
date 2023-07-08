#include "simple.h"

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

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

void *st_module_pathtools_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_pathtools_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_pathtools_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"pathtools_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_pathtools_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_pathtools_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_pathtools_import_functions(st_modctx_t *pathtools_ctx,
 st_modctx_t *logger_ctx) {
    st_pathtools_simple_t *pathtools = pathtools_ctx->data;
    st_logger_funcs_t     *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;

    pathtools->logger.debug = logger_funcs->logger_debug;
    pathtools->logger.info  = logger_funcs->logger_info;
}

static st_modctx_t *st_pathtools_init(st_modctx_t *logger_ctx) {
    st_modctx_t           *pathtools_ctx;
    st_pathtools_simple_t *pathtools;

    pathtools_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_pathtools_simple_data, sizeof(st_pathtools_simple_t));

    if (!pathtools_ctx)
        return NULL;

    pathtools_ctx->funcs = &st_pathtools_simple_funcs;

    st_pathtools_import_functions(pathtools_ctx, logger_ctx);
    pathtools = pathtools_ctx->data;
    pathtools->logger.ctx = logger_ctx;

    pathtools->logger.info(pathtools->logger.ctx,
     "pathtools_simple: Path tools initialized.");

    return pathtools_ctx;
}

static void st_pathtools_quit(st_modctx_t *pathtools_ctx) {
    st_pathtools_simple_t *pathtools = pathtools_ctx->data;

    pathtools->logger.info(pathtools->logger.ctx,
     "pathtools_simple: Path tools destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, pathtools_ctx);
}

static bool st_pathtools_concat(
 __attribute__((unused)) st_modctx_t *pathtools_ctx, char *dst, size_t dstsize,
 const char *path, const char *append) {
    char joined_path[PATH_MAX];
    char resolved_path[PATH_MAX];

    return strcpy_s(joined_path, PATH_MAX, path) == 0 &&
     strcat_s(joined_path, PATH_MAX, "/") == 0 &&
     strcat_s(joined_path, PATH_MAX, append) == 0 &&
     realpath(joined_path, resolved_path) &&
     strncpy_s(dst, dstsize, resolved_path, PATH_MAX) == 0;
}