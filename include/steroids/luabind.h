#pragma once

#include <stdbool.h>

#include "steroids/module.h"
#include "steroids/types/modules/luabind.h"

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx);
static void st_luabind_quit(st_modctx_t *luabind_ctx);
