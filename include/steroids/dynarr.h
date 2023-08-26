#ifndef ST_STEROIDS_DYNARR_H
#define ST_STEROIDS_DYNARR_H

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/dynarr.h"

static st_modctx_t *st_dynarr_init(st_modctx_t *logger_ctx);
static void st_dynarr_quit(st_modctx_t *dynarr_ctx);

static st_dynarr_t *st_dynarr_create(st_modctx_t *dynarr_ctx, size_t data_size,
 size_t initial_capacity);
static void st_dynarr_destroy(st_dynarr_t *dynarr);
static bool st_dynarr_append(st_dynarr_t *dynarr, const void *data);
static bool st_dynarr_set(st_dynarr_t *dynarr, size_t index, const void *data);
static bool st_dynarr_clear(st_dynarr_t *dynarr);
static bool st_dynarr_sort(st_dynarr_t *dynarr,
 int (*cmpfunc)(const void *, const void *, void *), void *userptr);
static bool st_dynarr_export(const st_dynarr_t *dynarr, void *dst,
 size_t index);
static const void *st_dynarr_get(st_dynarr_t *dynarr, size_t index);
static const void *st_dynarr_get_all(st_dynarr_t *dynarr);
static size_t st_dynarr_get_elements_count(const st_dynarr_t *dynarr);
static bool st_dynarr_is_empty(const st_dynarr_t *dynarr);

#endif /* ST_STEROIDS_DYNARR_H */
