#include "simple.h"

#include <sys/ioctl.h>
#include <unistd.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#include <safeclib/safe_mem_lib.h>
#include <safeclib/safe_str_lib.h>
#pragma GCC diagnostic pop
#include <safeclib/safe_types.h>

#define ERR_MSG_BUF_SIZE 1024

static void              *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;
static char               err_msg_buf[ERR_MSG_BUF_SIZE];

void *st_module_terminal_simple_get_func(const char *func_name) {
    st_modfuncstbl_t *funcs_table = &st_module_terminal_simple_funcs_table;

    for (size_t i = 0; i < FUNCS_COUNT; i++) {
        if (strcmp(funcs_table->entries[i].func_name, func_name) == 0)
            return funcs_table->entries[i].func_pointer;
    }

    return NULL;
}

st_moddata_t *st_module_terminal_simple_init(void *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    errno_t err = memcpy_s(&global_modsmgr_funcs, sizeof(st_modsmgr_funcs_t),
     modsmgr_funcs, sizeof(st_modsmgr_funcs_t));

    if (err) {
        strerror_s(err_msg_buf, ERR_MSG_BUF_SIZE, err);
        fprintf(stderr, "Unable to init module \"terminal_simple\": %s\n",
         err_msg_buf);

        return NULL;
    }

    global_modsmgr = modsmgr;

    return &st_module_terminal_simple_data;
}

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(void *modsmgr, st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_terminal_simple_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_terminal_import_functions(st_modctx_t *terminal_ctx,
 st_modctx_t *logger_ctx) {
    st_terminal_simple_t *module = terminal_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "terminal_simple: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("terminal_simple", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("terminal_simple", logger, info);

    return true;
}

static st_modctx_t *st_terminal_init(st_modctx_t *logger_ctx) {
    st_modctx_t          *terminal_ctx;
    st_terminal_simple_t *module;

    terminal_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_terminal_simple_data, sizeof(st_terminal_simple_t));

    if (!terminal_ctx)
        return NULL;

    terminal_ctx->funcs = &st_terminal_simple_funcs;

    module = terminal_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_terminal_import_functions(terminal_ctx, logger_ctx))
        return NULL;

    module->logger.info(module->logger.ctx,
     "terminal_simple: Terminal utils initialized.");

    return terminal_ctx;
}

static void st_terminal_quit(st_modctx_t *terminal_ctx) {
    st_terminal_simple_t *module = terminal_ctx->data;

    module->logger.info(module->logger.ctx,
     "terminal_simple: Terminal utils destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, terminal_ctx);
}

static int st_terminal_get_rows_count(
 __attribute__((unused)) st_modctx_t *terminal_ctx) {
    struct winsize ws;

    return ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 ? ws.ws_col : -1;
}

static int st_terminal_get_cols_count(
 __attribute__((unused)) st_modctx_t *terminal_ctx) {
    struct winsize ws;

    return ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 ? ws.ws_row : -1;
}