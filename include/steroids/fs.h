#ifndef ST_STEROIDS_FS_H
#define ST_STEROIDS_FS_H

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/fs.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_fs_init(st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx);
ST_VISIBILITY void st_fs_quit(st_modctx_t *fs_ctx);
ST_VISIBILITY st_filetype_t st_fs_get_file_type(st_modctx_t *fs_ctx,
 const char *filename);
ST_VISIBILITY bool st_fs_mkdir(st_modctx_t *fs_ctx, const char *dirname);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_FS_H */
