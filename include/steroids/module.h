#ifndef ST_INCLUDE_STEROIDS_MODULE_H
#define ST_INCLUDE_STEROIDS_MODULE_H

#include <stdlib.h>
#include "steroids/string.h"

#define ST_MODULE_NAME_LEN_MAX      32
#define ST_MODULE_SUBSYSTEM_LEN_MAX 32
#define ST_MODULE_TYPE_LEN_MAX      32
#define ST_MODULE_PREREQS_MAX       32
#define ST_MODULE_FUNC_NAME_LEN_MAX 128

typedef struct {
    char   subsystem[ST_MODULE_SUBSYSTEM_LEN_MAX];
    size_t subsystem_len;
    char   name[ST_MODULE_NAME_LEN_MAX];
    size_t name_len;
    void  *data;
} st_modctx_t;

typedef struct {
    char   subsystem[ST_MODULE_SUBSYSTEM_LEN_MAX];
    size_t subsystem_len;
    char   name[ST_MODULE_NAME_LEN_MAX];
    size_t name_len;
} st_modprerq_t;

typedef void *(*st_getfunc_t)(const char *funcname);

typedef struct {
    char          name[ST_MODULE_NAME_LEN_MAX];
    size_t        name_len;
    char          type[ST_MODULE_TYPE_LEN_MAX];
    size_t        type_len;
    char          subsystem[ST_MODULE_SUBSYSTEM_LEN_MAX];
    size_t        subsystem_len;
    st_modprerq_t prereqs[ST_MODULE_PREREQS_MAX];
    size_t        prereqs_count;
    st_getfunc_t  get_function;
} st_moddata_t;

typedef st_moddata_t *(*st_modinitfunc_t)(void);

typedef struct {
    char  func_name[ST_MODULE_FUNC_NAME_LEN_MAX];
    void *func_pointer;
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

    strlcpy(module_ctx->subsystem, module_data->subsystem,
     module_data->subsystem_len);
    module_ctx->subsystem_len = module_data->subsystem_len;
    strlcpy(module_ctx->name, module_data->name, module_data->name_len);
    module_ctx->name_len = module_data->name_len;

    module_ctx->data = malloc(data_size);
    if (module_ctx->data == NULL) {
        free(module_ctx);
        return NULL;
    }

    return module_ctx;
}

static inline void st_free_module_ctx(st_modctx_t *modctx) {
    if (modctx) {
        free(modctx->data);
        free(modctx);
    }
}

#endif
