#pragma once

#include "wpad.h"
#include <types.h>

typedef struct {
    float x;
    float y;
} KPADVec2D;

typedef struct {
    float x;
    float y;
    float z;
} KPADVec3D;

typedef struct {
    /* 0x00 */ u32 hold;
    /* 0x04 */ u32 trigger;
    /* 0x08 */ u32 release;

    /* 0x0C */ KPADVec3D acc;
    /* 0x18 */ float accMagnitude;
    /* 0x1C */ float accVariation;

    /* 0x20 */ KPADVec2D pos;
    FILL(0x28, 0x34);

    /* 0x34 */ KPADVec2D angle;
    FILL(0x3C, 0x58);
    /* 0x58 */ float unk_0x58;

    /* 0x5C */ u8 extensionType;
    /* 0x5D */ s8 error;
    /* 0x5E */ s8 posValid;
    /* 0x5F */ u8 format;

    FILL(0x60, 0x84);
} KPADStatus;

static_assert(sizeof(KPADStatus) == 0x84);