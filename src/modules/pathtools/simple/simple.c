#include "simple.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

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

void *st_module_pathtools_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_pathtools_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_pathtools_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"pathtools_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_pathtools_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_pathtools_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_pathtools_import_functions(st_modctx_t *pathtools_ctx,
 st_modctx_t *logger_ctx) {
    st_pathtools_simple_t *module = pathtools_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "pathtools_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION(logger, debug);
    ST_LOAD_FUNCTION(logger, info);

    return true;
}

static st_modctx_t *st_pathtools_init(st_modctx_t *logger_ctx) {
    st_modctx_t           *pathtools_ctx;
    st_pathtools_simple_t *pathtools;

    pathtools_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_pathtools_simple_data, sizeof(st_pathtools_simple_t));

    if (!pathtools_ctx)
        return NULL;

    pathtools_ctx->funcs = &st_pathtools_simple_funcs;

    pathtools = pathtools_ctx->data;
    pathtools->logger.ctx = logger_ctx;

    if (!st_pathtools_import_functions(pathtools_ctx, logger_ctx))
        return NULL;

    pathtools->logger.info(pathtools->logger.ctx,
     "pathtools_simple: Path tools initialized.");

    return pathtools_ctx;
}

static void st_pathtools_quit(st_modctx_t *pathtools_ctx) {
    st_pathtools_simple_t *pathtools = pathtools_ctx->data;

    pathtools->logger.info(pathtools->logger.ctx,
     "pathtools_simple: Path tools destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, pathtools_ctx);
}

static bool st_pathtools_resolve_absolute(
 __attribute__((unused)) st_modctx_t *pathtools_ctx, char *dst, size_t dstsize,
 const char *path) {
    char   resolved_path[PATH_MAX] = {0};
    char   resolved_path_last = '\0';
    size_t resolved_path_len;

    while (*path) {
        const char *slash;

        if (*path == '/') {
            if (resolved_path_last != '/') {
                strncat_s(resolved_path, PATH_MAX, "/", 1);
                resolved_path_last = '/';
            }

            path++;

            continue;
        }

        slash = strchr(path, '/');

        if (!slash) {
            slash = path;
            while (*slash)
                slash++;
        }

        if (path[0] == '.' && (path[1] == '/' || path[1] == '\0')) {
            path++;

            continue;
        }

        if (path[0] == '.' && path[1] == '.') {
            char *prev_resolv_slash = strrchr(resolved_path, '/');
            char *prev_prev_resolv_slash;

            if (!prev_resolv_slash || prev_resolv_slash == resolved_path)
                return false;

            *prev_resolv_slash = '\0';
            prev_prev_resolv_slash = strrchr(resolved_path, '/');
            if (!prev_prev_resolv_slash ||
             prev_prev_resolv_slash == resolved_path)
                return false;

            *prev_prev_resolv_slash = '\0';

            path += 2;

            continue;
        }

        strncat_s(resolved_path, PATH_MAX, path, (rsize_t)(slash - path));
        resolved_path_last = *(slash - 1);
        path = slash;
    }

    resolved_path_len = strlen(resolved_path);
    if (resolved_path[resolved_path_len - 1] == '/')
        resolved_path[resolved_path_len - 1] = '\0';

    return strncpy_s(dst, dstsize, resolved_path, PATH_MAX) == 0;
}

static bool st_pathtools_resolve_relative(
 __attribute__((unused)) st_modctx_t *pathtools_ctx, char *dst, size_t dstsize,
 const char *path) {
    char   resolved_path[PATH_MAX] = {0};
    char   resolved_path_last = '\0';
    size_t resolved_path_len;

    if (path[0] == '.' && path[1] != '/')
        return strncpy_s(dst, dstsize, ".", 1) == 0;

    if (path[0] == '.' && path[1] == '/') {
        strncpy_s(resolved_path, PATH_MAX, "./", 2);
        path += 2;
    }

    while (*path) {
        const char *slash;

        if (*path == '/') {
            if (resolved_path_last != '/') {
                strncat_s(resolved_path, PATH_MAX, "/", 1);
                resolved_path_last = '/';
            }

            path++;

            continue;
        }

        slash = strchr(path, '/');

        if (!slash) {
            slash = path;
            while (*slash)
                slash++;
        }

        if (path[0] == '.' && (path[1] == '/' || path[1] == '\0')) {
            path++;

            continue;
        }

        if (path[0] == '.' && path[1] == '.') {
            char *prev_resolv_slash = strrchr(resolved_path, '/');
            char *prev_prev_resolv_slash;

            if (prev_resolv_slash[1] == '.' &&
             prev_resolv_slash[2] == '.') {
                strcat_s(resolved_path, PATH_MAX, "/..");
                resolved_path_last = '.';
                path += 2;

                continue;
            }

            if (!prev_resolv_slash) {
                strcpy_s(resolved_path, PATH_MAX, "..");
                resolved_path_last = '.';
                path += 2;

                continue;
            }

            *prev_resolv_slash = '\0';
            prev_prev_resolv_slash = strrchr(resolved_path, '/');
            if (!prev_prev_resolv_slash) {
                strcpy_s(resolved_path, PATH_MAX, "..");
                resolved_path_last = '.';
                path += 2;

                continue;
            }

            *prev_prev_resolv_slash = '\0';

            path += 2;

            continue;
        }

        strncat_s(resolved_path, PATH_MAX, path, (rsize_t)(slash - path));
        resolved_path_last = *(slash - 1);
        path = slash;
    }

    resolved_path_len = strlen(resolved_path);
    if (resolved_path[resolved_path_len - 1] == '/')
        resolved_path[resolved_path_len - 1] = '\0';

    return strncpy_s(dst, dstsize, resolved_path, PATH_MAX) == 0;
}

static bool st_pathtools_resolve(
 __attribute__((unused)) st_modctx_t *pathtools_ctx, char *dst, size_t dstsize,
 const char *path) {
    if (!path || !dst || dstsize == 0)
        return false;

    return path[0] == '/'
     ? st_pathtools_resolve_absolute(pathtools_ctx, dst, dstsize, path)
     : st_pathtools_resolve_relative(pathtools_ctx, dst, dstsize, path);
}

static bool st_pathtools_get_parent_dir(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path) {
    return st_pathtools_concat(pathtools_ctx, dst, dstsize, path, "..");
}

static bool st_pathtools_concat(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append) {
    char joined_path[PATH_MAX];
    char resolved_path[PATH_MAX];

    return strcpy_s(joined_path, PATH_MAX, path) == 0 &&
     strcat_s(joined_path, PATH_MAX, "/") == 0 &&
     strcat_s(joined_path, PATH_MAX, append) == 0 &&
     st_pathtools_resolve(pathtools_ctx, resolved_path, PATH_MAX,
      joined_path) &&
     strncpy_s(dst, dstsize, resolved_path, PATH_MAX) == 0;
}
