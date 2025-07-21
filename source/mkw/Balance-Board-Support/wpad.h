#pragma once

#include <revolution/wpad.h>

extern "C" {

WPADResult WPADReadExtReg(WPADChannel chan, void* p_buf, u16 len, u16 addr, WPADCallback cb);
WPADResult WPADGetBLCalibration(WPADChannel chan, void* p_buf, u16 addr, u16 len, WPADCallback cb);
WPADResult WPADGetBLReg(WPADChannel chan, void* p_buf, u32 addr, WPADCallback cb);

} // extern "C"