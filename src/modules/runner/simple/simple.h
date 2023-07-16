#ifndef ST_MODULES_RUNNER_SIMPLE_SIMPLE_H
#define ST_MODULES_RUNNER_SIMPLE_SIMPLE_H

#include "config.h" // IWYU pragma: keep
#include "types.h"
#include "steroids/runner.h"

st_runner_funcs_t st_runner_simple_funcs = {
    .runner_init = st_runner_init,
    .runner_quit = st_runner_quit,
    .runner_run  = st_runner_run,
};

#define FUNCS_COUNT 3
st_modfuncstbl_t st_module_runner_simple_funcs_table = {
    .funcs_count = FUNCS_COUNT,
    .entries = {
        {"init", st_runner_init},
        {"quit", st_runner_quit},
        {"run" , st_runner_run},
    }
};

#endif /* ST_MODULES_RUNNER_SIMPLE_SIMPLE_H */
