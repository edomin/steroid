#include "zip.h"

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <zip/zip.h>

#define ERRMSGBUF_SIZE 128

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(zip_zip)
ST_MODULE_DEF_INIT_FUNC(zip_zip)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_zip_zip_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_zip_import_functions(st_modctx_t *zip_ctx,
 st_modctx_t *fs_ctx, st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx) {
    st_zip_zip_t *module = zip_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "zip_zip: Unable to load function \"error\" from module \"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("zip_zip", fs, get_file_type);

    ST_LOAD_FUNCTION_FROM_CTX("zip_zip", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("zip_zip", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("zip_zip", pathtools, concat);

    return true;
}

static st_modctx_t *st_zip_init(st_modctx_t *fs_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx) {
    st_modctx_t  *zip_ctx;
    st_zip_zip_t *module;

    zip_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_zip_zip_data, sizeof(st_zip_zip_t));

    if (!zip_ctx)
        return NULL;

    zip_ctx->funcs = &st_zip_zip_funcs;

    module = zip_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_zip_import_functions(zip_ctx, fs_ctx, logger_ctx, pathtools_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, zip_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "zip_zip: ZIP initialized.");

    return zip_ctx;
}

static void st_zip_quit(st_modctx_t *zip_ctx) {
    st_zip_zip_t *module = zip_ctx->data;

    module->logger.info(module->logger.ctx, "zip_zip: ZIP destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, zip_ctx);
}

static st_zip_t *st_zip_open(st_modctx_t *zip_ctx, const char *filename) {
    st_zip_zip_t *module = zip_ctx->data;
    struct zip_t *handle;
    st_zip_t     *zip;

    handle = zip_open(filename, 0, 'r');
    if (!handle) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open file: %s", filename);

        return NULL;
    }

    zip = malloc(sizeof(st_zip_t));
    if (!zip) {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "zip_zip: Unable to allocate memory for opened zip structure "
             "while opening file \"%s\": %s", filename, errbuf);

        zip_close(handle);

        return NULL;
    }

    zip->module = module;
    zip->handle = handle;
    zip->type   = ST_ZT_FILE;

    return zip;
}

static st_zip_t *st_zip_memopen(st_modctx_t *zip_ctx, const void *data,
 size_t size) {
    st_zip_zip_t *module = zip_ctx->data;
    struct zip_t *handle;
    st_zip_t     *zip;

    handle = zip_stream_open(data, size, 0, 'r');
    if (!handle) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to open zip from memory");

        return NULL;
    }

    zip = malloc(sizeof(st_zip_t));
    if (!zip) {
        char errbuf[ERRMSGBUF_SIZE];

        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            module->logger.error(module->logger.ctx,
             "zip_zip: Unable to allocate memory for opened zip structure: %s",
             errbuf);

        zip_close(handle);

        return NULL;
    }

    zip->module = module;
    zip->handle = handle;
    zip->type   = ST_ZT_MEM;

    return zip;
}

static void st_zip_close(st_zip_t *zip) {
    if (zip->type == ST_ZT_FILE)
        zip_close(zip->handle);
    else
        zip_stream_close(zip->handle);

    free(zip);
}

static ssize_t st_zip_get_entries_count(st_zip_t *zip) {
    st_zip_zip_t *module = zip->module;
    ssize_t       ret;

    if (!zip || !zip->handle)
        return 0;

    ret = zip_entries_total(zip->handle);
    if (ret < 0) {
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to get entries count: %s", zip_strerror((int)ret));

        return -1;
    }

    return ret;
}

static bool st_zip_get_entry_name(st_zip_t *zip, char *dst, size_t dstsize,
 size_t entrynum) {
    st_zip_zip_t *module = zip->module;
    int           ret;
    const char   *entry_name;

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

    ret = snprintf(dst, dstsize, "%s", entry_name);
    if (ret < 0 || (size_t)ret == dstsize)
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to get entry name");

    zip_entry_close(zip->handle);

    return ret > 0 && (size_t)ret <= dstsize;
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
            module->logger.error(module->logger.ctx,
             "zip_zip: Unable get output filename for entry: %s",
             zip_entry_name(zip->handle));

            zip_entry_close(zip->handle);

            return false;
        }
        presult_filename = result_filename;
    } else {
        presult_filename = path;
    }

    ret = zip_entry_fread(zip->handle, presult_filename);
    if (ret < 0)
        module->logger.error(module->logger.ctx,
         "zip_zip: Unable to extract entry: %s", zip_strerror(ret));

    zip_entry_close(zip->handle);

    return !ret;
}
