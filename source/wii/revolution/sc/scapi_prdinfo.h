#pragma once

#include <types.h>

extern "C" {

#define SC_PRDINFO_SIZE 0x100

enum SCProductArea : s8 {
    SC_AREA_JPN = 0,
    SC_AREA_USA = 1,
    SC_AREA_EUR = 2,
    SC_AREA_AUS = 3,
    SC_AREA_BRA = 4,
    SC_AREA_TWN = 5,
    SC_AREA_KOR = 6,
    SC_AREA_HKG = 7,
    SC_AREA_ASI = 8,
    SC_AREA_LTN = 9,
    SC_AREA_SAF = 10,
    SC_AREA_CHN = 11,
};

enum SCProductGameRegion : s8 {
    SC_PRD_GAME_REG_JP = 0,
    SC_PRD_GAME_REG_US = 1,
    SC_PRD_GAME_REG_EU = 2,
    SC_PRD_GAME_REG_DEBUG = 3,

    SC_PRD_GAME_REG_KR = 4,
    SC_PRD_GAME_REG_CN = 5,

    SC_PRD_GAME_REG_MAX = 6,
    SC_PRD_GAME_REG_NULL = -1,
};

cGCT_IMPORT_MULTI( //
    MKW(0x801B23A0), NSMBW(0x801DD9A0)
) SCProductArea SCGetProductArea();

cGCT_IMPORT_MULTI( //
    MKW(0x801B24C8), NSMBW(0x801DDA80)
) SCProductGameRegion SCGetProductGameRegion();

} // extern "C"