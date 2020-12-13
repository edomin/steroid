#include "modules_manager.h"

#define ST_ENABLE_FUNC_DECLS
#include "steroids/types/modules/logger.h"

static st_logger_init_t  st_logger_init;
static st_logger_quit_t  st_logger_quit;
static st_logger_debug_t st_logger_debug;

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
    st_modsmgr_t *modsmgr = st_modsmgr_init();
    void         *logger;

    st_logger_init = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_init");
    st_logger_quit = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_quit");
    st_logger_debug = st_modsmgr_get_function(modsmgr, "logger", "libsir",
     "st_logger_debug");

    logger = st_logger_init();
    st_logger_debug(logger, "hello %s\n", "world");
    st_logger_quit(logger);

    st_modsmgr_destroy(modsmgr);

    return 0;
}
