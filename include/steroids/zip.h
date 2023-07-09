#ifndef ST_STEROIDS_ZIP_H
#define ST_STEROIDS_ZIP_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/zip.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_zip_init(st_modctx_t *fs_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *pathtools_ctx);
ST_VISIBILITY void st_zip_quit(st_modctx_t *zip_ctx);
ST_VISIBILITY bool st_zip_open(st_modctx_t *zip_ctx, st_zip_t *zip,
 const char *filename);
ST_VISIBILITY bool st_zip_memopen(st_modctx_t *zip_ctx, st_zip_t *zip,
 const void *data, size_t size);
ST_VISIBILITY void st_zip_close(st_zip_t *zip);
ST_VISIBILITY ssize_t st_zip_get_entries_count(st_zip_t *zip);
ST_VISIBILITY bool st_zip_get_entry_name(st_zip_t *zip, char *dst,
 size_t dstsize, size_t entrynum);
ST_VISIBILITY st_zipentrytype_t st_zip_get_entry_type(st_zip_t *zip,
 size_t entrynum);
ST_VISIBILITY bool st_zip_extract_entry(st_zip_t *zip, size_t entrynum,
 const char *path);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_ZIP_H */
