#include "utils.h"

#include <string.h>

bool st_utl_strings_equal(const char *left, const char *right) {
    return strcmp(left, right) == 0;
}
