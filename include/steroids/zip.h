#pragma once

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/zip.h"

static st_zipctx_t *st_zip_init(st_fsctx_t *fs_ctx,
 struct st_loggerctx_s *logger_ctx, st_pathtoolsctx_t *pathtools_ctx);
static void st_zip_quit(st_zipctx_t *zip_ctx);

static st_zip_t *st_zip_open(st_zipctx_t *zip_ctx, const char *filename);
static st_zip_t *st_zip_memopen(st_zipctx_t *zip_ctx, const void *data,
 size_t size);
static void st_zip_close(st_zip_t *zip);
static ssize_t st_zip_get_entries_count(st_zip_t *zip);
static bool st_zip_get_entry_name(st_zip_t *zip, char *dst, size_t dstsize,
 size_t entrynum);
static st_zipentrytype_t st_zip_get_entry_type(st_zip_t *zip, size_t entrynum);
static bool st_zip_extract_entry(st_zip_t *zip, size_t entrynum,
 const char *path);
