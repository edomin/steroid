#ifndef ST_STEROIDS_TERMINAL_H
#define ST_STEROIDS_TERMINAL_H

#include "steroids/module.h"
#include "steroids/types/modules/terminal.h"

static st_modctx_t *st_terminal_init(st_modctx_t *logger_ctx);
static void st_terminal_quit(st_modctx_t *terminal_ctx);

static int st_terminal_get_rows_count(st_modctx_t *terminal_ctx);
static int st_terminal_get_cols_count(st_modctx_t *terminal_ctx);

#endif /* ST_STEROIDS_TERMINAL_H */
