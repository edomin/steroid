#include "simple.h"

#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_plugin_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_plugin_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_plugin_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"plugin_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_plugin_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_plugin_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_plugin_import_functions(st_modctx_t *plugin_ctx,
 st_modctx_t *fs_ctx, st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx,
 st_modctx_t *so_ctx, st_modctx_t *spcpaths_ctx, st_modctx_t *zip_ctx) {
    st_plugin_simple_t *module = plugin_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "plugin_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION("plugin_simple", fs, mkdir);

    ST_LOAD_FUNCTION("plugin_simple", logger, debug);
    ST_LOAD_FUNCTION("plugin_simple", logger, info);

    ST_LOAD_FUNCTION("plugin_simple", pathtools, get_parent_dir);
    ST_LOAD_FUNCTION("plugin_simple", pathtools, concat);

    ST_LOAD_FUNCTION("plugin_simple", so, open);
    ST_LOAD_FUNCTION("plugin_simple", so, close);
    ST_LOAD_FUNCTION("plugin_simple", so, load_symbol);

    ST_LOAD_FUNCTION("plugin_simple", spcpaths, get_cache_path);

    ST_LOAD_FUNCTION("plugin_simple", zip, open);
    ST_LOAD_FUNCTION("plugin_simple", zip, memopen);
    ST_LOAD_FUNCTION("plugin_simple", zip, close);
    ST_LOAD_FUNCTION("plugin_simple", zip, get_entries_count);
    ST_LOAD_FUNCTION("plugin_simple", zip, get_entry_name);
    ST_LOAD_FUNCTION("plugin_simple", zip, get_entry_type);
    ST_LOAD_FUNCTION("plugin_simple", zip, extract_entry);

    return true;
}

static st_modctx_t *st_plugin_init(st_modctx_t *fs_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx, st_modctx_t *so_ctx, st_modctx_t *spcpaths_ctx,
 st_modctx_t *zip_ctx) {
    st_modctx_t        *plugin_ctx;
    st_plugin_simple_t *module;

    plugin_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_plugin_simple_data, sizeof(st_plugin_simple_t));

    if (!plugin_ctx)
        return NULL;

    plugin_ctx->funcs = &st_plugin_simple_funcs;

    module = plugin_ctx->data;
    module->fs.ctx        = fs_ctx;
    module->logger.ctx    = logger_ctx;
    module->pathtools.ctx = pathtools_ctx;
    module->so.ctx        = so_ctx;
    module->spcpaths.ctx  = spcpaths_ctx;
    module->zip.ctx       = zip_ctx;

    if (!st_plugin_import_functions(plugin_ctx, fs_ctx, logger_ctx,
     pathtools_ctx, so_ctx, spcpaths_ctx, zip_ctx))
        return NULL;

    module->logger.info(module->logger.ctx,
     "plugin_simple: Module initialized.");

    return plugin_ctx;
}

static void st_plugin_quit(st_modctx_t *plugin_ctx) {
    st_plugin_simple_t *module = plugin_ctx->data;

    module->logger.info(module->logger.ctx, "plugin_simple: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, plugin_ctx);
}

static bool st_plugin_load_impl(st_modctx_t *plugin_ctx, st_zip_t *zip,
 const char *filename) {
    st_plugin_simple_t *module = plugin_ctx->data;
    ssize_t             zip_entries_count;
    char                tmp_path[PATH_MAX];
    char                triplet_path[PATH_MAX];

    zip_entries_count = module->zip.get_entries_count(zip);
    if (zip_entries_count == -1) {
        module->logger.error(module->logger.ctx,
         "plugin_simple: Unable to get entries count in plugin \"%s\"",
         filename);

        goto fail;
    }

    module->spcpaths.get_cache_path(module->spcpaths.ctx, tmp_path, PATH_MAX,
     "steroids");

    if (!module->pathtools.concat(module->pathtools.ctx, triplet_path,
     PATH_MAX, tmp_path, ST_TRIPLET)) {
        module->logger.error(module->logger.ctx,
         "plugin_simple: Unable to get triplet path for plugin \"%s\"",
         filename);

        return false;
    }

    for (size_t i = 0; i < (size_t)zip_entries_count; i++) {
        char             entry_name[PATH_MAX];
        char             entry_parent_dir[PATH_MAX];
        char             so_filename[PATH_MAX];
        st_modinitfunc_t modinit_func;
        st_so_t         *so;

        if (module->zip.get_entry_type(zip, i) == ST_ZET_DIR ||
         !module->zip.get_entry_name(zip, entry_name, PATH_MAX, i) ||
         !module->pathtools.get_parent_dir(module->pathtools.ctx,
          entry_parent_dir, PATH_MAX, entry_name) ||
         strcmp(entry_parent_dir, ST_TRIPLET) != 0)
            continue;

        if (!module->pathtools.concat(module->pathtools.ctx, so_filename,
          PATH_MAX, triplet_path, basename(entry_name))) {
            module->logger.error(module->logger.ctx,
             "plugin_simple: Unable to get output filename for plugin entry "
             "\"%s\"", entry_name);

            goto fail;
        }

        if (!module->fs.mkdir(module->fs.ctx, triplet_path) ||
         !module->zip.extract_entry(zip, i, so_filename))
            goto fail;

        so = module->so.open(module->so.ctx, so_filename);
        if (!so)
            goto fail;

        modinit_func = module->so.load_symbol(so, "st_module_init");
        if (!modinit_func) {
            module->logger.error(module->logger.ctx,
             "plugin_simple: Module %s has not function \"st_module_init\"");

            module->so.close(so);

            goto fail;
        }

        return global_modsmgr_funcs.load_module(global_modsmgr, modinit_func);
    }

fail:
    module->logger.error(module->logger.ctx,
     "plugin_simple: Plugin \"%s\" cannot be loaded on this platform",
     filename);
    module->zip.close(zip);

    return false;
}

static bool st_plugin_load(st_modctx_t *plugin_ctx, const char *filename) {
    st_plugin_simple_t *module = plugin_ctx->data;
    st_zip_t           *zip = module->zip.open(module->zip.ctx, filename);

    if (!zip)
        return false;

    return st_plugin_load_impl(plugin_ctx, zip, filename);
}

static bool st_plugin_memload(st_modctx_t *plugin_ctx, const void *data,
 size_t size) {
    st_plugin_simple_t *module = plugin_ctx->data;
    st_zip_t           *zip = module->zip.memopen(module->zip.ctx, data, size);

    if (!zip)
        return false;

    return st_plugin_load_impl(plugin_ctx, zip, "");
}
