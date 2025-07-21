#pragma once

#include <revolution/kpad.h>

extern "C" {

// Because the fields aren't in KPADStatus and there's not enough room for them
struct KPADBLStatus {
    SIZE_ASSERT(0x50);

    /* 0x00 - 0x60 */ f64 tgc_weight;
    /* 0x08 - 0x68 */ f64 weight[4];
    /* 0x28 - 0x88 */ f64 weight_ave[4];
    /* 0x48 - 0xA8 */ s32 weight_err;
    /* 0x4C - 0xAC */ s32 tgc_weight_err;
};

extern KPADBLStatus g_blStatus;

} // extern "C"