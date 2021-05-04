#include "modules_manager.h"

#include "steroids/types/modules.h"
#define ST_ENABLE_FUNC_DECLS
#include "steroids/types/modules/logger.h"
#include "steroids/types/modules/opts.h"

static st_logger_init_t     st_logger_init;
static st_logger_quit_t     st_logger_quit;
static st_logger_debug_t    st_logger_debug;

static st_opts_init_t       st_opts_init;
static st_opts_quit_t       st_opts_quit;
static st_opts_add_option_t st_opts_add_option;
static st_opts_get_str_t    st_opts_get_str;

int main(int argc, char **argv) {
    st_modsmgr_t *modsmgr = st_modsmgr_init();
    void         *logger;
    void         *opts;
    char          buffer[128] = {0};

    st_logger_init = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_init");
    st_logger_quit = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_quit");
    st_logger_debug = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_debug");


    st_opts_init = st_modsmgr_get_function(modsmgr, "opts", "ketopt",
     "st_opts_init");
    st_opts_quit = st_modsmgr_get_function(modsmgr, "opts", "ketopt",
     "st_opts_quit");
    st_opts_add_option = st_modsmgr_get_function(modsmgr, "opts", "ketopt",
     "st_opts_add_option");
    st_opts_get_str = st_modsmgr_get_function(modsmgr, "opts", "ketopt",
     "st_opts_get_str");

    logger = st_logger_init();
    st_logger_debug(logger, "hello %s", "world");

    opts = st_opts_init(argc, argv, logger);
    st_opts_add_option(opts, 't', "test", ST_OA_OPTIONAL, "ARG", "test option");
    st_opts_get_str(opts, "test", buffer, 128);
    st_logger_debug(logger, "buffer: %s", buffer);

    st_opts_quit(opts);

    st_logger_quit(logger);

    st_modsmgr_destroy(modsmgr);

    return 0;
}
