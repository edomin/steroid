#include "types.h"
#include "modules_manager.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop

#include <xmempool.h>

#include "genid.h"
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
    st_snode_t *node;

    SLIST_FOREACH(node, &modsmgr->modules_data, ST_SNODE_NEXT) {
        st_moddata_t *module_data = node->data;

        if (!st_modsmgr_module_have_deps(modsmgr, module_data)) {
            SLIST_REMOVE(&modsmgr->modules_data, node, st_snode_t, // NOLINT(altera-unroll-loops)
             ST_SNODE_NEXT);
            st_modsmgr_process_deps(modsmgr);

            return;
        }
    }
}

static void st_modsmgr_get_module_names(st_modsmgr_t *modsmgr, char **dst,
 size_t mods_count, size_t modname_size, const char *subsystem) {
    st_snode_t *node;
    size_t      mod_index = 0;

    if (!modsmgr || !subsystem || !dst || !mods_count || !modname_size)
        return;

    SLIST_FOREACH(node, &modsmgr->modules_data, ST_SNODE_NEXT) {
        st_moddata_t *module_data = node->data;
        char         *modname;

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
    st_snode_t   *node;
    st_moddata_t *module_data = modinit_func(modsmgr, ST_MODSMGR_FUNCS);

    printf("steroids: Trying to add module \"%s_%s\"\n", module_data->subsystem,
     module_data->name);

    if (!force && !st_modsmgr_module_have_deps(modsmgr, module_data))
        return false;

    node = malloc(sizeof(st_snode_t));
    if (!node) {
        fprintf(stderr, "steroids: Error occured while processing found module "
         "\"%s_%s\": %s. Module skipped.\n", module_data->subsystem,
         module_data->name,
         strerror(errno));
        return false;
    }
    node->data = module_data;
    SLIST_INSERT_HEAD(&modsmgr->modules_data, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)

    return true;
}

st_modsmgr_t *st_modsmgr_init(void) {
    st_modsmgr_t *modsmgr = malloc(sizeof(st_modsmgr_t));

    if (!modsmgr)
        return NULL;

    SLIST_INIT(&modsmgr->modules_data); // NOLINT(altera-unroll-loops)

    printf("steroids: Searching internal modules...\n");
    for (size_t i = 0; i < ST_INTERNAL_MODULES_COUNT; i++) {
        st_moddata_t *module_data =
         st_internal_modules_entrypoints.modules_init_funcs[i](modsmgr,
          ST_MODSMGR_FUNCS);
        st_snode_t *node;

        if (!module_data)
            continue;

        printf("steroids: Found module \"%s_%s\"\n", module_data->subsystem,
         module_data->name);

        node = malloc(sizeof(st_snode_t));
        if (!node) {
            fprintf(stderr, "steroids: Error occured while processing found "
             "module: \"%s_%s\": %s. Module skipped.\n", module_data->subsystem,
             module_data->name, strerror(errno));
            continue;
        }
        node->data = module_data;
        SLIST_INSERT_HEAD(&modsmgr->modules_data, node, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
    }

    st_modsmgr_process_deps(modsmgr);

    modsmgr->ctx_pool = xmem_create_pool(sizeof(st_modctx_t));

    return modsmgr;
}

void st_modsmgr_destroy(st_modsmgr_t *modsmgr) {
    if (modsmgr == NULL)
        return;

    while (!SLIST_EMPTY(&modsmgr->modules_data)) {
        st_snode_t *node = SLIST_FIRST(&modsmgr->modules_data);

        SLIST_REMOVE_HEAD(&modsmgr->modules_data, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
        free(node);
    }

    xmem_destroy_pool(modsmgr->ctx_pool);
}

void *st_modsmgr_get_function(const st_modsmgr_t *modsmgr,
 const char *subsystem, const char *module_name, const char *func_name) {
    st_moddata_t *module_data = st_modsmgr_find_module(modsmgr, subsystem,
     module_name);

    if (module_data == NULL)
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
    st_modctx_t *modctx = (st_modctx_t *)xmem_alloc(modsmgr->ctx_pool);

    if (modctx == NULL)
        return NULL;

    modctx->subsystem = strdup(module_data->subsystem);
    if (modctx->subsystem == NULL) {
        perror("strdup");

        goto error_free_ctx_pool;
    }

    modctx->name = strdup(module_data->name);
    if (modctx->name == NULL) {
        perror("strdup");

        goto error_free_subsystem;
    }

    modctx->data = malloc(data_size);
    if (modctx->data == NULL) {
        perror("malloc");

        goto error_free_name;
    }

    modctx->alive = true;
    modctx->id = st_genid();

    return modctx;

error_free_name:
    free(modctx->name);
error_free_subsystem:
    free(modctx->subsystem);
error_free_ctx_pool:
    xmem_free(modsmgr->ctx_pool, (char *)modctx);

    return NULL;
}

void st_free_module_ctx(st_modsmgr_t *modsmgr, st_modctx_t *modctx) {
    if (modsmgr && modctx) {
        while (!SLIST_EMPTY(&modctx->uses)) {
            SLIST_REMOVE_HEAD(&modctx->uses, ST_SNODE_NEXT); // NOLINT(altera-unroll-loops)
            free(node);
        }
        free(modctx->subsystem);
        free(modctx->name);
        free(modctx->data);
        modctx->alive = false;
        xmem_free(modsmgr->ctx_pool, (char *)modctx);
    }
}
