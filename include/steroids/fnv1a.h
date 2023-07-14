#ifndef ST_STEROIDS_FNV1A_H
#define ST_STEROIDS_FNV1A_H

#include <stddef.h>
#include <stdint.h>

#include "steroids/module.h"
#include "steroids/types/modules/fnv1a.h"

static st_modctx_t *st_fnv1a_init(st_modctx_t *logger_ctx);
static void st_fnv1a_quit(st_modctx_t *fnv1a_ctx);

static uint32_t st_fnv1a_u32hash_string(st_modctx_t *fnv1a_ctx,
 const char *str);
static uint32_t st_fnv1a_u32hash_bytes(st_modctx_t *fnv1a_ctx, const void *ptr,
 size_t size);
static st_u32hashstrfunc_t st_fnv1a_get_u32hashstr_func(st_modctx_t *fnv1a_ctx);
static st_u32hashbytesfunc_t st_fnv1a_get_u32hashbytes_func(
 st_modctx_t *fnv1a_ctx);

#endif /* ST_STEROIDS_FNV1A_H */
