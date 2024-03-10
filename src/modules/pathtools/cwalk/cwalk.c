#include "cwalk.h"

#include <ctype.h>
#include <stdio.h>

#include <cwalk.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(pathtools_cwalk)
ST_MODULE_DEF_INIT_FUNC(pathtools_cwalk)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_pathtools_cwalk_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_pathtools_import_functions(st_modctx_t *pathtools_ctx,
 st_modctx_t *logger_ctx) {
    st_pathtools_cwalk_t *module = pathtools_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "pathtools_cwalk: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("pathtools_cwalk", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("pathtools_cwalk", logger, info);

    return true;
}

static st_modctx_t *st_pathtools_init(st_modctx_t *logger_ctx) {
    st_modctx_t          *pathtools_ctx;
    st_pathtools_cwalk_t *module;

    pathtools_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_pathtools_cwalk_data, sizeof(st_pathtools_cwalk_t));

    if (!pathtools_ctx)
        return NULL;

    pathtools_ctx->funcs = &st_pathtools_cwalk_funcs;

    module = pathtools_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_pathtools_import_functions(pathtools_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, pathtools_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx,
     "pathtools_cwalk: Path tools initialized");

    return pathtools_ctx;
}

static void st_pathtools_quit(st_modctx_t *pathtools_ctx) {
    st_pathtools_cwalk_t *module = pathtools_ctx->data;

    module->logger.info(module->logger.ctx,
     "pathtools_path_normalize: Path tools destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, pathtools_ctx);
}

static bool st_pathtools_is_iri(const char *path) {
    const char *ppath = path;

    if (!path)
        return false;

    while (isalnum(*++ppath)) {
        if (ppath[0] == ':' && ppath[1] == '/' && (ppath - path) > 1)
            return true;
    }

    return false;
}

static void st_pathtools_to_unix(char *dst, size_t dstsize, const char *path) {
    bool scheme_skipped = !st_pathtools_is_iri(path);

    if (st_pathtools_is_iri(path)) {
        bool scheme_ending_begin = false;
        bool first_slashes_begin = false;

        while (path && *path && dstsize--) {
            if (*path == ':')
                scheme_ending_begin = true;
            *dst++ = *path;
            if (scheme_ending_begin && isalnum(*path++))
                break;
        }
    }

    while (path && *path && dstsize--) {
        *dst++ = *path == '\\' ? '/' : *path;
        path++;
    }

    if (dstsize == 0)
        *dst == '\0';
    else
        *++dst == '\0';
}

static bool st_pathtools_is_absolute(const char *path) {
    return path && !st_pathtools_is_iri(path)
     && (path[0] == '/' || path[0] == '~'
      || (isalpha(path[0]) && path[1] == ':'));
}

static bool st_pathtools_is_relative(const char *path) {
    return path
        && !st_pathtools_is_iri(path)
        && !st_pathtools_is_absolute(path);
}

static st_pathtype_t st_pathtools_get_type(const char *path) {
    if (st_pathtools_is_iri(path))
        return ST_PT_IRI;
    else if (st_pathtools_is_absolute(path))
        return ST_PT_ABSOLUTE;
    else if (st_pathtools_is_relative(path))
        return ST_PT_RELATIVE;
    else
        return ST_PT_NOT_PATH;
}

static bool st_pathtools_resolve(
 __attribute__((unused)) st_modctx_t *pathtools_ctx, char *dst, size_t dstsize,
 const char *path) {
    char                  unix_path[dstsize];
    st_pathtype_t         type = st_pathtools_get_type(path);

    if (type != ST_PT_NOT_PATH)
        st_pathtools_to_unix(unix_path, dstsize, path);

    switch (type) {
        case ST_PT_NOT_PATH:
            return false;
        case ST_PT_IRI: {
            st_pathtools_cwalk_t *module = pathtools_ctx->data;

            module->logger.error(module->logger.ctx,
             "pathtools_path_normalize: IRI normalizing is not implemented yet"
            );
            return false;
        }
        case ST_PT_ABSOLUTE:
        case ST_PT_RELATIVE:
            return cwk_path_normalize(path, dst, dstsize) < dstsize;
        default:
            break;
    }

    return false;
}

static bool st_pathtools_get_parent_dir(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path) {
    return st_pathtools_concat(pathtools_ctx, dst, dstsize, path, "..");
}

static bool st_pathtools_concat(st_modctx_t *pathtools_ctx, char *dst,
 size_t dstsize, const char *path, const char *append) {
    char concatenated[dstsize];
    int  ret;

    if (st_pathtools_is_iri(append) || st_pathtools_is_absolute(append))
        ret = snprintf(concatenated, dstsize, "%s", append);
    else
        ret = snprintf(concatenated, dstsize, "%s/%s", path, append);

    return ret > 0 && (size_t)ret < dstsize
        && st_pathtools_resolve(pathtools_ctx, dst, dstsize, concatenated);
}
