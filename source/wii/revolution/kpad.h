#pragma once

#include <revolution/types.h>
#include <revolution/wpad.h>

extern "C" {

struct Vec2 {
    float x;
    float y;
};

struct Vec {
    float x;
    float y;
    float z;
};

struct KPADEXStatus {
    union {
        struct {
            SIZE_ASSERT(0x1C);

            /* 0x00 - 0x60 */ Vec2 stick;
            /* 0x08 - 0x68 */ Vec acc;
            /* 0x14 - 0x74 */ f32 acc_value;
            /* 0x18 - 0x78 */ f32 acc_speed;
        } fs;

        struct {
            SIZE_ASSERT(0x24);

            /* 0x00 - 0x60 */ u32 hold;
            /* 0x04 - 0x64 */ u32 trig;
            /* 0x08 - 0x68 */ u32 release;
            /* 0x0C - 0x6C */ Vec2 lstick;
            /* 0x14 - 0x74 */ Vec2 rstick;
            /* 0x1C - 0x7C */ f32 ltrigger;
            /* 0x20 - 0x80 */ f32 rtrigger;
        } cl;

#if RVL_WBC
        struct {
            SIZE_ASSERT(0x50);

            /* 0x00 - 0x60 */ f64 tgc_weight;
            /* 0x08 - 0x68 */ f64 weight[4];
            /* 0x28 - 0x88 */ f64 weight_ave[4];
            /* 0x48 - 0xA8 */ s32 weight_err;
            /* 0x4C - 0xAC */ s32 tgc_weight_err;
        } bl;
#endif
    };
};

struct KPADStatus {
#if RVL_WBC
    SIZE_ASSERT(0xB0);
#else
    SIZE_ASSERT(0x84);
#endif

    /* 0x00 */ u32 hold;
    /* 0x04 */ u32 trig;
    /* 0x08 */ u32 release;
    /* 0x0C */ Vec acc;
    /* 0x18 */ float acc_value;
    /* 0x1C */ float acc_speed;
    /* 0x20 */ Vec2 pos;
    /* 0x28 */ Vec2 vec;
    /* 0x30 */ f32 speed;
    /* 0x34 */ Vec2 horizon;
    /* 0x3C */ Vec2 hori_vec;
    /* 0x44 */ f32 hori_speed;
    /* 0x48 */ f32 dist;
    /* 0x4C */ f32 dist_vec;
    /* 0x50 */ f32 dist_speed;
    /* 0x54 */ Vec2 acc_vertical;
    /* 0x5C */ WPADDeviceType dev_type;
    /* 0x5D */ s8 wpad_err;
    /* 0x5E */ s8 dpd_valid_fg;
    /* 0x5F */ u8 data_format;
    /* 0x60 */ KPADEXStatus ex_status;
};

/* 0x801ED4F0 */
int KPADReadEx(WPADChannel chan, KPADStatus* status, u32 count, s32* result);

} // extern "C"