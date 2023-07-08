#ifndef ST_MODULES_ZIP_ZIP_TYPES_H
#define ST_MODULES_ZIP_ZIP_TYPES_H

#include <zip/zip.h>

#include "steroids/types/modules/fs.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/pathtools.h"

typedef struct {
    st_modctx_t          *ctx;
    st_fs_get_file_type_t get_file_type;
} st_zip_zip_fs_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_zip_zip_logger_t;

typedef struct {
    st_modctx_t          *ctx;
    st_pathtools_concat_t concat;
} st_zip_zip_pathtools_t;

typedef struct {
    st_zip_zip_t *module;
    struct zip_t *handle;
} st_zip_t;

typedef struct {
    st_zip_zip_fs_t        fs;
    st_zip_zip_logger_t    logger;
    st_zip_zip_pathtools_t pathtools;
} st_zip_zip_t;

#define ST_ZIP_T_DEFINED

#endif /* ST_MODULES_ZIP_ZIP_TYPES_H */