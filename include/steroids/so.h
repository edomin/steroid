#pragma once

#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/so.h"

static st_soctx_t *st_so_init(struct st_loggerctx_s *logger_ctx);
static void st_so_quit(st_soctx_t *so_ctx);

static st_so_t *st_so_open(st_soctx_t *so_ctx, const char *filename);
static st_so_t *st_so_memopen(st_soctx_t *so_ctx, const void *data,
 size_t size);
static void st_so_close(st_so_t *so);
static void *st_so_load_symbol(st_so_t *so, const char *name);
