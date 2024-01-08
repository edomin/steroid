#pragma once

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/fs.h"

static st_modctx_t *st_fs_init(st_modctx_t *logger_ctx,
 st_modctx_t *pathtools_ctx);
static void st_fs_quit(st_modctx_t *fs_ctx);

static st_filetype_t st_fs_get_file_type(st_modctx_t *fs_ctx,
 const char *filename);
static bool st_fs_mkdir(st_modctx_t *fs_ctx, const char *dirname);
