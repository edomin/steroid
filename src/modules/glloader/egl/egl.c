#include "egl.h"

#include <EGL/egl.h>

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

ST_MODULE_DEF_GET_FUNC(glloader_egl)
ST_MODULE_DEF_INIT_FUNC(glloader_egl)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_glloader_egl_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_glloader_import_functions(st_modctx_t *glloader_ctx,
 st_modctx_t *logger_ctx) {
    st_glloader_egl_t *module = glloader_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "glloader_egl: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("glloader_egl", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("glloader_egl", logger, info);

    return true;
}

static st_modctx_t *st_glloader_init(st_modctx_t *logger_ctx) {
    st_modctx_t       *glloader_ctx;
    st_glloader_egl_t *module;

    glloader_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_glloader_egl_data, sizeof(st_glloader_egl_t));

    if (!glloader_ctx)
        return NULL;

    glloader_ctx->funcs = &st_glloader_egl_funcs;

    module = glloader_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_glloader_import_functions(glloader_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, glloader_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "glloader_egl: Open GL functions loader initialized");

    return glloader_ctx;
}

static void st_glloader_quit(st_modctx_t *glloader_ctx) {
    st_glloader_egl_t *module = glloader_ctx->data;

    module->logger.info(module->logger.ctx,
     "glloader_egl: Open GL functions loader destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, glloader_ctx);
}

static void *st_glloader_get_proc_address(st_modctx_t *glloader_ctx,
 const char *funcname) {
    return eglGetProcAddress(funcname);
}