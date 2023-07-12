#ifndef ST_STEROIDS_RUNNER_H
#define ST_STEROIDS_RUNNER_H

#include "steroids/module.h"
#include "steroids/types/modules/runner.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_runner_init(st_modctx_t *ini_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *opts_ctx, st_modctx_t *plugin_ctx);
ST_VISIBILITY void st_runner_quit(st_modctx_t *runner_ctx);

ST_VISIBILITY void st_runner_run(st_modctx_t *runner_ctx, const void *params);

#undef ST_VISIBILITY

#endif /* ST_STEROIDS_RUNNER_H */
