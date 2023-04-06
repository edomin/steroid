#include "genid.h"

static uint32_t st_genid_next = 0;

uint32_t st_genid(void) {
    return st_genid_next++;
}
