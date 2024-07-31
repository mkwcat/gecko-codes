#pragma once

#include <cstdio>
#include <cstring>
#include <cwchar>
#include <stdexcept>

cGCT_IMPORT( //
    0x80004364
) void* memcpy(void* __restrict dst, const void* __restrict src, size_t count);

cGCT_IMPORT( //
    0x800046B4
) void* memset(void* __restrict dst, int value, size_t count);

cGCT_IMPORT( //
    0x802DF264
) void* memmove(void* dst, const void* src, size_t count);

cGCT_IMPORT( //
    0x802E1CE8
) char* strncpy(char* __restrict dst, const char* __restrict src, size_t n);

cGCT_IMPORT( //
    0x802DF1AC
) size_t
    wcstombs(char* __restrict dest, const wchar_t* __restrict src, size_t n);

// mbstowcs
cGCT_IMPORT( //
    0x802DF0EC
) size_t
    mbstowcs(wchar_t* __restrict dest, const char* __restrict src, size_t n);

cGCT_IMPORT( //
    0x802E19D8
) int snprintf(char* __restrict s, size_t n, const char* __restrict format, ...);

void std::__throw_length_error(const char* str)
{
    __builtin_unreachable();
}