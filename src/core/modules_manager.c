#include "types.h"
#include "modules_manager.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#include "internal_modules.h"
#include "utils.h"

#define ST_MODSMGR_FUNCS                                           \
    &(st_modsmgr_funcs_t){                                         \
        .get_module_names      = st_modsmgr_get_module_names,      \
        .load_module           = st_modsmgr_load_module,           \
        .process_deps          = st_modsmgr_process_deps,          \
        .get_function          = st_modsmgr_get_function,          \
        .get_function_from_ctx = st_modsmgr_get_function_from_ctx, \
        .init_module_ctx       = st_modsmgr_init_module_ctx,       \
        .free_module_ctx       = st_free_module_ctx,               \
     }

st_modctx_t *st_modsmgr_init_module_ctx(st_modsmgr_t *modsmgr,
 const st_moddata_t *module_data, size_t data_size);
void st_free_module_ctx(st_modsmgr_t *modsmgr, st_modctx_t *modctx);

static st_moddata_t *st_modsmgr_find_module(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name) {
    st_slnode_t *node;

    if (!modsmgr || !subsystem)
        return NULL;

    node = st_slist_get_first(modsmgr);
    while (node) {
        st_moddata_t *module_data = st_slist_get_data(node);
        bool          subsystem_equal = st_utl_strings_equal(
         module_data->subsystem, subsystem);
        bool          name_equal = st_utl_strings_equal(module_data->name,
         module_name);
        bool          name_is_null = module_name == NULL;

        if (subsystem_equal && (name_equal || name_is_null))
            return module_data;

        node = st_slist_get_next(node);
    }

    return NULL;
}

static inline bool st_modsmgr_have_module(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name) {
    return !!st_modsmgr_find_module(modsmgr, subsystem, module_name);
}

static bool st_modsmgr_module_have_deps(const st_modsmgr_t *modsmgr,
 const st_moddata_t *module_data) {
    for (unsigned i = 0; i < module_data->prereqs_count; i++) {
        bool have_prereq = st_modsmgr_have_module(modsmgr,
         module_data->prereqs[i].subsystem, module_data->prereqs[i].name);

        if (have_prereq)
            continue;

        if (module_data->prereqs[i].name == NULL)
            fprintf(stderr, "steroids: Missing module of subsystem \"%s\" as "
             "prerequisite of module \"%s_%s\"\n",
             module_data->prereqs[i].subsystem,
             module_data->subsystem, module_data->name);
        else
            fprintf(stderr,
             "steroids: Missing module \"%s_%s\" as prerequisite of module "
             "\"%s_%s\"\n",
             module_data->prereqs[i].subsystem, module_data->prereqs[i].name,
             module_data->subsystem, module_data->name);

        return false;
    }

    return true;
}

static void st_modsmgr_process_deps(st_modsmgr_t *modsmgr) { // NOLINT(readability-function-cognitive-complexity)
    st_slnode_t *node = st_slist_get_first(modsmgr);

    while (node) {
        st_moddata_t *module_data = st_slist_get_data(node);

        if (!st_modsmgr_module_have_deps(modsmgr, module_data)) {
            st_slist_remove(node);
            st_modsmgr_process_deps(modsmgr);

            return;
        }

        node = st_slist_get_next(node);
    }
}

static void st_modsmgr_get_module_names(st_modsmgr_t *modsmgr, char **dst,
 size_t mods_count, size_t modname_size, const char *subsystem) {
    st_slnode_t *node;
    size_t       mod_index = 0;

    if (!modsmgr || !subsystem || !dst || !mods_count || !modname_size)
        return;

    node = st_slist_get_first(modsmgr);

    while (node) {
        st_moddata_t *module_data = st_slist_get_data(node);
        char         *modname;

        node = st_slist_get_next(node);

        if (!st_utl_strings_equal(module_data->subsystem, subsystem))
            continue;

        modname = dst[mod_index];
        if (!modname)
            break;

        if (strcpy_s(modname, modname_size, module_data->name) != 0)
            continue;

        if (++mod_index == mods_count)
            break;
    }
}

/*
 * Load noninternal module. Internal modules being loaded by function
 * st_modsmgr_init
 */
bool st_modsmgr_load_module(st_modsmgr_t *modsmgr,
 st_modinitfunc_t modinit_func, bool force) {
    st_moddata_t *module_data = modinit_func(modsmgr, ST_MODSMGR_FUNCS);

    printf("steroids: Trying to add module \"%s_%s\"\n", module_data->subsystem,
     module_data->name);

    if (!force && !st_modsmgr_module_have_deps(modsmgr, module_data))
        return false;

    return st_slist_insert_head(modsmgr, module_data);
}

st_modsmgr_t *st_modsmgr_init(void) {
    st_modsmgr_t *modsmgr = st_slist_create(sizeof(st_moddata_t));

    if (!modsmgr)
        return NULL;

    printf("steroids: Searching internal modules...\n");
    for (size_t i = 0; i < ST_INTERNAL_MODULES_COUNT; i++) {
        st_moddata_t *module_data =
         st_internal_modules_entrypoints.modules_init_funcs[i](modsmgr,
          ST_MODSMGR_FUNCS);

        if (!module_data)
            continue;

        printf("steroids: Found module \"%s_%s\"\n", module_data->subsystem,
         module_data->name);

        st_slist_insert_head(modsmgr, module_data);
    }

    st_modsmgr_process_deps(modsmgr);

    return modsmgr;
}

void st_modsmgr_destroy(st_modsmgr_t *modsmgr) {
    st_slist_destroy(modsmgr);
}

void *st_modsmgr_get_function(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name) {
    st_moddata_t *module_data = st_modsmgr_find_module(modsmgr, subsystem,
     module_name);

    if (!module_data)
        return NULL;

    return module_data->get_function(func_name);
}

void *st_modsmgr_get_function_from_ctx(const st_modsmgr_t *modsmgr,
 const st_modctx_t *ctx, const char *func_name) {
    st_moddata_t *module_data;

    if (!ctx)
        return NULL;

    module_data = st_modsmgr_find_module(modsmgr, ctx->subsystem, ctx->name);

    if (!module_data)
        return NULL;

    return module_data->get_function(func_name);
}

st_modctx_t *st_modsmgr_init_module_ctx(st_modsmgr_t *modsmgr,
 const st_moddata_t *module_data, size_t data_size) {
    st_modctx_t *modctx = malloc(sizeof(st_modctx_t));

    if (!modctx) {
        perror("malloc");

        return NULL;
    }

    modctx->subsystem = strdup(module_data->subsystem);
    if (!modctx->subsystem) {
        perror("strdup");

        goto strdup_sybsystem_fail;
    }

    modctx->name = strdup(module_data->name);
    if (!modctx->name) {
        perror("strdup");

        goto strdup_name_fail;
    }

    modctx->data = calloc(1, data_size);
    if (!modctx->data) {
        perror("malloc");

        goto malloc_fail;
    }

    return modctx;

malloc_fail:
    free(modctx->name);
strdup_name_fail:
    free(modctx->subsystem);
strdup_sybsystem_fail:
    free(modctx);

    return NULL;
}

void st_free_module_ctx(st_modsmgr_t *modsmgr, st_modctx_t *modctx) {
    st_slnode_t *node;

    if (!modsmgr || !modctx)
        return;

    node = st_slist_get_first(modsmgr);

    while (node) {
        if (st_slist_get_data(node) == modctx->data) {
            free(modctx->subsystem);
            free(modctx->name);
            free(modctx->data);
            free(modctx);
            st_slist_remove(node);

            break;
        }

        node = st_slist_get_next(node);
    }
}
