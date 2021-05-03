#ifndef ST_UTILS_H
#define ST_UTILS_H

#include <stdbool.h>
#include <stddef.h>

/* string */
bool st_ult_string_create(char **dst_str, size_t *dst_len, const char *src_str,
 size_t src_len);
void st_utl_string_free_if_not_matching(char *str, const char *to_compare);
bool st_utl_strings_equal(const char *left, const char *right);

#endif
