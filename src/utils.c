#include <stdbool.h>
#include <string.h>

bool st_utl_strings_equal(const char *left, const char *right) {
    if (!left && !right)
        return true;

    if (!left || !right)
        return false;

    return strcmp(left, right) == 0;
}
