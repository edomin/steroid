#ifndef ST_UTILS_H
#define ST_UTILS_H

bool ST_UtlStringCreate(char **dstStr, size_t *dstLen, const char *srcStr,
 size_t srcLen);

void ST_UtlStringFreeIfNotMatching(char *str, const char *toCompare);

#endif
