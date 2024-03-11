#include "ketopt.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LONG_OPT_NUM_TO_INDEX_OFFSET 300
#define SHORT_OPTS_FMT_SIZE          100
#define LONGOPTS_COUNT                64
#define OPTS_COLUMNS_MAX            1024
#define HELP_COLUMNS_MIN               8

static st_modsmgr_t      *global_modsmgr;
static st_modsmgr_funcs_t global_modsmgr_funcs;

typedef enum {
    ST_OT_SHORT,
    ST_OT_LONG,
} st_opttype_t;

ST_MODULE_DEF_GET_FUNC(opts_ketopt)
ST_MODULE_DEF_INIT_FUNC(opts_ketopt)

#ifdef ST_MODULE_TYPE_shared
st_moddata_t *st_module_init(st_modsmgr_t *modsmgr,
 st_modsmgr_funcs_t *modsmgr_funcs) {
    return st_module_opts_ketopt_init(modsmgr, modsmgr_funcs);
}
#endif

static bool st_opts_import_functions(st_modctx_t *opts_ctx,
 st_modctx_t *logger_ctx) {
    st_opts_ketopt_t *module = opts_ctx->data;

    module->logger.error = global_modsmgr_funcs.get_function_from_ctx(
     global_modsmgr, logger_ctx, "error");
    if (!module->logger.error) {
        fprintf(stderr,
         "opts_ketopt: Unable to load function \"error\" from module "
         "\"logger\"\n");

        return false;
    }

    ST_LOAD_FUNCTION_FROM_CTX("opts_ketopt", logger, debug);
    ST_LOAD_FUNCTION_FROM_CTX("opts_ketopt", logger, info);
    ST_LOAD_FUNCTION_FROM_CTX("opts_ketopt", logger, warning);

    return true;
}

static st_modctx_t *st_opts_init(int argc, char **argv,
 st_modctx_t *logger_ctx) {
    st_modctx_t      *opts_ctx;
    st_opts_ketopt_t *module;

    opts_ctx = global_modsmgr_funcs.init_module_ctx(global_modsmgr,
     &st_module_opts_ketopt_data, sizeof(st_opts_ketopt_t));

    if (!opts_ctx)
        return NULL;

    opts_ctx->funcs = &st_opts_ketopt_funcs;

    module = opts_ctx->data;
    module->logger.ctx = logger_ctx;

    if (!st_opts_import_functions(opts_ctx, logger_ctx)) {
        global_modsmgr_funcs.free_module_ctx(global_modsmgr, opts_ctx);

        return NULL;
    }

    module->argc = argc;
    module->argv = argv;
    memset(module->opts, '\0', sizeof(st_opt_t) * ST_OPTS_OPTS_MAX);
    module->opts_count = 0;

    module->logger.info(module->logger.ctx, "%s",
     "opts_ketopt: Opts initialized.");

    return opts_ctx;
}

static void st_opts_quit(st_modctx_t *opts_ctx) {
    st_opts_ketopt_t *module = opts_ctx->data;

    module->logger.info(module->logger.ctx, "%s",
     "opts_ketopt: Destroying opts");
    for (unsigned i = 0; i < module->opts_count; i++) {
        free(module->opts[i].longopt);
        free(module->opts[i].arg_fmt);
        free(module->opts[i].opt_descr);
    }

    module->logger.info(module->logger.ctx, "%s",
     "opts_ketopt: Opts destroyed");
    global_modsmgr_funcs.free_module_ctx(global_modsmgr, opts_ctx);
}

static bool st_opts_add_option(st_modctx_t *opts_ctx, char short_option,
 const char *long_option, st_opt_arg_t arg, const char *arg_fmt,
 const char *option_descr) {
    st_opts_ketopt_t *module = opts_ctx->data;
    st_opt_t         *opt = &module->opts[module->opts_count];
    bool              opt_set = false;

    if (!long_option || long_option[0] == '\0' || long_option[1] == '\0') {
        module->logger.error(module->logger.ctx,
         "opts_ketopt: Incorrect long option: %s", long_option);
    } else {
        opt->longopt = strdup(long_option);
        if (!opt->longopt) {
            module->logger.error(module->logger.ctx,
             "opts_ketopt: Unable to allocate memory for long option \"%s\": "
             "%s", long_option, strerror(errno));
        } else {
            opt->longopt_index = (int)module->opts_count +
             LONG_OPT_NUM_TO_INDEX_OFFSET;
            opt_set = true;
        }
    }

    if (short_option) {
        if (isalnum(short_option)) {
            opt->shortopt = short_option;
            opt_set = true;
        } else {
            module->logger.error(module->logger.ctx,
             "opts_ketopt: Incorrect character for short option \"%c\"",
             short_option);
        }
    }

    if (!opt_set)
        return false;

    opt->arg = arg;

    if (arg_fmt) {
        opt->arg_fmt = strdup(arg_fmt);
        if (!opt->arg_fmt)
            module->logger.warning(module->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option argument "
             "format");
    }
    if (option_descr) {
        opt->opt_descr = strdup(option_descr);
        if (!opt->opt_descr)
            module->logger.warning(module->logger.ctx,
             "opts_ketopt: Unable to allocate memory for option description");
    }

    module->opts_count++;

    return true;
}

static bool st_opts_get_str(st_modctx_t *opts_ctx, const char *opt, char *dst,
 size_t dstsize) {
    st_opts_ketopt_t *module = opts_ctx->data;
    char              short_opts_fmt[SHORT_OPTS_FMT_SIZE] = "";
    char             *pshortopts_fmt = short_opts_fmt;
    ko_longopt_t      longopts[LONGOPTS_COUNT] = {0};
    size_t            longopts_count = 0;
    int               longopt_index = -1;
    char              shortopt = '\0';
    ketopt_t          kopt = KETOPT_INIT;

    if (!dst || !dstsize)
        return false;

    if (!opt || opt[0] == '\0') {
        module->logger.error(module->logger.ctx, "opts_ketopt: Empty option");

        return false;
    }

    for (unsigned i = 0; i < module->opts_count; i++) {
        if (module->opts[i].shortopt) {
            ptrdiff_t opt_fmt_size = module->opts[i].arg == ST_OA_NO ? 1 : 2;

            if (pshortopts_fmt - short_opts_fmt
             >= SHORT_OPTS_FMT_SIZE - opt_fmt_size) {
                module->logger.error(module->logger.ctx,
                 "opts_ketopt: Short opts format overflow");
            } else {
                *pshortopts_fmt++ = module->opts[i].shortopt;

                if (module->opts[i].arg != ST_OA_NO) {
                    *pshortopts_fmt++ = module->opts[i].arg == ST_OA_REQUIRED
                        ?':'
                        : '?';
                }
            }
        }

        if (module->opts[i].longopt) {
            if (strcmp(module->opts[i].longopt, opt) == 0 ||
             (opt[1] == '\0' && module->opts[i].shortopt == opt[0])) {
                longopt_index = module->opts[i].longopt_index;
                shortopt = module->opts[i].shortopt;
            }

            longopts[longopts_count].name = module->opts[i].longopt;
            longopts[longopts_count].has_arg = (int)module->opts[i].arg;
            longopts[longopts_count].val = module->opts[i].longopt_index;
            longopts_count++;
        }
    }

    while (true) {
        int parse_result = ketopt(&kopt, module->argc, module->argv, true,
         short_opts_fmt, longopts);

        if (parse_result == '?' || parse_result == ':')
            continue;

        if (parse_result < 0)
            break;

        if (parse_result == longopt_index ||
         (opt[1] == '\0' && parse_result == opt[0]) ||
         parse_result == shortopt) {
            int ret;

            if (!kopt.arg) {
                dst[0] = '\0';

                return true;
            }

            ret = snprintf(dst, dstsize, "%s", kopt.arg);
            if (ret < 0 || (size_t)ret == dstsize) {
                module->logger.error(module->logger.ctx,
                 "opts_ketopt: Unable to get option argument");

                return false;
            }

            return true;
        }
    }

    return false;
}

static bool st_opts_get_help(st_modctx_t *opts_ctx, char *dst, size_t dstsize,
 size_t columns) {
    st_opts_ketopt_t *module = opts_ctx->data;
    size_t            block_size;
    size_t            opts_columns;
    size_t            descr_columns;
    int               ret;

    if (columns <= HELP_COLUMNS_MIN)
        columns = HELP_COLUMNS_MIN;

    opts_columns = columns / 2 - 1;
    descr_columns = columns / 2 - 1;

    ret = snprintf(dst, dstsize, "Usage:\n%s [OPTS]\n\n", module->argv[0]);
    if (ret < 0 || (size_t)ret == dstsize)
        return false;
    dst += ret;
    dstsize -= (size_t)ret;

    for (unsigned i = 0; i < module->opts_count; i++) {
        char        opts[OPTS_COLUMNS_MAX] = {0};
        char       *popts = opts + 2;
        size_t      columns = OPTS_COLUMNS_MAX - 2;
        size_t      opts_len;
        const char *descr = module->opts[i].opt_descr;
        size_t      descr_len = strlen(descr);
        int         ret;

        if (module->opts[i].shortopt != ST_OPTS_SHORT_UNSPEC) {
            opts[0] = '-';
            opts[1] = module->opts[i].shortopt;
        } else {
            opts[0] = ' ';
            opts[1] = ' ';
        }

        if (module->opts[i].longopt) {
            opts[2] = module->opts[i].shortopt == ST_OPTS_SHORT_UNSPEC
             ? ' '
             : ',';
            opts[3] = ' ';
            opts[4] = '-';
            opts[5] = '-'; // NOLINT(readability-magic-numbers)

            popts += 4;
            columns -= 4;

            ret = snprintf(popts, columns, "%s", module->opts[i].longopt);
            if (ret < 0 || (size_t)ret == columns)
                return false;
            popts += ret;
            columns -= (size_t)ret;
        }

        if (module->opts[i].arg != ST_OA_NO) {
            if (module->opts[i].arg == ST_OA_OPTIONAL) {
                if (columns-- == 0)
                    return false;
                *popts++ = '[';
            }

            if (columns-- == 0)
                return false;
            *popts++ = module->opts[i].longopt ? '=' : ' ';

            ret = snprintf(popts, columns, "%s", module->opts[i].arg_fmt);
            if (ret < 0 || (size_t)ret == columns)
                return false;
            popts += ret;
            columns -= (size_t)ret;

            if (module->opts[i].arg == ST_OA_OPTIONAL) {
                if (columns-- == 0)
                    return false;
                *popts++ = ']';
            }
        }

        ret = snprintf(dst, dstsize, "%s", opts);
        if (ret < 0 || (size_t)ret == dstsize)
            return false;
        dst += ret;
        dstsize -= (size_t)ret;

        opts_len = strlen(opts);

        if (opts_len > opts_columns) {
            if (dstsize-- == 0)
                return false;
            *dst++ = '\n';
        }

        ret = snprintf(dst, dstsize, "%*s",
         (opts_len > opts_columns) ? opts_columns : (opts_columns - opts_len),
         "");
        if (ret < 0 || (size_t)ret == dstsize)
            return false;
        dst += ret;
        dstsize -= (size_t)ret;

        if (dstsize-- == 0)
            return false;
        *dst++ = ' ';

        while (descr_len > 0) {
            size_t      to_copy_len;
            const char *space = NULL;

            if (descr_len > descr_columns) {
                space = descr + descr_columns;

                while (*space != ' ' && space != descr)
                    space--;

                to_copy_len = (space == descr)
                    ? descr_columns
                    : (size_t)(space - descr);
            } else {
                to_copy_len = descr_len;
            }

            if (dstsize < to_copy_len)
                return false;
            ret = snprintf(dst, to_copy_len + 1, "%s", descr);
            if (ret < 0 || (size_t)ret == dstsize)
                return false;
            dst += to_copy_len;
            dstsize -= to_copy_len;

            if (to_copy_len < descr_len) {
                if (dstsize-- == 0)
                    return false;
                *dst++ = '\n';

                ret = snprintf(dst, dstsize, "%*s", opts_columns, "");
                if (ret < 0 || (size_t)ret == dstsize)
                    return false;
                dst += ret;
                dstsize -= (size_t)ret;
            }

            if (descr == space)
                to_copy_len++;

            descr_len -= to_copy_len;
            descr += to_copy_len;
        }

        if (dstsize-- == 0)
            return false;
        *dst++ = '\n';
    }

    return true;
}
