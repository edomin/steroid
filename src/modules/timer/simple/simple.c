#include "simple.h"

#include <errno.h>
#include <stdio.h>
#include <time.h>

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

ST_MODULE_DEF_GET_FUNC(timer_simple)
ST_MODULE_DEF_INIT_FUNC(timer_simple)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_timer_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_timer_import_functions(st_modctx_t *timer_ctx,
 st_modctx_t *logger_ctx) {
    st_timer_simple_t *module = timer_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "timer_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("timer_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("timer_simple", logger, info);

    return true;
}

static st_modctx_t *st_timer_init(st_modctx_t *logger_ctx) {
    st_modctx_t       *timer_ctx;
    st_timer_simple_t *module;

    timer_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_timer_simple_data, sizeof(st_timer_simple_t));

    if (!timer_ctx)
        return NULL;

    timer_ctx->funcs = &st_timer_simple_funcs;

    module = timer_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_timer_import_functions(timer_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, timer_ctx);

        return NULL;
    }

    module->logger.info(module->logger.ctx, "timer_simple: Timer initialized.");

    return timer_ctx;
}

static void st_timer_quit(st_modctx_t *timer_ctx) {
    st_timer_simple_t *module = timer_ctx->data;

    module->logger.info(module->logger.ctx, "timer_simple: Timer destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, timer_ctx);
}

static uint64_t st_timer_start(st_modctx_t *timer_ctx) {
    return (uint64_t)clock() / (uint64_t)(CLOCKS_PER_SEC / 1000); // NOLINT(readability-magic-numbers)
}

static unsigned st_timer_get_elapsed(st_modctx_t *timer_ctx, uint64_t start) {
    return (unsigned)(start - (uint64_t)clock() /
     (uint64_t)(CLOCKS_PER_SEC / 1000)); // NOLINT(readability-magic-numbers)
}

static void st_timer_sleep(st_modctx_t *timer_ctx, unsigned ms) {
    int             ret;
    struct timespec ts = {
        .tv_sec  = (long)ms / 1000, // NOLINT(readability-magic-numbers)
        .tv_nsec = ((long)ms % 1000) * 1000000, // NOLINT(readability-magic-numbers)
    };

    do
        ret = nanosleep(&ts, &ts);
    while (ret != 0 && errno == EINTR);
}

static void st_timer_sleep_for_fps(st_modctx_t *timer_ctx, unsigned fps) {
    st_timer_sleep(timer_ctx, 1000 / fps); // NOLINT(readability-magic-numbers)
}
