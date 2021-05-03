#include "modules_manager.h"

#include <stdbool.h>
#include <stdio.h>

#include "internal_modules.h"
#include "utils.h"

static st_moddata_t *st_modsmgr_find_module(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name) {
    list_node_t     *node;
    list_iterator_t *moddata_iter;

    if (!modsmgr || !subsystem)
        return NULL;

    moddata_iter = list_iterator_new(modsmgr->modules_data, LIST_HEAD);

    for (node = list_iterator_next(moddata_iter); node;
     node = list_iterator_next(moddata_iter)) {
        st_moddata_t *module_data = node->val;
        bool          subsystem_equal = st_utl_strings_equal(
         module_data->subsystem, subsystem);
        bool          name_equal = st_utl_strings_equal(module_data->name,
         module_name);
        bool          name_is_null = module_name == NULL;

        if (subsystem_equal && (name_equal || name_is_null)) {
            list_iterator_destroy(moddata_iter);

            return module_data;
        }
    }

    return NULL;
}

static inline bool st_modsmgr_have_module(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name) {
    return st_modsmgr_find_module(modsmgr, subsystem, module_name) != NULL;
}

static bool st_modsmgr_module_have_deps(const st_modsmgr_t *modsmgr,
 const st_moddata_t *module_data) {
    for (unsigned i = 0; i < module_data->prereqs_count; i++) {
        bool have_prereq = st_modsmgr_have_module(modsmgr,
         module_data->prereqs[i].subsystem, module_data->prereqs[i].name);

        if (have_prereq)
            continue;

        if (module_data->prereqs[i].name == NULL)
            printf("Missing module of subsystem \"%s\" as prerequisite of "
             "module \"%s_%s\"\n", module_data->prereqs[i].subsystem,
             module_data->subsystem, module_data->name);
        else
            printf(
             "Missing module \"%s_%s\" as prerequisite of module \"%s_%s\"\n",
             module_data->prereqs[i].subsystem, module_data->prereqs[i].name,
             module_data->subsystem, module_data->name);

        return false;
    }

    return true;
}

static void st_modsmgr_process_deps(st_modsmgr_t *modsmgr) {
    list_node_t     *node;
    list_iterator_t *moddata_iter = list_iterator_new(
     modsmgr->modules_data, LIST_HEAD);

    for (node = list_iterator_next(moddata_iter); node;
     node = list_iterator_next(moddata_iter)) {
        st_moddata_t *module_data = node->val;

        if (!st_modsmgr_module_have_deps(modsmgr, module_data)) {
            list_remove(modsmgr->modules_data, node);
            list_iterator_destroy(moddata_iter);
            st_modsmgr_process_deps(modsmgr);

            return;
        }
    }
}

st_modsmgr_t *st_modsmgr_init(void) {
    st_modsmgr_t *modsmgr = malloc(sizeof(st_modsmgr_t));

    if (!modsmgr)
        return NULL;

    modsmgr->modules_data = list_new();

    printf("Searching internal modules...\n");
    for (size_t i = 0; i < st_internal_modules_entrypoints.modules_count; i++) {
        st_moddata_t *module_data =
         st_internal_modules_entrypoints.modules_init_funcs[i](modsmgr,
         st_modsmgr_get_function);

        printf("Found module \"%s_%s\"\n", module_data->subsystem,
         module_data->name);
        list_rpush(modsmgr->modules_data, list_node_new(module_data));
    }

    st_modsmgr_process_deps(modsmgr);

    return modsmgr;
}

void st_modsmgr_destroy(st_modsmgr_t *modsmgr) {
    if (modsmgr == NULL)
        return;

    list_destroy(modsmgr->modules_data);
}


void *st_modsmgr_get_function(const void *modsmgr, const char *subsystem,
 const char *module_name, const char *func_name) {
    st_moddata_t *module_data = st_modsmgr_find_module(modsmgr, subsystem,
     module_name);

    if (module_data == NULL)
        return NULL;

    return module_data->get_function(func_name);
}
