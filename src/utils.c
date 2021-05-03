#include "utils.h"

#include <stdlib.h>

#include "steroids/string.h"

bool st_ult_string_create(char **dst_str, size_t *dst_len, const char *src_str,
 size_t src_len) {
    *dst_str = malloc(sizeof(char *) * src_len);
    if (*dst_str == NULL)
        return false;

    strlcpy(*dst_str, src_str, src_len);
    *dst_len = src_len;

    return true;
}

void st_utl_string_free_if_not_matching(char *str, const char *to_compare) {
    if (str != to_compare)
        free(str);
}

bool st_utl_strings_equal(const char *left, const char *right) {
    return strcmp(left, right) == 0;
}
