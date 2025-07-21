#pragma once

#include <revolution/wpad.h>
#include <types.h>

extern "C" {

BOOL WBCSetupCalibration();

BOOL WBCGetCalibrationStatus();

s32 WBCSetZEROPoint(f64* offset, u32 size);

s32 WBCGetBatteryLevel(u8 arg);

s32 WBCRead(WPADBLStatus* status, f64* weight, u32 size);

s32 WBCGetTGCWeight(f64* weight, WPADBLStatus* status, f64 scale);

} // extern "C"