#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "steroids/module.h"
#include "steroids/types/modules/rbuf.h"

static st_rbufctx_t *st_rbuf_init(struct st_loggerctx_s *logger_ctx);
static void st_rbuf_quit(st_rbufctx_t *rbuf_ctx);

static st_rbuf_t *st_rbuf_create(st_rbufctx_t *rbuf_ctx, size_t size);
static void st_rbuf_destroy(st_rbuf_t *rbuf);
static bool st_rbuf_push(st_rbuf_t *rbuf, const void *data, size_t size);
static bool st_rbuf_peek(const st_rbuf_t *rbuf, void *data, size_t size);
static bool st_rbuf_pop(st_rbuf_t *rbuf, void *data, size_t size);
static bool st_rbuf_drop(st_rbuf_t *rbuf, size_t size);
static bool st_rbuf_clear(st_rbuf_t *rbuf);
static size_t st_rbuf_get_free_space(const st_rbuf_t *rbuf);
static bool st_rbuf_is_empty(const st_rbuf_t *rbuf);
