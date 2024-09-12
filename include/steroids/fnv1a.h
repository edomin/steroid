#pragma once

#include <stddef.h>
#include <stdint.h>

#include "steroids/module.h"
#include "steroids/types/modules/fnv1a.h"

static st_fnv1actx_t *st_fnv1a_init(struct st_loggerctx_s *logger_ctx);
static void st_fnv1a_quit(st_fnv1actx_t *fnv1a_ctx);

static uint32_t st_fnv1a_u32hash_string(st_fnv1actx_t *fnv1a_ctx,
 const char *str);
static uint32_t st_fnv1a_u32hash_bytes(st_fnv1actx_t *fnv1a_ctx,
 const void *ptr, size_t size);
static st_u32hashstrfunc_t st_fnv1a_get_u32hashstr_func(
 st_fnv1actx_t *fnv1a_ctx);
static st_u32hashbytesfunc_t st_fnv1a_get_u32hashbytes_func(
 st_fnv1actx_t *fnv1a_ctx);
