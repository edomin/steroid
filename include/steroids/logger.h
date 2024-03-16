#pragma once

#include <stdbool.h>

#include "steroids/module.h"
#include "steroids/types/modules/logger.h"

static st_modctx_t *st_logger_init(st_modctx_t *events_ctx);
static void st_logger_quit(st_modctx_t *logger_ctx);

static bool st_logger_enable_events(st_modctx_t *logger_ctx,
 st_modctx_t *events_ctx);
static bool st_logger_set_stdout_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
static bool st_logger_set_stderr_levels(st_modctx_t *logger_ctx,
 st_loglvl_t levels);
static bool st_logger_set_log_file(st_modctx_t *logger_ctx,
 const char *filename, st_loglvl_t levels);
static bool st_logger_set_callback(st_modctx_t *logger_ctx,
 st_logcbk_t callback, void *userdata, st_loglvl_t levels);
static void st_logger_debug(const st_modctx_t *logger_ctx, const char* format,
 ...);
static void st_logger_info(const st_modctx_t *logger_ctx, const char* format,
 ...);
static void st_logger_warning(const st_modctx_t *logger_ctx, const char* format,
 ...);
static void st_logger_error(const st_modctx_t *logger_ctx, const char* format,
 ...);
static void st_logger_set_postmortem_msg(st_modctx_t *logger_ctx,
 const char *msg);
