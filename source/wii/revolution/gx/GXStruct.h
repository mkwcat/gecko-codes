#pragma once

#include <revolution/types.h>

extern "C" {

struct GXColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
};

struct GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
};

struct GXTexObj;

struct GXTlutObj;

} // extern "C"