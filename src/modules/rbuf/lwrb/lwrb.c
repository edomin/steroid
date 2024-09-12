#include "lwrb.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#include <lwrb.h>
#pragma GCC diagnostic pop

#define ERRMSGBUF_SIZE 128

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

static st_rbufctx_funcs_t rbufctx_funcs = {
    .quit   = st_rbuf_quit,
    .create = st_rbuf_create,
};

static st_rbuf_funcs_t rbuf_funcs = {
    .destroy        = st_rbuf_destroy,
    .push           = st_rbuf_push,
    .peek           = st_rbuf_peek,
    .pop            = st_rbuf_pop,
    .drop           = st_rbuf_drop,
    .clear          = st_rbuf_clear,
    .get_free_space = st_rbuf_get_free_space,
    .is_empty       = st_rbuf_is_empty,
};

ST_MODULE_DEF_GET_FUNC(rbuf_lwrb)
ST_MODULE_DEF_INIT_FUNC(rbuf_lwrb)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_rbuf_lwrb_init(modsmgr, modsmgr_funcs);
}
#endif

static const char *st_module_subsystem = "rbuf";
static const char *st_module_name = "lwrb";

static st_rbufctx_t *st_rbuf_init(struct st_loggerctx_s *logger_ctx) {
    st_rbufctx_t *rbuf_ctx = st_modctx_new(st_module_subsystem, st_module_name,
     sizeof(st_rbufctx_t), NULL, &rbufctx_funcs);

    if (!rbuf_ctx) {
        ST_LOGGERCTX_CALL(logger_ctx, error,
         "rbuf_lwrb: unable to create new rbuf ctx object");

        return NULL;
    }

    rbuf_ctx->logger_ctx = logger_ctx;

    ST_LOGGERCTX_CALL(logger_ctx, info, "rbuf_lwrb: Module initialized");

    return rbuf_ctx;
}

static void st_rbuf_quit(st_rbufctx_t *rbuf_ctx) {
    ST_LOGGERCTX_CALL(rbuf_ctx->logger_ctx, info,
     "rbuf_lwrb: Module destroyed");
    free(rbuf_ctx);
}

static st_rbuf_t *st_rbuf_create(st_rbufctx_t *rbuf_ctx, size_t size) {
    st_rbuf_t *rbuf = malloc(sizeof(lwrb_t) + size);
    char       errbuf[ERRMSGBUF_SIZE];

    if (!rbuf) {
        if (strerror_r(errno, errbuf, ERRMSGBUF_SIZE) == 0)
            ST_LOGGERCTX_CALL(rbuf_ctx->logger_ctx, error,
             "rbuf_lwrb: Unable to allocate memory for ring buffer structure: "
             "%s", errbuf);

        return NULL;
    }

    if (!lwrb_init(&rbuf->handle, rbuf->data, size)) {
        ST_LOGGERCTX_CALL(rbuf_ctx->logger_ctx, error,
         "rbuf_lwrb: Unable to init ring buffer");
        free(rbuf);

        return NULL;
    }

    st_object_make(rbuf, rbuf_ctx, &rbuf_funcs);

    return rbuf;
}

static void st_rbuf_destroy(st_rbuf_t *rbuf) {
    lwrb_free(&rbuf->handle);
    free(rbuf);
}

static bool st_rbuf_push(st_rbuf_t *rbuf, const void *data, size_t size) {
    if (lwrb_get_free(&rbuf->handle) < size)
        return false;

    return lwrb_write(&rbuf->handle, data, size) == size;
}

static bool st_rbuf_peek(const st_rbuf_t *rbuf, void *data, size_t size) {
    return lwrb_peek(&rbuf->handle, 0, data, size) == size;
}

static bool st_rbuf_pop(st_rbuf_t *rbuf, void *data, size_t size) {
    return lwrb_read(&rbuf->handle, data, size) == size;
}

static bool st_rbuf_drop(st_rbuf_t *rbuf, size_t size) {
    return lwrb_skip(&rbuf->handle, size) == size;
}

static bool st_rbuf_clear(st_rbuf_t *rbuf) {
    while (lwrb_get_full(&rbuf->handle)) {
        if (lwrb_skip(&rbuf->handle, 1) != 1)
            return false;
    }

    return true;
}

static size_t st_rbuf_get_free_space(const st_rbuf_t *rbuf) {
    return lwrb_get_free(&rbuf->handle);
}

static bool st_rbuf_is_empty(const st_rbuf_t *rbuf) {
    return lwrb_get_full(&rbuf->handle) == 0;
}
