#ifndef ST_INCLUDE_STEROIDS_MODULE_H
#define ST_INCLUDE_STEROIDS_MODULE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "steroids/types/list.h"

#define ST_MT_internal 0
#define ST_MT_shared   1

#define ST_LOAD_FUNCTION(this_mod, mod, function)                        \
    module->mod.function = global_modsmgr_funcs.get_function_from_ctx(   \
     global_modsmgr, mod##_ctx, #function);                              \
    if (!module->mod.function) {                                         \
        module->logger.error(module->logger.ctx,                         \
         this_mod ": Unable to load function \"%s\" from module \"%s\"", \
         #function, #mod);                                               \
        return false;                                                    \
    }

#define ST_LOAD_GLOBAL_FUNCTION(this_mod, mod, function)                      \
    st_##mod##_##function = global_modsmgr_funcs.get_function(global_modsmgr, \
     #mod, NULL, #function);                                                  \
    if (!st_##mod##_##function) {                                             \
        module->logger.error(module->logger.ctx,                              \
         this_mod ": Unable to load function \"%s\" from module \"%s\"",      \
         #function, #mod);                                                    \
        return false;                                                         \
    }

typedef struct {
    char      *subsystem;
    char      *name;
    void      *data;
    void      *funcs;
    bool       alive;
    uint32_t   id;
    st_slist_t uses;
} st_modctx_t;

typedef struct {
    st_modctx_t *ctx;
    uint32_t     id;
} st_modctxuses_t;

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

struct st_modsmgr_funcs_s;

#ifndef ST_MODSMGR_T_DEFINED
    typedef struct st_modsmgr_s st_modsmgr_t;
#endif

typedef st_moddata_t *(*st_modinitfunc_t)(st_modsmgr_t *modsmgr,
 struct st_modsmgr_funcs_s *modsmgr_funcs);

typedef bool (*st_modsmgr_load_module_t)(st_modsmgr_t *modsmgr,
 st_modinitfunc_t modinit_func);
typedef void (*st_modsmgr_get_module_names_t)(st_modsmgr_t *modsmgr, char **dst,
 size_t mods_count, size_t modname_size, const char *subsystem);
typedef void *(*st_modsmgr_get_function_t)(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name);
typedef void *(*st_modsmgr_get_function_from_ctx_t)(const st_modsmgr_t *modsmgr,
 const st_modctx_t *ctx, const char *func_name);
typedef st_modctx_t *(*st_modsmgr_init_module_ctx_t)(st_modsmgr_t *modsmgr,
 const st_moddata_t *module_data, size_t data_size);
typedef void (*st_modsmgr_free_module_ctx_t)(st_modsmgr_t *modsmgr,
 st_modctx_t *modctx);

typedef struct st_modsmgr_funcs_s {
    st_modsmgr_get_module_names_t      get_module_names;
    st_modsmgr_load_module_t           load_module;
    st_modsmgr_get_function_t          get_function;
    st_modsmgr_get_function_from_ctx_t get_function_from_ctx;
    st_modsmgr_init_module_ctx_t       init_module_ctx;
    st_modsmgr_free_module_ctx_t       free_module_ctx;
} st_modsmgr_funcs_t;

typedef struct {
    const char *func_name;
    void       *func_pointer;
} st_modfuncentry_t;

typedef struct {
    size_t            funcs_count;
    st_modfuncentry_t entries[];
} st_modfuncstbl_t;

#endif
