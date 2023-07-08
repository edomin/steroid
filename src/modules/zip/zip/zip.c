#include "zip.h"

#include <limits.h>
#include <stddef.h>
#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#include <zip/zip.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_zip_zip_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_zip_zip_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_zip_zip_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"zip_zip\": %s\n", err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_zip_zip_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_zip_zip_init(modsmgr, modsmgr_funcs);
}
#endif

static void st_zip_import_functions(st_modctx_t *zip_ctx,
 st_modctx_t *fs_ctx, st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx) {
    st_zip_zip_t         *module = spcpaths_ctx->data;
    st_fs_funcs_t        *fs_funcs = (st_fs_funcs_t *)fs_ctx->funcs;
    st_logger_funcs_t    *logger_funcs = (st_logger_funcs_t *)logger_ctx->funcs;
    st_pathtools_funcs_t *pathtools_funcs =
     (st_pathtools_funcs_t *)pathtools_ctx->funcs;

    module->fs.get_file_type = fs_funcs->fs_get_file_type;

    module->logger.debug = logger_funcs->logger_debug;
    module->logger.info  = logger_funcs->logger_info;
    module->logger.error  = logger_funcs->logger_error;

    module->pathtools.concat = pathtools_funcs->pathtools_concat;
}

static st_modctx_t *st_zip_init(st_modctx_t *logger_ctx) {
    st_modctx_t  *zip_ctx;
    st_zip_zip_t *module;

    zip_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_zip_zip_data, sizeof(st_zip_zip_t));

    if (!zip_ctx)
        return NULL;

    zip_ctx->funcs = &st_zip_zip_funcs;

    st_zip_import_functions(zip_ctx, logger_ctx);
    module = zip_ctx->data;
    module->logger.ctx = logger_ctx;

    module->logger.info(module->logger.ctx, "%s", "zip_zip: ZIP initialized.");

    return zip_ctx;
}

static void st_zip_quit(st_modctx_t *zip_ctx) {
    st_zip_zip_t *module = zip_ctx->data;

    module->logger.info(module->logger.ctx, "%s", "zip_zip: ZIP destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, zip_ctx);
}

static bool st_zip_open(st_modctx_t *zip_ctx, st_zip_t *zip,
 const char *filename) {
    st_zip_zip_t *module = zip_ctx->data;
    struct zip_t *handle;

    if (!zip)
        return false;

    handle = zip_open(filename, 0, 'r');
    if (!handle) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open file: %s", filename);

        return false;
    }

    zip->module = module;
    zip->handle = handle;
    zip->type   = ST_ZT_FILE;

    return true;
}

static bool st_zip_memopen(st_modctx_t *zip_ctx, st_zip_t *zip,
 const void *data, size_t size) {
    st_zip_zip_t *module = zip_ctx->data;
    struct zip_t *handle;

    if (!zip)
        return false;

    handle = zip_stream_open(data, size, 0, 'r');
    if (!handle) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open zip from memory");

        return false;
    }

    zip->module = module;
    zip->handle = handle;
    zip->type   = ST_ZT_MEM;

    return true;
}

static void st_zip_close(st_zip_t *zip) {
    if (zip->type == ST_ZT_FILE)
        zip_close(zip->handle);
    else
        zip_stream_close(zip->handle);
}

static ssize_t st_zip_get_entries_count(st_zip_t *zip) {
    st_zip_zip_t *module = zip->module;
    ssize_t       ret;

    if (!zip || !zip->handle)
        return 0;

    ret = zip_entries_total(zip->handle);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to get entries count: %s", zip_strerror(ret));

        return -1;
    }

    return ret;
}

static bool st_zip_get_entry_name(st_zip_t *zip, char *dst, size_t dstsize,
 size_t entrynum) {
    st_zip_zip_t *module = zip->module;
    int           ret;
    const char   *entry_name;
    errno_t       err;

    if (!zip || !zip->handle)
        return 0;

    ret = zip_entry_openbyindex(zip->handle, entrynum);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open entry: %s", zip_strerror(ret));

        return false;
    }

    entry_name = zip_entry_name(zip->handle);
    if (!entry_name) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to get entry name");
        zip_entry_close(zip->handle);

        return false;
    }

    err = strcpy_s(dst, dstsize, entry_name);
    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to get entry name: %s", err_msg_buf);
    }

    zip_entry_close(zip->handle);

    return !err;
}

static st_zipentrytype_t st_zip_get_entry_type(st_zip_t *zip, size_t entrynum) {
    st_zip_zip_t *module = zip->module;
    int           ret;

    if (!zip || !zip->handle)
        return 0;

    ret = zip_entry_openbyindex(zip->handle, entrynum);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open entry: %s", zip_strerror(ret));

        return ST_ZET_UNKNOWN;
    }

    ret = zip_entry_isdir(zip->handle);
    if (ret < 0)
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to determine entry type: %s", zip_strerror(ret));

    zip_entry_close(zip->handle);

    return ret < 0 ? ST_ZET_UNKNOWN : (ret ? ST_ZET_DIR : ST_ZET_FILE);
}

static bool st_zip_extract_entry(st_zip_t *zip, size_t entrynum,
 const char *path) {
    st_zip_zip_t *module = zip->module;
    char          result_filename[PATH_MAX];
    const char   *presult_filename;
    int           ret;

    if (!zip || !zip->handle)
        return 0;

    ret = zip_entry_openbyindex(zip->handle, entrynum);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open entry: %s", zip_strerror(ret));

        return false;
    }

    if (module->fs.get_file_type(module->fs.ctx, path) == ST_FT_DIR) {
        if (!module->pathtools.concat(module->pathtools.ctx, result_filename,
         PATH_MAX, path, zip_entry_name(zip->handle))) {
            zip_entry_close(zip->handle);

            return false;
        }
        presult_filename = result_filename;
    } else {
        presult_filename = path;
    }

    ret = zip_entry_fread(zip->handle, presult_filename);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to extract entry: %s", zip_strerror(ret));
    }

    zip_entry_close(zip->handle);

    return !ret;
}