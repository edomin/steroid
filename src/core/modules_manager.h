#ifndef ST_SRC_CORE_MODULES_MANAGER_H
#define ST_SRC_CORE_MODULES_MANAGER_H

#include "steroids/module.h"

st_modsmgr_t *st_modsmgr_init(void);
void st_modsmgr_destroy(st_modsmgr_t *modsmgr);
void *st_modsmgr_get_function(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name);
void *st_modsmgr_get_function_from_ctx(const st_modsmgr_t *modsmgr,
 const st_modctx_t *ctx, const char *func_name);

#endif
