#include "cfgpath.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include <cfgpath.h>
#pragma GCC diagnostic pop

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

void *st_module_spcpaths_cfgpath_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_spcpaths_cfgpath_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_spcpaths_cfgpath_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"spcpaths_cfgpath\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_spcpaths_cfgpath_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_spcpaths_cfgpath_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_spcpaths_import_functions(st_modctx_t *spcpaths_ctx,
 st_modctx_t *logger_ctx) {
    st_spcpaths_cfgpath_t *module = spcpaths_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "spcpaths_cfgpath: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION(logger, debug);
    ST_LOAD_FUNCTION(logger, info);

    return true;
}

static st_modctx_t *st_spcpaths_init(st_modctx_t *logger_ctx) {
    st_modctx_t           *spcpaths_ctx;
    st_spcpaths_cfgpath_t *spcpaths;

    spcpaths_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_spcpaths_cfgpath_data, sizeof(st_spcpaths_cfgpath_t));

    if (!spcpaths_ctx)
        return NULL;

    spcpaths_ctx->funcs = &st_spcpaths_cfgpath_funcs;

    spcpaths = spcpaths_ctx->data;
    spcpaths->logger.ctx = logger_ctx;

    if (!st_spcpaths_import_functions(spcpaths_ctx, logger_ctx))
        return NULL;

    spcpaths->logger.info(spcpaths->logger.ctx,
     "spcpaths_cfgpath: Special paths mgr initialized.");

    return spcpaths_ctx;
}

static void st_spcpaths_quit(st_modctx_t *spcpaths_ctx) {
    st_spcpaths_cfgpath_t *spcpaths = spcpaths_ctx->data;

    spcpaths->logger.info(spcpaths->logger.ctx,
     "spcpaths_cfgpath: Special paths mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, spcpaths_ctx);
}

static void st_spcpaths_get_config_path(
 __attribute__((unused)) st_modctx_t *spcpaths_ctx, char *dst, size_t dstlen,
 const char *appname) {
    get_user_config_folder(dst, (unsigned)dstlen, appname);
}

static void st_spcpaths_get_data_path(
 __attribute__((unused)) st_modctx_t *spcpaths_ctx, char *dst, size_t dstlen,
 const char *appname) {
    get_user_data_folder(dst, (unsigned)dstlen, appname);
}

static void st_spcpaths_get_cache_path(
 __attribute__((unused)) st_modctx_t *spcpaths_ctx, char *dst, size_t dstlen,
 const char *appname) {
    get_user_cache_folder(dst, (unsigned)dstlen, appname);
}
