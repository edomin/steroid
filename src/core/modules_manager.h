#ifndef ST_SRC_CORE_MODULES_MANAGER_H
#define ST_SRC_CORE_MODULES_MANAGER_H

#include <list.h>

// find modules

//typedef void *(*st_getfunc_t)(char *module_name, char *func_name);

//typedef struct {
    //char        *name;
    //st_getfunc_t get_function;
//} st_modtype_t;

typedef struct {
    list_t *modules_data;
} st_modsmgr_t;

st_modsmgr_t *st_modsmgr_init(void);
void st_modsmgr_destroy(st_modsmgr_t *modsmgr);
void *st_modsmgr_get_function(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name);

#endif
