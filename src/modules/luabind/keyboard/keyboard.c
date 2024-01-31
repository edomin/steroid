#include "keyboard.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE   1024
#define CTX_METATABLE_NAME "keyboard_ctx"

static st_modsmgr_t                   *global_modsmgr;
static st_modsmgr_funcs_t              global_modsmgr_funcs;
static char                            err_msg_buf[ERR_MSG_BUF_SIZE];

static st_keyboard_init_t              st_keyboard_init;
static st_keyboard_quit_t              st_keyboard_quit;
static st_keyboard_process_t           st_keyboard_process;
static st_keyboard_press_t             st_keyboard_press;
static st_keyboard_release_t           st_keyboard_release;
static st_keyboard_pressed_t           st_keyboard_pressed;
static st_keyboard_get_input_t         st_keyboard_get_input;

static st_lua_get_state_t              st_lua_get_state;
static st_lua_create_userdata_t        st_lua_create_userdata;
static st_lua_create_metatable_t       st_lua_create_metatable;
static st_lua_create_module_t          st_lua_create_module;
static st_lua_set_metatable_t          st_lua_set_metatable;
static st_lua_set_cfunction_to_field_t st_lua_set_cfunction_to_field;
static st_lua_set_integer_to_field_t   st_lua_set_integer_to_field;
static st_lua_set_copy_to_field_t      st_lua_set_copy_to_field;
static st_lua_get_named_userdata_t     st_lua_get_named_userdata;
static st_lua_get_integer_t            st_lua_get_integer;
static st_lua_push_nil_t               st_lua_push_nil;
static st_lua_push_bool_t              st_lua_push_bool;
static st_lua_push_string_t            st_lua_push_string;
static st_lua_pop_t                    st_lua_pop;

static void st_luabind_bind_all(st_modctx_t *luabind_ctx);

ST_MODULE_DEF_GET_FUNC(luabind_keyboard)
ST_MODULE_DEF_INIT_FUNC(luabind_keyboard)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_luabind_keyboard_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_luabind_import_functions(st_modctx_t *luabind_ctx,
 st_modctx_t *lua_ctx, st_modctx_t *logger_ctx) {
    st_luabind_keyboard_t *module = luabind_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "luabind_keyboard: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("luabind_keyboard", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("luabind_keyboard", logger, info);

    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, init);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, quit);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, process);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, press);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, release);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, pressed);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", keyboard, get_input);

    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, get_state);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, create_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, create_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, create_module);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, set_metatable);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, set_cfunction_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, set_integer_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, set_copy_to_field);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, get_named_userdata);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, get_integer);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, push_nil);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, push_bool);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, push_string);
    ST_LOAD_GLOBAL_FUNCTION("luabind_keyboard", lua, pop);

    return true;
}

static st_modctx_t *st_luabind_init(st_modctx_t *logger_ctx,
 st_modctx_t *lua_ctx) {
    st_modctx_t           *luabind_ctx;
    st_luabind_keyboard_t *luabind;

    luabind_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_luabind_keyboard_data, sizeof(st_luabind_keyboard_t));

    if (!luabind_ctx)
        return NULL;

    luabind_ctx->funcs = &st_luabind_keyboard_funcs;

    luabind = luabind_ctx->data;
    luabind->lua.ctx = lua_ctx;
    luabind->logger.ctx = logger_ctx;

    if (!st_luabind_import_functions(luabind_ctx, lua_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);

        return NULL;
    }

    st_luabind_bind_all(luabind_ctx);

    luabind->logger.info(luabind->logger.ctx,
     "luabind_keyboard: Keyboard funcs binding initialized");

    return luabind_ctx;
}

static void st_luabind_quit(st_modctx_t *luabind_ctx) {
    st_luabind_keyboard_t *module = luabind_ctx->data;

    module->logger.info(module->logger.ctx,
     "luabind_keyboard: Keyboard funcs binding destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, luabind_ctx);
}

static int st_keyboard_init_bind(st_luastate_t *lua_state) {
    void        *userdata = st_lua_create_userdata(lua_state,
     sizeof(st_modctx_t *));
    st_modctx_t *events_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, "events_ctx");
    st_modctx_t *logger_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 2, "logger_ctx");

    *(st_modctx_t **)userdata = st_keyboard_init(events_ctx, logger_ctx);
    st_lua_set_metatable(lua_state, CTX_METATABLE_NAME);

    return 1;
}

static int st_keyboard_quit_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_keyboard_quit(keyboard_ctx);

    return 0;
}

static int st_keyboard_process_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);

    st_keyboard_process(keyboard_ctx);

    return 0;
}

static int st_keyboard_press_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    key = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_keyboard_press(keyboard_ctx,
     (st_key_t)key));

    return 1;
}

static int st_keyboard_release_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    key = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_keyboard_release(keyboard_ctx,
     (st_key_t)key));

    return 1;
}

static int st_keyboard_pressed_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    ptrdiff_t    key = st_lua_get_integer(lua_state, 2);

    st_lua_push_bool(lua_state, st_keyboard_pressed(keyboard_ctx,
     (st_key_t)key));

    return 1;
}

static int st_keyboard_get_input_bind(st_luastate_t *lua_state) {
    st_modctx_t *keyboard_ctx = *(st_modctx_t **)st_lua_get_named_userdata(
     lua_state, 1, CTX_METATABLE_NAME);
    const char *input = st_keyboard_get_input(keyboard_ctx);

    if (input)
        st_lua_push_string(lua_state, input);
    else
        st_lua_push_nil(lua_state);

    return 1;
}

static void st_luabind_bind_all(st_modctx_t *luabind_ctx) {
    st_luabind_keyboard_t *module = luabind_ctx->data;
    st_luastate_t         *lua_state = st_lua_get_state(module->lua.ctx);

    st_lua_create_module(lua_state, "Keyboard");
    st_lua_set_cfunction_to_field(lua_state, "new_ctx", st_keyboard_init_bind);

    st_lua_set_integer_to_field(lua_state, "key_space", ST_KEY_SPACE);
    st_lua_set_integer_to_field(lua_state, "key_exclam", ST_KEY_EXCLAM);
    st_lua_set_integer_to_field(lua_state, "key_quotedbl", ST_KEY_QUOTEDBL);
    st_lua_set_integer_to_field(lua_state, "key_numbersign", ST_KEY_NUMBERSIGN);
    st_lua_set_integer_to_field(lua_state, "key_dollar", ST_KEY_DOLLAR);
    st_lua_set_integer_to_field(lua_state, "key_percent", ST_KEY_PERCENT);
    st_lua_set_integer_to_field(lua_state, "key_ampersand", ST_KEY_AMPERSAND);
    st_lua_set_integer_to_field(lua_state, "key_apostrophe", ST_KEY_APOSTROPHE);
    st_lua_set_integer_to_field(lua_state, "key_quoteright", ST_KEY_QUOTERIGHT);
    st_lua_set_integer_to_field(lua_state, "key_parenleft", ST_KEY_PARENLEFT);
    st_lua_set_integer_to_field(lua_state, "key_parenright", ST_KEY_PARENRIGHT);
    st_lua_set_integer_to_field(lua_state, "key_asterisk", ST_KEY_ASTERISK);
    st_lua_set_integer_to_field(lua_state, "key_plus", ST_KEY_PLUS);
    st_lua_set_integer_to_field(lua_state, "key_comma", ST_KEY_COMMA);
    st_lua_set_integer_to_field(lua_state, "key_minus", ST_KEY_MINUS);
    st_lua_set_integer_to_field(lua_state, "key_period", ST_KEY_PERIOD);
    st_lua_set_integer_to_field(lua_state, "key_slash", ST_KEY_SLASH);
    st_lua_set_integer_to_field(lua_state, "key_0", ST_KEY_0);
    st_lua_set_integer_to_field(lua_state, "key_1", ST_KEY_1);
    st_lua_set_integer_to_field(lua_state, "key_2", ST_KEY_2);
    st_lua_set_integer_to_field(lua_state, "key_3", ST_KEY_3);
    st_lua_set_integer_to_field(lua_state, "key_4", ST_KEY_4);
    st_lua_set_integer_to_field(lua_state, "key_5", ST_KEY_5);
    st_lua_set_integer_to_field(lua_state, "key_6", ST_KEY_6);
    st_lua_set_integer_to_field(lua_state, "key_7", ST_KEY_7);
    st_lua_set_integer_to_field(lua_state, "key_8", ST_KEY_8);
    st_lua_set_integer_to_field(lua_state, "key_9", ST_KEY_9);
    st_lua_set_integer_to_field(lua_state, "key_colon", ST_KEY_COLON);
    st_lua_set_integer_to_field(lua_state, "key_semicolon", ST_KEY_SEMICOLON);
    st_lua_set_integer_to_field(lua_state, "key_less", ST_KEY_LESS);
    st_lua_set_integer_to_field(lua_state, "key_equal", ST_KEY_EQUAL);
    st_lua_set_integer_to_field(lua_state, "key_greater", ST_KEY_GREATER);
    st_lua_set_integer_to_field(lua_state, "key_question", ST_KEY_QUESTION);
    st_lua_set_integer_to_field(lua_state, "key_at", ST_KEY_AT);
    st_lua_set_integer_to_field(lua_state, "key_a", ST_KEY_A);
    st_lua_set_integer_to_field(lua_state, "key_b", ST_KEY_B);
    st_lua_set_integer_to_field(lua_state, "key_c", ST_KEY_C);
    st_lua_set_integer_to_field(lua_state, "key_d", ST_KEY_D);
    st_lua_set_integer_to_field(lua_state, "key_e", ST_KEY_E);
    st_lua_set_integer_to_field(lua_state, "key_f", ST_KEY_F);
    st_lua_set_integer_to_field(lua_state, "key_g", ST_KEY_G);
    st_lua_set_integer_to_field(lua_state, "key_h", ST_KEY_H);
    st_lua_set_integer_to_field(lua_state, "key_i", ST_KEY_I);
    st_lua_set_integer_to_field(lua_state, "key_j", ST_KEY_J);
    st_lua_set_integer_to_field(lua_state, "key_k", ST_KEY_K);
    st_lua_set_integer_to_field(lua_state, "key_l", ST_KEY_L);
    st_lua_set_integer_to_field(lua_state, "key_m", ST_KEY_M);
    st_lua_set_integer_to_field(lua_state, "key_n", ST_KEY_N);
    st_lua_set_integer_to_field(lua_state, "key_o", ST_KEY_O);
    st_lua_set_integer_to_field(lua_state, "key_p", ST_KEY_P);
    st_lua_set_integer_to_field(lua_state, "key_q", ST_KEY_Q);
    st_lua_set_integer_to_field(lua_state, "key_r", ST_KEY_R);
    st_lua_set_integer_to_field(lua_state, "key_s", ST_KEY_S);
    st_lua_set_integer_to_field(lua_state, "key_t", ST_KEY_T);
    st_lua_set_integer_to_field(lua_state, "key_u", ST_KEY_U);
    st_lua_set_integer_to_field(lua_state, "key_v", ST_KEY_V);
    st_lua_set_integer_to_field(lua_state, "key_w", ST_KEY_W);
    st_lua_set_integer_to_field(lua_state, "key_x", ST_KEY_X);
    st_lua_set_integer_to_field(lua_state, "key_y", ST_KEY_Y);
    st_lua_set_integer_to_field(lua_state, "key_z", ST_KEY_Z);
    st_lua_set_integer_to_field(lua_state, "key_bracketleft",
     ST_KEY_BRACKETLEFT);
    st_lua_set_integer_to_field(lua_state, "key_backslash", ST_KEY_BACKSLASH);
    st_lua_set_integer_to_field(lua_state, "key_bracketright",
     ST_KEY_BRACKETRIGHT);
    st_lua_set_integer_to_field(lua_state, "key_asciicircum",
     ST_KEY_ASCIICIRCUM);
    st_lua_set_integer_to_field(lua_state, "key_underscore", ST_KEY_UNDERSCORE);
    st_lua_set_integer_to_field(lua_state, "key_grave", ST_KEY_GRAVE);
    st_lua_set_integer_to_field(lua_state, "key_quoteleft", ST_KEY_QUOTELEFT);
    st_lua_set_integer_to_field(lua_state, "key_braceleft", ST_KEY_BRACELEFT);
    st_lua_set_integer_to_field(lua_state, "key_bar", ST_KEY_BAR);
    st_lua_set_integer_to_field(lua_state, "key_braceright", ST_KEY_BRACERIGHT);
    st_lua_set_integer_to_field(lua_state, "key_asciitilde", ST_KEY_ASCIITILDE);

    st_lua_set_integer_to_field(lua_state, "key_backspace", ST_KEY_BACKSPACE);
    st_lua_set_integer_to_field(lua_state, "key_tab", ST_KEY_TAB);
    st_lua_set_integer_to_field(lua_state, "key_linefeed", ST_KEY_LINEFEED);
    st_lua_set_integer_to_field(lua_state, "key_clear", ST_KEY_CLEAR);
    st_lua_set_integer_to_field(lua_state, "key_return", ST_KEY_RETURN);
    st_lua_set_integer_to_field(lua_state, "key_pause", ST_KEY_PAUSE);
    st_lua_set_integer_to_field(lua_state, "key_scroll_lock",
     ST_KEY_SCROLL_LOCK);
    st_lua_set_integer_to_field(lua_state, "key_sys_req", ST_KEY_SYS_REQ);
    st_lua_set_integer_to_field(lua_state, "key_escape", ST_KEY_ESCAPE);
    st_lua_set_integer_to_field(lua_state, "key_delete", ST_KEY_DELETE);

    st_lua_set_integer_to_field(lua_state, "key_home", ST_KEY_HOME);
    st_lua_set_integer_to_field(lua_state, "key_left", ST_KEY_LEFT);
    st_lua_set_integer_to_field(lua_state, "key_up", ST_KEY_UP);
    st_lua_set_integer_to_field(lua_state, "key_right", ST_KEY_RIGHT);
    st_lua_set_integer_to_field(lua_state, "key_down", ST_KEY_DOWN);
    st_lua_set_integer_to_field(lua_state, "key_prior", ST_KEY_PRIOR);
    st_lua_set_integer_to_field(lua_state, "key_page_up", ST_KEY_PAGE_UP);
    st_lua_set_integer_to_field(lua_state, "key_next", ST_KEY_NEXT);
    st_lua_set_integer_to_field(lua_state, "key_page_down", ST_KEY_PAGE_DOWN);
    st_lua_set_integer_to_field(lua_state, "key_end", ST_KEY_END);
    st_lua_set_integer_to_field(lua_state, "key_begin", ST_KEY_BEGIN);

    st_lua_set_integer_to_field(lua_state, "key_select", ST_KEY_SELECT);
    st_lua_set_integer_to_field(lua_state, "key_print", ST_KEY_PRINT);
    st_lua_set_integer_to_field(lua_state, "key_execute", ST_KEY_EXECUTE);
    st_lua_set_integer_to_field(lua_state, "key_insert", ST_KEY_INSERT);
    st_lua_set_integer_to_field(lua_state, "key_undo", ST_KEY_UNDO);
    st_lua_set_integer_to_field(lua_state, "key_redo", ST_KEY_REDO);
    st_lua_set_integer_to_field(lua_state, "key_menu", ST_KEY_MENU);
    st_lua_set_integer_to_field(lua_state, "key_find", ST_KEY_FIND);
    st_lua_set_integer_to_field(lua_state, "key_cancel", ST_KEY_CANCEL);
    st_lua_set_integer_to_field(lua_state, "key_help", ST_KEY_HELP);
    st_lua_set_integer_to_field(lua_state, "key_break", ST_KEY_BREAK);
    st_lua_set_integer_to_field(lua_state, "key_mode_switch",
     ST_KEY_MODE_SWITCH);
    st_lua_set_integer_to_field(lua_state, "key_script_switch",
     ST_KEY_SCRIPT_SWITCH);
    st_lua_set_integer_to_field(lua_state, "key_num_lock", ST_KEY_NUM_LOCK);

    st_lua_set_integer_to_field(lua_state, "key_kp_space", ST_KEY_KP_SPACE);
    st_lua_set_integer_to_field(lua_state, "key_kp_tab", ST_KEY_KP_TAB);
    st_lua_set_integer_to_field(lua_state, "key_kp_enter", ST_KEY_KP_ENTER);
    st_lua_set_integer_to_field(lua_state, "key_kp_f1", ST_KEY_KP_F1);
    st_lua_set_integer_to_field(lua_state, "key_kp_f2", ST_KEY_KP_F2);
    st_lua_set_integer_to_field(lua_state, "key_kp_f3", ST_KEY_KP_F3);
    st_lua_set_integer_to_field(lua_state, "key_kp_f4", ST_KEY_KP_F4);
    st_lua_set_integer_to_field(lua_state, "key_kp_home", ST_KEY_KP_HOME);
    st_lua_set_integer_to_field(lua_state, "key_kp_left", ST_KEY_KP_LEFT);
    st_lua_set_integer_to_field(lua_state, "key_kp_up", ST_KEY_KP_UP);
    st_lua_set_integer_to_field(lua_state, "key_kp_right", ST_KEY_KP_RIGHT);
    st_lua_set_integer_to_field(lua_state, "key_kp_down", ST_KEY_KP_DOWN);
    st_lua_set_integer_to_field(lua_state, "key_kp_prior", ST_KEY_KP_PRIOR);
    st_lua_set_integer_to_field(lua_state, "key_kp_page_up", ST_KEY_KP_PAGE_UP);
    st_lua_set_integer_to_field(lua_state, "key_kp_next", ST_KEY_KP_NEXT);
    st_lua_set_integer_to_field(lua_state, "key_kp_page_down",
     ST_KEY_KP_PAGE_DOWN);
    st_lua_set_integer_to_field(lua_state, "key_kp_end", ST_KEY_KP_END);
    st_lua_set_integer_to_field(lua_state, "key_kp_begin", ST_KEY_KP_BEGIN);
    st_lua_set_integer_to_field(lua_state, "key_kp_insert", ST_KEY_KP_INSERT);
    st_lua_set_integer_to_field(lua_state, "key_kp_delete", ST_KEY_KP_DELETE);
    st_lua_set_integer_to_field(lua_state, "key_kp_equal", ST_KEY_KP_EQUAL);
    st_lua_set_integer_to_field(lua_state, "key_kp_multiply",
     ST_KEY_KP_MULTIPLY);
    st_lua_set_integer_to_field(lua_state, "key_kp_add", ST_KEY_KP_ADD);
    st_lua_set_integer_to_field(lua_state, "key_kp_separator",
     ST_KEY_KP_SEPARATOR);
    st_lua_set_integer_to_field(lua_state, "key_kp_subtract",
     ST_KEY_KP_SUBTRACT);
    st_lua_set_integer_to_field(lua_state, "key_kp_decimal", ST_KEY_KP_DECIMAL);
    st_lua_set_integer_to_field(lua_state, "key_kp_divide", ST_KEY_KP_DIVIDE);

    st_lua_set_integer_to_field(lua_state, "key_kp_0", ST_KEY_KP_0);
    st_lua_set_integer_to_field(lua_state, "key_kp_1", ST_KEY_KP_1);
    st_lua_set_integer_to_field(lua_state, "key_kp_2", ST_KEY_KP_2);
    st_lua_set_integer_to_field(lua_state, "key_kp_3", ST_KEY_KP_3);
    st_lua_set_integer_to_field(lua_state, "key_kp_4", ST_KEY_KP_4);
    st_lua_set_integer_to_field(lua_state, "key_kp_5", ST_KEY_KP_5);
    st_lua_set_integer_to_field(lua_state, "key_kp_6", ST_KEY_KP_6);
    st_lua_set_integer_to_field(lua_state, "key_kp_7", ST_KEY_KP_7);
    st_lua_set_integer_to_field(lua_state, "key_kp_8", ST_KEY_KP_8);
    st_lua_set_integer_to_field(lua_state, "key_kp_9", ST_KEY_KP_9);

    st_lua_set_integer_to_field(lua_state, "key_f1", ST_KEY_F1);
    st_lua_set_integer_to_field(lua_state, "key_f2", ST_KEY_F2);
    st_lua_set_integer_to_field(lua_state, "key_f3", ST_KEY_F3);
    st_lua_set_integer_to_field(lua_state, "key_f4", ST_KEY_F4);
    st_lua_set_integer_to_field(lua_state, "key_f5", ST_KEY_F5);
    st_lua_set_integer_to_field(lua_state, "key_f6", ST_KEY_F6);
    st_lua_set_integer_to_field(lua_state, "key_f7", ST_KEY_F7);
    st_lua_set_integer_to_field(lua_state, "key_f8", ST_KEY_F8);
    st_lua_set_integer_to_field(lua_state, "key_f9", ST_KEY_F9);
    st_lua_set_integer_to_field(lua_state, "key_f10", ST_KEY_F10);
    st_lua_set_integer_to_field(lua_state, "key_f11", ST_KEY_F11);
    st_lua_set_integer_to_field(lua_state, "key_l1", ST_KEY_L1);
    st_lua_set_integer_to_field(lua_state, "key_f12", ST_KEY_F12);
    st_lua_set_integer_to_field(lua_state, "key_l2", ST_KEY_L2);
    st_lua_set_integer_to_field(lua_state, "key_f13", ST_KEY_F13);
    st_lua_set_integer_to_field(lua_state, "key_l3", ST_KEY_L3);
    st_lua_set_integer_to_field(lua_state, "key_f14", ST_KEY_F14);
    st_lua_set_integer_to_field(lua_state, "key_l4", ST_KEY_L4);
    st_lua_set_integer_to_field(lua_state, "key_f15", ST_KEY_F15);
    st_lua_set_integer_to_field(lua_state, "key_l5", ST_KEY_L5);
    st_lua_set_integer_to_field(lua_state, "key_f16", ST_KEY_F16);
    st_lua_set_integer_to_field(lua_state, "key_l6", ST_KEY_L6);
    st_lua_set_integer_to_field(lua_state, "key_f17", ST_KEY_F17);
    st_lua_set_integer_to_field(lua_state, "key_l7", ST_KEY_L7);
    st_lua_set_integer_to_field(lua_state, "key_f18", ST_KEY_F18);
    st_lua_set_integer_to_field(lua_state, "key_l8", ST_KEY_L8);
    st_lua_set_integer_to_field(lua_state, "key_f19", ST_KEY_F19);
    st_lua_set_integer_to_field(lua_state, "key_l9", ST_KEY_L9);
    st_lua_set_integer_to_field(lua_state, "key_f20", ST_KEY_F20);
    st_lua_set_integer_to_field(lua_state, "key_l10", ST_KEY_L10);
    st_lua_set_integer_to_field(lua_state, "key_f21", ST_KEY_F21);
    st_lua_set_integer_to_field(lua_state, "key_r1", ST_KEY_R1);
    st_lua_set_integer_to_field(lua_state, "key_f22", ST_KEY_F22);
    st_lua_set_integer_to_field(lua_state, "key_r2", ST_KEY_R2);
    st_lua_set_integer_to_field(lua_state, "key_f23", ST_KEY_F23);
    st_lua_set_integer_to_field(lua_state, "key_r3", ST_KEY_R3);
    st_lua_set_integer_to_field(lua_state, "key_f24", ST_KEY_F24);
    st_lua_set_integer_to_field(lua_state, "key_r4", ST_KEY_R4);
    st_lua_set_integer_to_field(lua_state, "key_f25", ST_KEY_F25);
    st_lua_set_integer_to_field(lua_state, "key_r5", ST_KEY_R5);
    st_lua_set_integer_to_field(lua_state, "key_f26", ST_KEY_F26);
    st_lua_set_integer_to_field(lua_state, "key_r6", ST_KEY_R6);
    st_lua_set_integer_to_field(lua_state, "key_f27", ST_KEY_F27);
    st_lua_set_integer_to_field(lua_state, "key_r7", ST_KEY_R7);
    st_lua_set_integer_to_field(lua_state, "key_f28", ST_KEY_F28);
    st_lua_set_integer_to_field(lua_state, "key_r8", ST_KEY_R8);
    st_lua_set_integer_to_field(lua_state, "key_f29", ST_KEY_F29);
    st_lua_set_integer_to_field(lua_state, "key_r9", ST_KEY_R9);
    st_lua_set_integer_to_field(lua_state, "key_f30", ST_KEY_F30);
    st_lua_set_integer_to_field(lua_state, "key_r10", ST_KEY_R10);
    st_lua_set_integer_to_field(lua_state, "key_f31", ST_KEY_F31);
    st_lua_set_integer_to_field(lua_state, "key_r11", ST_KEY_R11);
    st_lua_set_integer_to_field(lua_state, "key_f32", ST_KEY_F32);
    st_lua_set_integer_to_field(lua_state, "key_r12", ST_KEY_R12);
    st_lua_set_integer_to_field(lua_state, "key_f33", ST_KEY_F33);
    st_lua_set_integer_to_field(lua_state, "key_r13", ST_KEY_R13);
    st_lua_set_integer_to_field(lua_state, "key_f34", ST_KEY_F34);
    st_lua_set_integer_to_field(lua_state, "key_r14", ST_KEY_R14);
    st_lua_set_integer_to_field(lua_state, "key_f35", ST_KEY_F35);
    st_lua_set_integer_to_field(lua_state, "key_r15", ST_KEY_R15);

    st_lua_set_integer_to_field(lua_state, "key_shift_l", ST_KEY_SHIFT_L);
    st_lua_set_integer_to_field(lua_state, "key_shift_r", ST_KEY_SHIFT_R);
    st_lua_set_integer_to_field(lua_state, "key_control_l", ST_KEY_CONTROL_L);
    st_lua_set_integer_to_field(lua_state, "key_control_r", ST_KEY_CONTROL_R);
    st_lua_set_integer_to_field(lua_state, "key_caps_lock", ST_KEY_CAPS_LOCK);
    st_lua_set_integer_to_field(lua_state, "key_shift_lock", ST_KEY_SHIFT_LOCK);

    st_lua_set_integer_to_field(lua_state, "key_meta_l", ST_KEY_META_L);
    st_lua_set_integer_to_field(lua_state, "key_meta_r", ST_KEY_META_R);
    st_lua_set_integer_to_field(lua_state, "key_alt_l", ST_KEY_ALT_L);
    st_lua_set_integer_to_field(lua_state, "key_alt_r", ST_KEY_ALT_R);
    st_lua_set_integer_to_field(lua_state, "key_super_l", ST_KEY_SUPER_L);
    st_lua_set_integer_to_field(lua_state, "key_super_r", ST_KEY_SUPER_R);
    st_lua_set_integer_to_field(lua_state, "key_hyper_l", ST_KEY_HYPER_L);
    st_lua_set_integer_to_field(lua_state, "key_hyper_r", ST_KEY_HYPER_R);

    st_lua_pop(lua_state, 3);

    st_lua_create_metatable(lua_state, CTX_METATABLE_NAME);

    st_lua_set_cfunction_to_field(lua_state, "__gc", st_keyboard_quit_bind);
    st_lua_set_cfunction_to_field(lua_state, "destroy", st_keyboard_quit_bind);
    st_lua_set_copy_to_field(lua_state, "__index", -1);
    st_lua_set_cfunction_to_field(lua_state, "process",
     st_keyboard_process_bind);
    st_lua_set_cfunction_to_field(lua_state, "press", st_keyboard_press_bind);
    st_lua_set_cfunction_to_field(lua_state, "release",
     st_keyboard_release_bind);
    st_lua_set_cfunction_to_field(lua_state, "pressed",
     st_keyboard_pressed_bind);
    st_lua_set_cfunction_to_field(lua_state, "get_input",
     st_keyboard_get_input_bind);

    st_lua_pop(lua_state, 1);
}
