#include "utils.h"

bool ST_UtlStringCreate(char **dstStr, size_t *dstLen, const char *srcStr,
 size_t srcLen) {
    *dstStr = malloc(sizeof(char *) * srcLen);
    if (*dstStr == NULL)
        return false;

    strlcpy(*dstStr, srcStr, srcLen);
    *dstLen = srcLen;

    return true;
}

void ST_UtlStringFreeIfNotMatching(char *str, const char *toCompare) {
    if (str != toCompare)
        free(str);
}
