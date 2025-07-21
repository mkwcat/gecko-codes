// Decompilation of wbc.c from RVL SDK in UFC Personal Trainer

#include "wpad.h"
#include <revolution/os.h>
#include <revolution/sc/scapi_prdinfo.h>
#include <revolution/types.h>
#include <revolution/wpad.h>
#include <string.h>

extern "C" {

// wbc.c
static const char* __WBCVersion =
    "<< RVL_SDK - WBC \trelease build: Aug 23 2010 17:34:25 (0x4302_145) >>";
static BOOL wbc_lib_initialized;

// From https://www.w3.org/TR/png/#D-CRCAppendix
// {

#if 0

/* Table of CRCs of all 8-bit messages. */
static unsigned long crc_table[256];

/* Flag: has the table been computed? Initially false. */
static int crc_table_computed = 0;

/* Make the table for a fast CRC. */
static void make_crc_table(void)
{
    unsigned long c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (unsigned long) n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = 1;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC. */
static u32 Calc_crc32(unsigned char* buf, int len)
{
    unsigned long c = 0xffffffff;
    int n;

    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c ^ 0xffffffff;
}

#else

cGCT_IMPORT( //
    0x801D1CA0 // NETCalcCRC32
) u32 Calc_crc32(unsigned char* buf, int len);

#endif

// } // End of CRC32

static void __WBCInit(void);
void (*_initWBC)(void) = __WBCInit;

BOOL _linkedWBC = TRUE;

static f64 wbc_grav_coff;

static s32 __abs_weight;
static u8 wbc_offset;
static u8 batt_thrsh;

static u16 CALB_DATA[4][3];

struct {
    u8 wbc_offset;
    u8 batt_thrsh;
    u8 m0x02;
    /* volatile */ u8 m0x03; // why is this volatile???
    u32 crc;
    u8 _pad[0x10];
} wbcinfo;

static f64 wbc[4][7];
static u8 rxCalib[16];

static void Calc_ab(u16* param1, u16* param2, f64* param3, f64* param4)
{
    f64 var1 = param1[1] - param1[0];
    f64 var2 = param2[1] - param2[0];
    if (var1 == 0 || var2 == 0) {
        *param3 = 0;
        *param4 = param2[0];
    } else {
        *param3 = var2 / var1;
        *param4 = param2[0] - *param3 * param1[0];
    }
}

static u32 calc_read_crc()
{
    u8 buf[0x1C];

    buf[0x00] = CALB_DATA[0][0] >> 8;
    buf[0x01] = CALB_DATA[0][0] & 0xFF;
    buf[0x02] = CALB_DATA[1][0] >> 8;
    buf[0x03] = CALB_DATA[1][0] & 0xFF;
    buf[0x04] = CALB_DATA[2][0] >> 8;
    buf[0x05] = CALB_DATA[2][0] & 0xFF;
    buf[0x06] = CALB_DATA[3][0] >> 8;
    buf[0x07] = CALB_DATA[3][0] & 0xFF;
    buf[0x08] = CALB_DATA[0][1] >> 8;
    buf[0x09] = CALB_DATA[0][1] & 0xFF;
    buf[0x0A] = CALB_DATA[1][1] >> 8;
    buf[0x0B] = CALB_DATA[1][1] & 0xFF;
    buf[0x0C] = CALB_DATA[2][1] >> 8;
    buf[0x0D] = CALB_DATA[2][1] & 0xFF;
    buf[0x0E] = CALB_DATA[3][1] >> 8;
    buf[0x0F] = CALB_DATA[3][1] & 0xFF;
    buf[0x10] = CALB_DATA[0][2] >> 8;
    buf[0x11] = CALB_DATA[0][2] & 0xFF;
    buf[0x12] = CALB_DATA[1][2] >> 8;
    buf[0x13] = CALB_DATA[1][2] & 0xFF;
    buf[0x14] = CALB_DATA[2][2] >> 8;
    buf[0x15] = CALB_DATA[2][2] & 0xFF;
    buf[0x16] = CALB_DATA[3][2] >> 8;
    buf[0x17] = CALB_DATA[3][2] & 0xFF;
    buf[0x18] = wbcinfo.wbc_offset;
    buf[0x19] = wbcinfo.batt_thrsh;
    buf[0x1A] = wbcinfo.m0x02;
    buf[0x1B] = wbcinfo.m0x03;

    return Calc_crc32(buf, 0x1C);
}

static void getOffset(WPADChannel chan, WPADResult result)
{
    u16 local_14[3] = {0x0000, 0x30D4, 0x61A8};
    u16 local_1c[3] = {0x0000, 0x4268, 0x84D0};

    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    wbcinfo.wbc_offset = wbc_offset;

    if (calc_read_crc() != wbcinfo.crc) {
        return;
    }

    for (int i = 0; i < 4; i++) {
        u16 local_24[3];
        u16 local_2c[3];
        for (int j = 0; j < 3; j++) {
            if (wbcinfo.m0x03 == 1) {
                local_24[j] = local_1c[j];
            } else {
                local_24[j] = local_14[j];
            }
            local_2c[j] = CALB_DATA[i][j] - CALB_DATA[i][0];
        }
        for (int j = 0, j2 = 3; j < 2; j++, j2++) {
            Calc_ab(&local_24[j], &local_2c[j], &wbc[i][j], &wbc[i][j2]);
        }
    }

    wbc_lib_initialized = TRUE;
}

static void getBattThrsh(WPADChannel chan, WPADResult result)
{
    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    wbcinfo.batt_thrsh = batt_thrsh;
    wbc_offset = 0;
    WPADGetBLReg(WPADChannel::WPAD_CHAN3, &wbc_offset, 0x20, getOffset);
}

static void get_calibration5(WPADChannel chan, WPADResult result)
{
    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    wbcinfo.m0x02 = rxCalib[0];
    wbcinfo.m0x03 = rxCalib[1];
    batt_thrsh = 0;
    WPADGetBLReg(WPADChannel::WPAD_CHAN3, &batt_thrsh, 0x21, getBattThrsh);
}

static void get_calibration3(WPADChannel chan, WPADResult result)
{
    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    wbcinfo.crc = rxCalib[3] | (rxCalib[2] << 8) | (rxCalib[1] << 16) | (rxCalib[0] << 24);
    memset(rxCalib, 0, 16);
    WPADGetBLCalibration(
        WPADChannel::WPAD_CHAN3, rxCalib, WPAD_BLCLB_BLK5_ADDR, WPAD_BLCLB_BLK5_LEN,
        &get_calibration5
    );
}

static void get_calibration2(WPADChannel chan, WPADResult result)
{
    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    CALB_DATA[0][2] = (rxCalib[1] & ~0xFF00) | ((rxCalib[0] << 8) & 0xFF00);
    CALB_DATA[1][2] = (rxCalib[3] & ~0xFF00) | ((rxCalib[2] << 8) & 0xFF00);
    CALB_DATA[2][2] = (rxCalib[5] & ~0xFF00) | ((rxCalib[4] << 8) & 0xFF00);
    CALB_DATA[3][2] = (rxCalib[7] & ~0xFF00) | ((rxCalib[6] << 8) & 0xFF00);
    memset(rxCalib, 0, 4);
    WPADGetBLCalibration(
        WPADChannel::WPAD_CHAN3, rxCalib, WPAD_BLCLB_BLK3_ADDR, WPAD_BLCLB_BLK3_LEN,
        &get_calibration3
    );
}

static void get_calibration1(WPADChannel chan, WPADResult result)
{
    if (result != WPADResult::WPAD_ERR_OK) {
        return;
    }

    CALB_DATA[0][0] = (rxCalib[1] & ~0xFF00) | ((rxCalib[0] << 8) & 0xFF00);
    CALB_DATA[1][0] = (rxCalib[3] & ~0xFF00) | ((rxCalib[2] << 8) & 0xFF00);
    CALB_DATA[2][0] = (rxCalib[5] & ~0xFF00) | ((rxCalib[4] << 8) & 0xFF00);
    CALB_DATA[3][0] = (rxCalib[7] & ~0xFF00) | ((rxCalib[6] << 8) & 0xFF00);
    CALB_DATA[0][1] = (rxCalib[9] & ~0xFF00) | ((rxCalib[8] << 8) & 0xFF00);
    CALB_DATA[1][1] = (rxCalib[11] & ~0xFF00) | ((rxCalib[10] << 8) & 0xFF00);
    CALB_DATA[2][1] = (rxCalib[13] & ~0xFF00) | ((rxCalib[12] << 8) & 0xFF00);
    CALB_DATA[3][1] = (rxCalib[15] & ~0xFF00) | ((rxCalib[14] << 8) & 0xFF00);
    memset(rxCalib, 0, 16);
    WPADGetBLCalibration(
        WPADChannel::WPAD_CHAN3, rxCalib, WPAD_BLCLB_BLK2_ADDR, WPAD_BLCLB_BLK2_LEN,
        &get_calibration2
    );
}

static void __WBCInit(void)
{
    static BOOL IsFirstInitialized = FALSE;
    if (IsFirstInitialized == FALSE) {
        memset(&wbc, 0, 0xE0U);
        CALB_DATA[0][0] = 0;
        CALB_DATA[1][0] = 0;
        CALB_DATA[2][0] = 0;
        CALB_DATA[3][0] = 0;
        CALB_DATA[0][1] = 0;
        CALB_DATA[1][1] = 0;
        CALB_DATA[2][1] = 0;
        CALB_DATA[3][1] = 0;
        CALB_DATA[0][2] = 0;
        CALB_DATA[1][2] = 0;
        CALB_DATA[2][2] = 0;
        CALB_DATA[3][2] = 0;
        OSRegisterVersion(__WBCVersion);
    }
    IsFirstInitialized = TRUE;

    s32 productArea = SCGetProductArea();
    switch (productArea) {
    case SC_AREA_JPN:
    case SC_AREA_USA:
    case SC_AREA_TWN:
    case SC_AREA_KOR:
        wbc_grav_coff = 0.9990813732147217;
        return;

    case SC_AREA_EUR:
        wbc_grav_coff = 0.997757613658905;
        return;

    case SC_AREA_CHN:
        wbc_grav_coff = 0.9994894862174988;
        return;

    default:
        wbc_grav_coff = 0.9990813732147217;
        return;
    }
}

BOOL WBCSetupCalibration()
{
    wbc_lib_initialized = FALSE;
    __WBCInit();
    memset(rxCalib, 0, 16);
    return WPADGetBLCalibration(
               WPADChannel::WPAD_CHAN3, rxCalib, WPAD_BLCLB_BLK1_ADDR, WPAD_BLCLB_BLK1_LEN,
               &get_calibration1
           ) == WPADResult::WPAD_ERR_OK;
}

BOOL WBCGetCalibrationStatus()
{
    return wbc_lib_initialized;
}

s32 WBCSetZEROPoint(f64* offset, u32 size)
{
    ASSERTMSG(offset != NULL, "The value of offset is null.");
    ASSERTMSG(
        size != 0 && size <= 4, "Out of size error. The value of size should set 4."
    ); // This assert is weird

    if (wbc_lib_initialized == FALSE) {
        return -2;
    }

    if (size < 4 || offset == NULL) {
        return -4;
    }

    for (int i = 0; i < 4; i++) {
        wbc[0][6] = offset[0];
        wbc[1][6] = offset[1];
        wbc[2][6] = offset[2];
        wbc[3][6] = offset[3];
    }

    return 0;
}

s32 WBCRead(WPADBLStatus* status, f64* weight, u32 size)
{
    ASSERTMSG(weight != NULL, "The value of weight is null.");
    ASSERTMSG(
        size != 0 && size <= 4, "Out of size error. The value of size should set 4."
    ); // This assert is weird
    ASSERTMSG(status != NULL, "The value of status is null.");

    if (wbc_lib_initialized == FALSE) {
        return -2;
    }

    if (size < 4 || status == NULL || weight == NULL) {
        return -4;
    }

    int tmpAbsWeight = 0;
    for (int i = 0; i < 4; i++) {
        u16 press = status->press[i];
        int var_r8 = 0;

        for (int j = 1; j < 3; j++) {
            if (press > CALB_DATA[i][j]) {
                var_r8++;
            }
        }

        if (var_r8 == 2) {
            var_r8 -= 1;
        }

        f64 temp_f0 = press - wbc[i][var_r8 + 3] - wbc[i][6];
        f64 temp_f5 = wbc[i][var_r8];
        if (temp_f5 == 0 || temp_f0 == 0) {
            weight[i] = 0;
        } else {
            weight[i] = temp_f0 / temp_f5 / 1000;
        }

        tmpAbsWeight += press - CALB_DATA[i][0];
    }

    __abs_weight = tmpAbsWeight;

    if (tmpAbsWeight > 1000) {
        return 1;
    }
    return 0;
}

s32 WBCGetBatteryLevel(u8 arg)
{
    s32 battery = arg * 2;

    if (battery >= 260) {
        return 4;
    }

    if (battery >= 250 && battery < 260) {
        return 3;
    }

    if (battery >= 240 && battery < 250) {
        return 2;
    }

    if (battery >= 212 && battery < 240) {
        return 1;
    }

    if (battery < 212) {
        return 0;
    }

    return -1;
}

s32 WBCGetTCWeight(f64* weight, WPADBLStatus* status, f64 scale)
{
    ASSERTMSG(status != NULL, "The value of status is null.");

    *weight = 0;

    if (wbc_lib_initialized == FALSE) {
        return -2;
    }

    if (status == NULL) {
        return -4;
    }

    if (status->temp == 127 || status->temp == -128) {
        return -3;
    }

    *weight = scale * (1 - 0.007000000216066837 * ((f64) (status->temp - wbcinfo.m0x02) / 10));

    return 0;
}

f64 WBCGetGCWeight(f64 weight)
{
    return weight * wbc_grav_coff;
}

s32 WBCGetTGCWeight(f64* weight, WPADBLStatus* status, f64 scale)
{
    ASSERTMSG(status != NULL, "The value of status is null.");

    if (wbc_lib_initialized == FALSE) {
        return -2;
    }

    if (status == NULL) {
        return -4;
    }

    f64 tmp;
    s32 ret = WBCGetTCWeight(&tmp, status, scale);
    if (ret < 0) {
        return ret;
    }

    *weight = WBCGetGCWeight(tmp);

    return 0;
}

} // extern "C"