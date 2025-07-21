#pragma once

#include "gct-use-cpp.h"
#include_next <string.h>

cGCT_IMPORT_MULTI( //
    MKW(0x80006038)
) void* memset(void* b, int c, size_t len);

cGCT_IMPORT_MULTI( //
    MKW(0x8000F314)
) int memcmp(const void* s1, const void* s2, size_t n);