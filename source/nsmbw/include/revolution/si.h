#pragma once

extern "C" {

typedef void (*SICallback)(s32, u32, void*);
typedef void (*SITypeAndStatusCallback)(s32, u32);

bool SIIsChanBusy(s32 status);
u32 SIGetStatus(s32 chan);
void SISetCommand(s32 param_1, u32 param_2);
void SITransferCommands();
u32 SIEnablePolling(u32 param_1);
u32 SIDisablePolling(u32 param_1);

cGCT_IMPORT( //
    0x801BB390
) bool SITransfer(s32 chan, void* out, u32 outSize, void* in, u32 inSize, SICallback callback, s64 param_7);

cGCT_IMPORT( //
    0x801BBA60
) void SIRefreshSamplingRate();

bool SIGetResponse(s32 chan, void* param_2);

cGCT_IMPORT( //
    0x801BB7C0
) u32 SIGetType(s32 chan);

// PAL: 0x801b39bc
u32 SIGetTypeAsync(s32 param_1, SITypeAndStatusCallback param_2);
}