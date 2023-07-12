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

static void st_plugin_import_functions(st_modctx_t *plugin_ctx,
 st_modctx_t *fs_ctx, st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx,
 st_modctx_t *so_ctx, st_modctx_t *spcpaths_ctx, st_modctx_t *zip_ctx) {
    st_plugin_simple_t   *module = plugin_ctx->data;
    st_fs_funcs_t        *fs_funcs = (st_fs_funcs_t *)fs_ctx->funcs;
    st_logger_funcs_t    *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;
    st_pathtools_funcs_t *pathtools_funcs =
     (st_pathtools_funcs_t *)pathtools_ctx->funcs;
    st_so_funcs_t        *so_funcs = (st_so_funcs_t *)so_ctx->funcs;
    st_spcpaths_funcs_t  *spcpaths_funcs =
     (st_spcpaths_funcs_t *)spcpaths_ctx->funcs;
    st_zip_funcs_t       *zip_funcs = (st_zip_funcs_t *)zip_ctx->funcs;

    module->fs.mkdir                 = fs_funcs->fs_mkdir;

    module->logger.debug             = logger_funcs->logger_debug;
    module->logger.info              = logger_funcs->logger_info;
    module->logger.error             = logger_funcs->logger_error;

    module->pathtools.get_parent_dir =
     pathtools_funcs->pathtools_get_parent_dir;
    module->pathtools.concat         = pathtools_funcs->pathtools_concat;

    module->so.open                  = so_funcs->so_open;
    module->so.close                 = so_funcs->so_close;
    module->so.load_symbol           = so_funcs->so_load_symbol;

    module->spcpaths.get_cache_path  = spcpaths_funcs->spcpaths_get_cache_path;

    module->zip.open                 = zip_funcs->zip_open;
    module->zip.memopen              = zip_funcs->zip_memopen;
    module->zip.close                = zip_funcs->zip_close;
    module->zip.get_entries_count    = zip_funcs->zip_get_entries_count;
    module->zip.get_entry_name       = zip_funcs->zip_get_entry_name;
    module->zip.get_entry_type       = zip_funcs->zip_get_entry_type;
    module->zip.extract_entry        = zip_funcs->zip_extract_entry;
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

    st_plugin_import_functions(plugin_ctx, fs_ctx, logger_ctx, pathtools_ctx,
     so_ctx, spcpaths_ctx, zip_ctx);
    module = plugin_ctx->data;
    module->fs.ctx        = fs_ctx;
    module->logger.ctx    = logger_ctx;
    module->pathtools.ctx = pathtools_ctx;
    module->so.ctx        = so_ctx;
    module->spcpaths.ctx  = spcpaths_ctx;
    module->zip.ctx       = zip_ctx;

    module->logger.info(module->logger.ctx,
     "plugin_simple: Module initialized.");

    return plugin_ctx;
}

static void st_plugin_quit(st_modctx_t *plugin_ctx) {
    st_plugin_simple_t *module = plugin_ctx->data;

    module->logger.info(module->logger.ctx, "plugin_simple: Module destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, plugin_ctx);
}

static bool st_plugin_load_impl(st_modctx_t *plugin_ctx, st_zip_t *zip) {
    st_plugin_simple_t *module = plugin_ctx->data;
    ssize_t             zip_entries_count;
    char                tmp_path[PATH_MAX];
    char                triplet_path[PATH_MAX];

    zip_entries_count = module->zip.get_entries_count(zip);
    if (zip_entries_count == -1)
        goto fail;

    module->spcpaths.get_cache_path(module->spcpaths.ctx, tmp_path, PATH_MAX,
     "steroids");

    if (!module->pathtools.concat(module->pathtools.ctx, triplet_path,
     PATH_MAX, tmp_path, ST_TRIPLET))
        return false;

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
          PATH_MAX, triplet_path, basename(entry_name)) ||
         !module->fs.mkdir(module->fs.ctx, triplet_path) ||
         !module->zip.extract_entry(zip, i, so_filename))
            goto fail;

        so = module->so.open(module->so.ctx, so_filename);
        if (!so)
            goto fail;

        modinit_func = module->so.load_symbol(so, "st_module_init");
        if (!modinit_func) {
            module->so.close(so);

            goto fail;
        }

        return global_modsmgr_funcs.load_module(global_modsmgr, modinit_func);
    }

fail:
    module->zip.close(zip);

    return false;
}

static bool st_plugin_load(st_modctx_t *plugin_ctx, const char *filename) {
    st_plugin_simple_t *module = plugin_ctx->data;
    st_zip_t           *zip = module->zip.open(module->zip.ctx, filename);

    if (!zip)
        return false;

    return st_plugin_load_impl(plugin_ctx, zip);
}

static bool st_plugin_memload(st_modctx_t *plugin_ctx, const void *data,
 size_t size) {
    st_plugin_simple_t *module = plugin_ctx->data;
    st_zip_t           *zip = module->zip.memopen(module->zip.ctx, data, size);

    if (!zip)
        return false;

    return st_plugin_load_impl(plugin_ctx, zip);
}
