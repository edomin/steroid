#pragma once

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/runner.h"

static st_runner_funcs_t st_runner_simple_funcs = {
    .runner_init = st_runner_init,
    .runner_quit = st_runner_quit,
    .runner_run  = st_runner_run,
};

static st_modfuncentry_t st_module_runner_simple_funcs[] = {
    {"init", st_runner_init},
    {"quit", st_runner_quit},
    {"run" , st_runner_run},
    {NULL, NULL},
};
