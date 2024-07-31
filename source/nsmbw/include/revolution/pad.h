#pragma once

#include "os.h"
#include <types.h>

extern "C" {

// D-pad left
#define PAD_BUTTON_LEFT 0x0001
// D-pad right
#define PAD_BUTTON_RIGHT 0x0002
// D-pad down
#define PAD_BUTTON_DOWN 0x0004
// D-pad up
#define PAD_BUTTON_UP 0x0008
#define PAD_TRIGGER_Z 0x0010
#define PAD_TRIGGER_R 0x0020
#define PAD_TRIGGER_L 0x0040
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800
#define PAD_BUTTON_START 0x1000

// https://github.com/devkitPro/libogc/blob/bc4b778d558915aa40676e33514c4c9ba2af66b8/gc/ogc/pad.h#L46
struct PADStatus {
    /* 0x0 */ u16 button;
    /* 0x2 */ s8 stickX;
    /* 0x3 */ s8 stickY;
    /* 0x4 */ s8 substickX;
    /* 0x5 */ s8 substickY;
    /* 0x6 */ u8 triggerL;
    /* 0x7 */ u8 triggerR;
    /* 0x8 */ u8 analogA;
    /* 0x9 */ u8 analogB;
    /* 0xA */ s8 err;
};

static_assert(sizeof(PADStatus) == 0xC);

// Size: 0xA
struct PADClampRegion {
    /* 0x0 */ u8 unk_00;
    /* 0x1 */ u8 unk_01;
    /* 0x2 */ s8 stickMin;
    /* 0x3 */ s8 unk_03;
    /* 0x4 */ s8 unk_04;
    /* 0x5 */ s8 substickMin;
    /* 0x6 */ s8 unk_06;
    /* 0x7 */ s8 unk_07;
    /* 0x8 */ s8 stickRad;
    /* 0x9 */ s8 substickRad;
};

static_assert(sizeof(PADClampRegion) == 0xA);

void PAD_ClampCircle(s8*, s8*, s8, s8);
void PADClampCircle(PADStatus*);
void PADClampCircle2(PADStatus*, u32);
void PAD_UpdateOrigin(s32);
void PADOriginCallback(s32, u32, void*);
void PADOriginUpdateCallback(s32, u32, void*);
void PADProbeCallback(s32, u32, void*);
void PADTypeAndStatusCallback(s32, u32);
int PADReset(u32);
int PADRecalibrate(u32);
int PADInit(void);
u32 PADRead(PADStatus*);
void PADControlMotor(s32, u32);
int PAD_OnReset(int);
void PAD_SamplingHandler(s32, OSContext*);

cGCT_IMPORT( //
    0x801DDB10
) int __PADDisableRecalibration(int);

void PADSetSpec(u32 model);
u32 PADGetSpec(void);

typedef void (*PADSamplingCallback)(void);

} // extern "C"
