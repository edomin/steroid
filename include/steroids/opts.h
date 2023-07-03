#ifndef ST_STEROIDS_OPTS_H
#define ST_STEROIDS_OPTS_H

#include "steroids/module.h"
#include "steroids/types/modules/opts.h"

#if defined(ST_MODULE_TYPE_internal)
    #define ST_VISIBILITY static
#else
    #define ST_VISIBILITY
#endif

ST_VISIBILITY st_modctx_t *st_opts_init(int argc, char **argv,
 st_modctx_t *logger_ctx);
ST_VISIBILITY void st_opts_quit(st_modctx_t *opts_ctx);

ST_VISIBILITY bool st_opts_add_option(st_modctx_t *opts_ctx, char short_option,
 const char *long_option, st_opt_arg_t arg, const char *arg_fmt,
 const char *option_descr);
ST_VISIBILITY bool st_opts_get_str(st_modctx_t *opts_ctx, const char *opt,
 char *optarg, size_t optarg_size_max);

#undef ST_VISIBILITY

#endif
