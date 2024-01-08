#pragma once

#include "steroids/module.h"
#include "steroids/types/modules/runner.h"

static st_modctx_t *st_runner_init(st_modctx_t *ini_ctx,
 st_modctx_t *logger_ctx, st_modctx_t *opts_ctx, st_modctx_t *pathtools_ctx,
 st_modctx_t *plugin_ctx);
static void st_runner_quit(st_modctx_t *runner_ctx);

static void st_runner_run(st_modctx_t *runner_ctx, const void *params);
