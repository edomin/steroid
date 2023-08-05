#include "simple.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

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

void *st_module_fs_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_fs_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_fs_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"fs_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_fs_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_fs_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_fs_import_functions(st_modctx_t *fs_ctx, st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx) {
    st_fs_simple_t *module = fs_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "fs_simple: Unable to load function \"error\" from module \"logger\"\n"
        );

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("fs_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("fs_simple", logger, info);

    ST_LOAD_FUNCTION_FROM_CTX("fs_simple", pathtools, resolve);

    return true;
}

static st_modctx_t *st_fs_init(st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx) {
    st_modctx_t    *fs_ctx;
    st_fs_simple_t *fs;

    fs_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_fs_simple_data, sizeof(st_fs_simple_t));

    if (!fs_ctx)
        return NULL;

    fs_ctx->funcs = &st_fs_simple_funcs;

    fs = fs_ctx->data;
    fs->logger.ctx = logger_ctx;
    fs->pathtools.ctx = pathtools_ctx;

    if (!st_fs_import_functions(fs_ctx, logger_ctx, pathtools_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, fs_ctx);

        return NULL;
    }

    fs->logger.info(fs->logger.ctx, "fs_simple: Filesystem mgr initialized.");

    return fs_ctx;
}

static void st_fs_quit(st_modctx_t *fs_ctx) {
    st_fs_simple_t *fs = fs_ctx->data;

    fs->logger.info(fs->logger.ctx, "fs_simple: Filesystem mgr destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, fs_ctx);
}

static st_filetype_t st_fs_get_file_type(
 __attribute__((unused)) st_modctx_t *fs_ctx, const char *filename) {
    struct stat path_stat;

    if (stat(filename, &path_stat) == -1)
        return ST_FT_UNKNOWN;

    switch ((unsigned)path_stat.st_mode & (unsigned)S_IFMT) {
        case S_IFREG:
            return ST_FT_REG;
        case S_IFDIR:
            return ST_FT_DIR;
        case S_IFCHR:
            return ST_FT_CHR;
        case S_IFBLK:
            return ST_FT_BLK;
        case S_IFIFO:
            return ST_FT_FIFO;
        case S_IFLNK:
            return ST_FT_LINK;
        case S_IFSOCK:
            return ST_FT_SOCK;
        default:
            return ST_FT_UNKNOWN;
    }
}

static bool st_fs_mkdir(st_modctx_t *fs_ctx, const char *dirname) {
    st_fs_simple_t *module = fs_ctx->data;
    char            path[PATH_MAX] = "";
    char           *ch = path;
    bool            last_is_slash;

    if (!module->pathtools.resolve(module->pathtools.ctx, path, PATH_MAX,
     dirname)) {
        module->logger.error(module->logger.ctx,
         "fs_simple: Unable to get resolved path for directory \"%s\"",
         dirname);

        return false;
    }

    if (path[0] && !path[1]) {
        module->logger.error(module->logger.ctx,
         "fs_simple: Unable to create directory with name \"%s\"", path);

        return false;
    }

    if (*ch == '/')
        ch++;

    do {
        struct stat unused;

        while(*ch && *ch != '/')
            ch++;

        last_is_slash = *ch == '/';

        *ch = '\0';



        if (path[0] != '.' && path[1] && stat(path, &unused) != 0 &&
         mkdir(path, S_IRWXU | S_IRGRP | S_IROTH) == -1) { // NOLINT(hicpp-signed-bitwise)
            module->logger.error(module->logger.ctx,
             "fs_simple: Unable to create directory \"%s\"", path);

            return false;
        }

        if (last_is_slash)
            *ch = '/';

        ch++;
    } while (last_is_slash);

    return true;
}