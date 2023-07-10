#ifndef ST_STEROIDS_TYPES_MODULES_FS_H
#define ST_STEROIDS_TYPES_MODULES_FS_H

#include "steroids/module.h"

typedef enum {
    ST_FT_UNKNOWN,
    ST_FT_REG,
    ST_FT_DIR,
    ST_FT_CHR,
    ST_FT_BLK,
    ST_FT_FIFO,
    ST_FT_LINK,
    ST_FT_SOCK,
} st_filetype_t;

typedef st_modctx_t *(*st_fs_init_t)(st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx);
typedef void (*st_fs_quit_t)(st_modctx_t *fs_ctx);
typedef st_filetype_t (*st_fs_get_file_type_t)(st_modctx_t *fs_ctx,
 const char *filename);
typedef bool (*st_fs_mkdir_t)(st_modctx_t *fs_ctx, const char *dirname);

typedef struct {
    st_fs_init_t          fs_init;
    st_fs_quit_t          fs_quit;
    st_fs_get_file_type_t fs_get_file_type;
    st_fs_mkdir_t         fs_mkdir;
} st_fs_funcs_t;

#endif /* ST_STEROIDS_TYPES_MODULES_FS_H */
