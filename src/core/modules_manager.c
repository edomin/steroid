#include "modules_manager.h"

#include <stdbool.h>
#include <stdio.h>

#include "internal_modules.h"
#include "utils.h"

static st_moddata_t *st_modsmgr_find_module(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name) {
    st_snode_t *node;

    if (!modsmgr || !subsystem)
        return NULL;

    SLIST_FOREACH(node, &modsmgr->modules_data, ST_SNODE_NEXT) {
        st_moddata_t *module_data = node->data;
        bool          subsystem_equal = st_utl_strings_equal(
         module_data->subsystem, subsystem);
        bool          name_equal = st_utl_strings_equal(module_data->name,
         module_name);
        bool          name_is_null = module_name == NULL;

        if (subsystem_equal && (name_equal || name_is_null))
            return module_data;
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
    st_snode_t *node;

    SLIST_FOREACH(node, &modsmgr->modules_data, ST_SNODE_NEXT) {
        st_moddata_t *module_data = node->data;

        if (!st_modsmgr_module_have_deps(modsmgr, module_data)) {
            SLIST_REMOVE(&modsmgr->modules_data, node, st_snode_t,
             ST_SNODE_NEXT);
            st_modsmgr_process_deps(modsmgr);

            return;
        }
    }
}

st_modsmgr_t *st_modsmgr_init(void) {
    st_modsmgr_t *modsmgr = malloc(sizeof(st_modsmgr_t));

    if (!modsmgr)
        return NULL;

    SLIST_INIT(&modsmgr->modules_data);

    printf("Searching internal modules...\n");
    for (size_t i = 0; i < st_internal_modules_entrypoints.modules_count; i++) {
        st_moddata_t *module_data =
         st_internal_modules_entrypoints.modules_init_funcs[i](modsmgr,
         st_modsmgr_get_function);
        st_snode_t *node;

        printf("Found module \"%s_%s\"\n", module_data->subsystem,
         module_data->name);

        node = malloc(sizeof(st_snode_t));
        if (!node) {
            perror("malloc");
            printf("Error occured while processing found module: \"%s_%s\". "
             "Module skipped.\n", module_data->subsystem, module_data->name);
        }
        node->data = module_data;
        SLIST_INSERT_HEAD(&modsmgr->modules_data, node, ST_SNODE_NEXT);
    }

    st_modsmgr_process_deps(modsmgr);

    return modsmgr;
}

void st_modsmgr_destroy(st_modsmgr_t *modsmgr) {
    if (modsmgr == NULL)
        return;

    while (!SLIST_EMPTY(&modsmgr->modules_data)) {
       st_snode_t *node = SLIST_FIRST(&modsmgr->modules_data);
       SLIST_REMOVE_HEAD(&modsmgr->modules_data, ST_SNODE_NEXT);
       free(node);
   }
}


void *st_modsmgr_get_function(const void *modsmgr, const char *subsystem,
 const char *module_name, const char *func_name) {
    st_moddata_t *module_data = st_modsmgr_find_module(modsmgr, subsystem,
     module_name);

    if (module_data == NULL)
        return NULL;

    return module_data->get_function(func_name);
}
