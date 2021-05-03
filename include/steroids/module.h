#ifndef ST_INCLUDE_STEROIDS_MODULE_H
#define ST_INCLUDE_STEROIDS_MODULE_H

#include <stdlib.h>
#include "steroids/string.h"

typedef struct {
    char *subsystem;
    char *name;
    void *data;
    void *funcs;
} st_modctx_t;

typedef struct {
    const char *subsystem;
    const char *name;
} st_modprerq_t;

typedef void *(*st_getfunc_t)(const char *funcname);

typedef struct {
    const char    *name;
    const char    *type;
    const char    *subsystem;
    st_modprerq_t *prereqs;
    size_t         prereqs_count;
    st_getfunc_t   get_function;
} st_moddata_t;

typedef void *(*st_modsmgr_get_function_t)(const void *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name);
typedef st_moddata_t *(*st_modinitfunc_t)(void *modsmgr,
 st_modsmgr_get_function_t modsmgr_get_function);

typedef struct {
    const char *func_name;
    void       *func_pointer;
} st_modfuncentry_t;

typedef struct {
    size_t            funcs_count;
    st_modfuncentry_t entries[];
} st_modfuncstbl_t;

static inline st_modctx_t *st_init_module_ctx(st_moddata_t *module_data,
 size_t data_size) {
    st_modctx_t *module_ctx = malloc(sizeof(st_modctx_t));

    if (module_ctx == NULL)
        return NULL;

    module_ctx->subsystem = strdup(module_data->subsystem);
    module_ctx->name = strdup(module_data->name);

    module_ctx->data = malloc(data_size);
    if (module_ctx->data == NULL) {
        free(module_ctx);

        return NULL;
    }

    return module_ctx;
}

static inline void st_module_ctx_add_funcs(st_modctx_t *modctx, void *funcs) {
    modctx->funcs = funcs;
}

static inline void st_free_module_ctx(st_modctx_t *modctx) {
    if (modctx) {
        free(modctx->data);
        free(modctx);
    }
}

#endif
