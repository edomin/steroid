#ifndef ST_MODULES_LUABIND_WINDOW_WINDOW_H
#define ST_MODULES_LUABIND_WINDOW_WINDOW_H

#include "config.h" // IWYU pragma: keep
#include "steroids/luabind.h"
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/lua.h"
#include "steroids/types/modules/window.h"

typedef struct {
    st_modctx_t      *ctx;
    st_logger_debug_t debug;
    st_logger_info_t  info;
    st_logger_error_t error;
} st_luabind_window_logger_t;

typedef struct {
    st_modctx_t *ctx;
} st_luabind_window_lua_t;

typedef struct {
    st_luabind_window_logger_t logger;
    st_luabind_window_lua_t    lua;
} st_luabind_window_t;

st_luabind_funcs_t st_luabind_window_funcs = {
    .luabind_init = st_luabind_init,
    .luabind_quit = st_luabind_quit,
};

#define FUNCS_COUNT 2
st_modfuncstbl_t st_module_luabind_window_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init", st_luabind_init},
        {"quit", st_luabind_quit},
    }
};

#endif /* ST_MODULES_LUABIND_WINDOW_WINDOW_H */