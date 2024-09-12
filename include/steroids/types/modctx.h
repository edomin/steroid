#pragma once

#include <stdlib.h>

#include "steroids/types/object.h"

#define ST_MODCTX(...)    \
ST_CLASS (                \
    _st_modctx_t _modctx; \
    __VA_ARGS__;          \
)

#define ST_STRUCT_MODCTX(struct_name, ...) \
ST_STRUCT_CLASS (struct_name,              \
    _st_modctx_t _modctx;                  \
    __VA_ARGS__;                           \
)

typedef struct {
    const char *_subsystem;
    const char *_name;
} _st_modctx_t;

ST_MODCTX() st_modctx_t;

static inline void *st_modctx_new(const char *subsystem, const char *name,
 size_t size, void *module, void *funcs) {
    st_modctx_t *ctx = malloc(size);

    if (!ctx)
        return NULL;

    st_object_make(ctx, module, funcs);
    ctx->_modctx._subsystem = subsystem;
    ctx->_modctx._name = name;
}

static inline const char *st_modctx_get_subsystem(const void *modctx) {
    return ((const st_modctx_t *)modctx)->_modctx._subsystem;
}

static inline const char *st_modctx_get_name(const void *modctx) {
    return ((const st_modctx_t *)modctx)->_modctx._name;
}
