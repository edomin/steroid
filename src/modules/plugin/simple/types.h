#pragma once

#include "steroids/types/modules/fs.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/pathtools.h"
#include "steroids/types/modules/so.h"
#include "steroids/types/modules/spcpaths.h"
#include "steroids/types/modules/zip.h"

typedef struct {
    st_modctx_t  *ctx;
    st_fs_mkdir_t mkdir;
} st_plugin_simple_fs_t;

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_plugin_simple_logger_t;

typedef struct {
    st_modctx_t                  *ctx;
    st_pathtools_get_parent_dir_t get_parent_dir;
    st_pathtools_concat_t         concat;
} st_plugin_simple_pathtools_t;

typedef struct {
    st_modctx_t        *ctx;
    st_so_open_t        open;
    st_so_close_t       close;
    st_so_load_symbol_t load_symbol;
} st_plugin_simple_so_t;

typedef struct {
    st_modctx_t                 *ctx;
    st_spcpaths_get_cache_path_t get_cache_path;
} st_plugin_simple_spcpaths_t;

typedef struct {
    st_modctx_t               *ctx;
    st_zip_open_t              open;
    st_zip_memopen_t           memopen;
    st_zip_close_t             close;
    st_zip_get_entries_count_t get_entries_count;
    st_zip_get_entry_name_t    get_entry_name;
    st_zip_get_entry_type_t    get_entry_type;
    st_zip_extract_entry_t     extract_entry;
} st_plugin_simple_zip_t;

typedef struct {
    st_plugin_simple_fs_t        fs;
    st_plugin_simple_logger_t    logger;
    st_plugin_simple_pathtools_t pathtools;
    st_plugin_simple_so_t        so;
    st_plugin_simple_spcpaths_t  spcpaths;
    st_plugin_simple_zip_t       zip;
} st_plugin_simple_t;

#define ST_INI_T_DEFINED