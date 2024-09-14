#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/htable.h"

static st_htablectx_t *st_htable_init(struct st_loggerctx_s *logger_ctx);
static void st_htable_quit(st_htablectx_t *htable_ctx);

static st_htable_t *st_htable_create(st_htablectx_t *htable_ctx,
 st_u32hashfunc_t hashfunc, st_keyeqfunc_t keyeqfunc, st_freefunc_t keydelfunc,
 st_freefunc_t valdelfunc);
static void st_htable_destroy(st_htable_t *htable);
static bool st_htable_insert(st_htable_t *htable, st_htiter_t *iter,
 const void *key, void *value);
static void *st_htable_get(st_htable_t *htable, const void *key);
static bool st_htable_remove(st_htable_t *htable, const void *key);
static void st_htable_clear(st_htable_t *htable);
static bool st_htable_contains(st_htable_t *htable, const void *key);
static bool st_htable_find(st_htable_t *htable, st_htiter_t *dst,
 const void *key);
static bool st_htable_first(st_htable_t *htable, st_htiter_t *dst);
static bool st_htable_next(st_htiter_t *current, st_htiter_t *dst);
static const void *st_htable_get_iter_key(const st_htiter_t *iter);
static void *st_htable_get_iter_value(const st_htiter_t *iter);
