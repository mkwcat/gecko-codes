#include <gct-use-cpp.h>

#include "KPAD.h"
#include "wbc.h"
#include "wpad.h"
#include <revolution/kpad.h>
#include <revolution/os.h>
#include <revolution/wpad.h>

extern "C" {

static f64 kp_wbc_tgc_weight;
static u16 kp_wbc_ave_sample_count;
static u8 kp_wbc_zero_point_done;
static bool kp_wbc_tgc_weight_issued;
static bool kp_wbc_setup;
static bool kp_wbc_enabled;
static bool kp_wbc_issued;

static u8 kp_wbc_wait_count = 50;
static f32 kp_wbc_ave_count = 400;

static f64 kp_wbc_weight_ave[4];
static f64 kp_wbc_ave_sample[4];

typedef struct {
    /* 0x000 */ KPADStatus data;
    FILL(0x084, 0x51E);
    /* 0x51E */ bool m0x51E; // 0x555 elsewhere
} KPADInsideStatus;

typedef struct {
    SIZE_ASSERT(0x38);

    union {
        WPADStatus base;
        WPADFSStatus fs;
        WPADBLStatus bl;
        WPADCLStatus cl;
    };

    u8 wpad_type;
} wpad_status_data;

// Because the fields aren't in KPADStatus and there's not enough room for them
KPADBLStatus g_blStatus;

static void KPADResetWbcZeroPoint();

void read_kpad_ext_WBC(KPADInsideStatus* inside, wpad_status_data* status)
{
    if (status->base.dev != WPAD_DEV_BALANCE_CHECKER ||
        status->wpad_type != WPAD_FMT_BALANCE_CHECKER) {
        return;
    }

    if (inside->m0x51E) {
        inside->m0x51E = false;
        KPADResetWbcZeroPoint();
    }

    if (WBCGetBatteryLevel(status->bl.battery) == 0) {
        g_blStatus.weight_err = -1;
        return;
    }

    if (kp_wbc_zero_point_done < 3) {
        g_blStatus.weight_err = -2;
        return;
    }

    if (status->bl.temp == 127 || status->bl.temp == -128) {
        g_blStatus.weight_err = -3;
        return;
    }

    g_blStatus.weight_err = WBCRead(&status->bl, g_blStatus.weight, 4);
    if (g_blStatus.weight_err >= 0) {
        for (int i = 0; i < 4; i++) {
            kp_wbc_weight_ave[i] =
                ((f64) kp_wbc_ave_count * (f64) kp_wbc_weight_ave[i] + g_blStatus.weight[i]) /
                (f64) (kp_wbc_ave_count + 1);
        }

        if (kp_wbc_tgc_weight_issued) {
            kp_wbc_ave_sample_count++;
            f64 total = 0;
            for (int i = 0; i < 4; i++) {
                kp_wbc_ave_sample[i] = ((f64) (kp_wbc_ave_sample_count - 1) * kp_wbc_ave_sample[i] +
                                        g_blStatus.weight[i]) /
                                       kp_wbc_ave_sample_count;
                total += kp_wbc_ave_sample[i];
            }

            if (kp_wbc_ave_sample_count == kp_wbc_ave_count) {
                g_blStatus.weight_err = WBCGetTGCWeight(&kp_wbc_tgc_weight, &status->bl, total);
                if (kp_wbc_tgc_weight < -0.5) {
                    g_blStatus.weight_err = -4;
                }
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        g_blStatus.weight_ave[i] = kp_wbc_weight_ave[i];
    }
    g_blStatus.tgc_weight = kp_wbc_tgc_weight;
    OSReport(
        "WBC: %f %f %f %f\n", g_blStatus.weight[0], g_blStatus.weight[1], g_blStatus.weight[2],
        g_blStatus.weight[3]
    );
    g_blStatus.tgc_weight_err = kp_wbc_tgc_weight_issued;
}

void KPADInitWBC()
{
    kp_wbc_issued = false;
    kp_wbc_enabled = false;
    kp_wbc_setup = false;
    KPADResetWbcZeroPoint();
}

static void KPADiControlWbcCallback(WPADChannel chan, WPADResult status)
{
    ASSERT(chan == WPAD_CHAN3);

    kp_wbc_enabled = status == WPAD_ERR_OK;
    kp_wbc_issued = false;
}

static void KPADiUpdateTempWbcCallback(WPADChannel chan, WPADResult status)
{
    ASSERT(chan == WPAD_CHAN3);

    kp_wbc_zero_point_done = status == WPAD_ERR_OK;
    kp_wbc_issued = false;
}

static void KPADResetWbcZeroPoint()
{
    kp_wbc_zero_point_done = 0;
    kp_wbc_ave_sample_count = 0;
    kp_wbc_tgc_weight_issued = 0;
    for (int i = 0; i < 4; i++) {
        kp_wbc_ave_sample[i] = 0;
        kp_wbc_weight_ave[i] = 0;
    }
}

void KPADiSamplingCallback_WBC(WPADChannel chan, WPADBLStatus* status)
{
    if (!kp_wbc_issued && !kp_wbc_enabled) {
        if (WPADControlBLC(chan, WPAD_BLC_ENABLE, KPADiControlWbcCallback) == WPAD_ERR_OK) {
            kp_wbc_issued = true;
        }
        return;
    }

    if (kp_wbc_enabled && !kp_wbc_setup) {
        if (!kp_wbc_issued) {
            if (WBCSetupCalibration()) {
                kp_wbc_issued = true;
            }
            return;
        }

        kp_wbc_setup = WBCGetCalibrationStatus();
        kp_wbc_issued = !kp_wbc_setup;
        return;
    }

    if (kp_wbc_issued || !kp_wbc_setup) {
        return;
    }

    switch (kp_wbc_zero_point_done) {
    case 0:
        if (WPADControlBLC(chan, WPAD_BLC_DISABLE, KPADiUpdateTempWbcCallback) == WPAD_ERR_OK) {
            kp_wbc_issued = true;
        }
        return;

    case 1:
        if (status->temp == 127 || status->temp == -128) {
            KPADResetWbcZeroPoint();
            return;
        }

        kp_wbc_ave_sample_count++;
        if (kp_wbc_wait_count < kp_wbc_ave_sample_count) {
            kp_wbc_zero_point_done = 2;
            kp_wbc_ave_sample_count = 0;
        }
        return;

    case 2:
        if (status->wpad.err != WPAD_ERR_OK) {
            return;
        }

        kp_wbc_ave_sample_count++;
        for (int i = 0; i < 4; i++) {
            kp_wbc_ave_sample[i] =
                ((f64) (kp_wbc_ave_sample_count - 1) * kp_wbc_ave_sample[i] + status->press[i]) /
                kp_wbc_ave_sample_count;
        }

        if (kp_wbc_ave_count < kp_wbc_ave_sample_count) {
            kp_wbc_zero_point_done = 3;
            WBCSetZEROPoint(kp_wbc_ave_sample, 4);
        }
    }
}

GCT_ASM(
    // clang-format off

GCT_INSERT(0x80197364, read_kpad_stick_WBCInsert);
    subi    r3, r30, 0x60; // Move back to beginning of struct
    mr      r4, r29;
    bl      read_kpad_ext_WBC; // Function renamed to read_kpad_ext in newer versions

    lwz     r0, 0x34(r1); // Original instruction
GCT_INSERT_END(read_kpad_stick_WBCInsert);

GCT_INSERT(0x80198068, KPADiSamplingCallback_WBCInsert);
    lwz     r0, 0x8(r1);
    cmplwi  r0, 3;
    bne-    L_KPADiSamplingCallback_WBCInsert_NotWBC;

    mr      r3, r29;
    mr      r4, r30;
    bl      KPADiSamplingCallback_WBC;

L_KPADiSamplingCallback_WBCInsert_NotWBC:;
    lwz     r12, 0x518(r31); // Original instruction
GCT_INSERT_END(KPADiSamplingCallback_WBCInsert);

    // clang-format on
);

} // extern "C"