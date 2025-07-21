#include "WUD.h"
#include <revolution/os.h>
#include <revolution/wpad.h>

extern "C" {

extern BOOL _linkedWBC;

BOOL WUDIsLinkedWBC()
{
    return _linkedWBC;
}

#define WPAD_EXT_REG_EXTENSION 0xA4
#define WM_EXT_REG_ADDR(type_, addr_)                                                              \
    (((addr_) & 0xffff) | ((WPAD_EXT_REG_##type_) << 16) | (1 << 26))
#define WM_REG_EXTENSION_CERT_CHALLENGE WM_EXT_REG_ADDR(EXTENSION, 0xf1)

struct WPADCmdQueue {
    SIZE_ASSERT(0xC);
    /* 0x0 */ s8 indexOut;
    /* 0x1 */ s8 indexIn;
    /* 0x4 */ struct WPADCmd* queue;
    /* 0x8 */ u32 length;
};

extern struct wpad_cb_st {
    FILL(0x000, 0x160);
    /* 0x160 */ struct WPADCmdQueue stdCmdQueue;
    FILL(0x16C, 0x8BC);
    /* 0x8BC */ WPADResult status;
    FILL(0x8C0, 0x8D8);
    /* 0x8D8 */ BOOL used;
    /* 0x8DC */ BOOL handshakeFinished;
    FILL(0x8E0, 0x980);
    /* 0x980 */ u16 wmReadLength;
}* __rvl_p_wpadcb[4] asm("PORT(0x80350930)");

cGCT_IMPORT( //
    0x801C47E8
) int WPADiSendWriteDataCmd();

cGCT_IMPORT( //
    0x801C499C
) BOOL
    WPADiSendWriteData(
        struct WPADCmdQueue* cmdQueue, const void* p_buf, u16 len, u32 address, WPADCallback* cb
    );

cGCT_IMPORT( //
    0x801C4B54
) BOOL
    WPADiSendReadData(
        struct WPADCmdQueue* cmdQueue, void* p_buf, u16 len, u32 address, WPADCallback* cb
    );

static s8 __wpadGetQueueSize(struct WPADCmdQueue* cmdQueue)
{
    BOOL intrStatus = OSDisableInterrupts();

    s8 queueRemaining = cmdQueue->indexIn - cmdQueue->indexOut;

    if (queueRemaining < 0)
        queueRemaining += cmdQueue->length;

    OSRestoreInterrupts(intrStatus);

    return queueRemaining;
}

BOOL WPADiIsAvailableCmdQueue(struct WPADCmdQueue* cmdQueue, s8 num)
{
    s8 queueSize = __wpadGetQueueSize(cmdQueue);

    if ((u32) (queueSize + num) <= cmdQueue->length - 1)
        return TRUE;
    else
        return FALSE;
}

WPADResult WPADReadExtReg(WPADChannel chan, void* p_buf, u16 len, u16 addr, WPADCallback cb)
{
    struct wpad_cb_st* p_wpd = __rvl_p_wpadcb[chan];

    BOOL interrupt = OSDisableInterrupts();
    WPADResult status = p_wpd->status;
    BOOL handshakeFinished = p_wpd->handshakeFinished;
    OSRestoreInterrupts(interrupt);

    if (status != WPAD_ERR_NO_CONTROLLER) {
        if (handshakeFinished == FALSE) {
            status = WPAD_ERR_COMMUNICATION_ERROR;
        } else {
            if (WPADiSendReadData(&p_wpd->stdCmdQueue, p_buf, len, addr | 0x4A40000, cb) == FALSE) {
                status = WPAD_ERR_COMMUNICATION_ERROR;
            } else {
                status = WPAD_ERR_OK;
            }
        }
    }

    if (status != WPAD_ERR_OK && cb != NULL) {
        cb(chan, status);
    }

    return status;
}

WPADResult WPADWriteExtReg(
    WPADChannel chan, const void* data, u16 length, s32 extReg, u16 address, WPADCallback* cb
)
{
    wpad_cb_st* p_wpd = __rvl_p_wpadcb[chan];

    BOOL success;
    BOOL intrStatus;
    BOOL handshakeFinished;

    WPADResult status = WPAD_ERR_OK;

    intrStatus = OSDisableInterrupts();

    status = p_wpd->status;
    handshakeFinished = p_wpd->handshakeFinished;

    OSRestoreInterrupts(intrStatus);

    if (status == WPAD_ERR_NO_CONTROLLER)
        goto end;

    if (!handshakeFinished) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto end;
    }

    {
        u32 finalAddress = (address & 0xffff) | (extReg << 16) | (1 << 26);

        success = WPADiSendWriteData(&p_wpd->stdCmdQueue, data, length, finalAddress, cb);
        status = success ? WPAD_ERR_OK : WPAD_ERR_COMMUNICATION_ERROR;
    }

end:
    if (status != WPAD_ERR_OK && cb)
        (*cb)(chan, status);

    return status;
}

WPADResult WPADControlBLC(WPADChannel chan, WPADBLCCommand command, WPADCallback* cb)
{
    wpad_cb_st* p_wpd = __rvl_p_wpadcb[chan];

    ASSERT(WUDIsLinkedWBC());

    BOOL intrStatus = OSDisableInterrupts();

    WPADResult status = p_wpd->status;
    BOOL handshakeFinished = p_wpd->handshakeFinished;

    OSRestoreInterrupts(intrStatus);

    if (status == WPAD_ERR_NO_CONTROLLER)
        goto end;

    if (!handshakeFinished || !WUDIsLinkedWBC()) {
        status = WPAD_ERR_COMMUNICATION_ERROR;
        goto end;
    } else {
        u8 data[7];
        data[0] = data[1] = data[2] = 0xaa;
        data[3] = 0x55;
        data[4] = data[5] = data[6] = command;

        intrStatus = OSDisableInterrupts();

        switch (command) {
        case WPAD_BLC_ENABLE:
            if (WPADiIsAvailableCmdQueue(&p_wpd->stdCmdQueue, 4)) {
                WPADWriteExtReg(chan, data, sizeof data, WPAD_EXT_REG_EXTENSION, 0xf1, NULL);
                WPADWriteExtReg(chan, data, 1, WPAD_EXT_REG_EXTENSION, 0xf1, NULL);
                WPADWriteExtReg(chan, data, 1, WPAD_EXT_REG_EXTENSION, 0xf1, NULL);
                WPADWriteExtReg(chan, data, 1, WPAD_EXT_REG_EXTENSION, 0xf1, cb);

                OSRestoreInterrupts(intrStatus);

                return WPAD_ERR_OK;
            }

            status = WPAD_ERR_COMMUNICATION_ERROR;
            break;

        case WPAD_BLC_CMD_55:
            status = WPADWriteExtReg(chan, data, sizeof data, WPAD_EXT_REG_EXTENSION, 0xf1, cb);

            if (status == WPAD_ERR_OK) {
                OSRestoreInterrupts(intrStatus);
                return WPAD_ERR_OK;
            }

            break;

        case WPAD_BLC_DISABLE:
            status = WPADWriteExtReg(chan, data, 1, WPAD_EXT_REG_EXTENSION, 0xf1, cb);

            if (status == WPAD_ERR_OK) {
                OSRestoreInterrupts(intrStatus);
                return WPAD_ERR_OK;
            }

            break;

        default:
            status = WPAD_ERR_COMMUNICATION_ERROR;
            break;
        }
    }

    OSRestoreInterrupts(intrStatus);

end:
    if (cb)
        (*cb)(chan, status);

    return status;
}

WPADResult WPADGetBLCalibration(WPADChannel chan, void* p_buf, u16 addr, u16 len, WPADCallback cb)
{
    // clang-format off
    ASSERT(( addr >= WPAD_BLCLB_BLK1_ADDR && addr + (len - 1) < WPAD_BLCLB_BLK3_ADDR + WPAD_BLCLB_BLK3_LEN ) || ( addr >= WPAD_BLCLB_BLK4_ADDR && addr + (len - 1) < WPAD_BLCLB_BLK5_ADDR + WPAD_BLCLB_BLK5_LEN ));
    ASSERT(WUDIsLinkedWBC());
    // clang-format on

    if (!(addr >= WPAD_BLCLB_BLK1_ADDR &&
          addr + (len - 1) < WPAD_BLCLB_BLK3_ADDR + WPAD_BLCLB_BLK3_LEN) &&
        !(addr >= WPAD_BLCLB_BLK4_ADDR &&
          addr + (len - 1) < WPAD_BLCLB_BLK5_ADDR + WPAD_BLCLB_BLK5_LEN)) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    if (!WUDIsLinkedWBC()) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    return WPADReadExtReg(chan, p_buf, len, addr, cb);
}

WPADResult WPADGetBLReg(WPADChannel chan, void* p_buf, u32 addr, WPADCallback cb)
{
    // clang-format off
    ASSERT(( addr >= 0x20 && addr < 0x24 ) || ( addr >= 0xf1 && addr < 0xf6 ));
    ASSERT(WUDIsLinkedWBC());
    // clang-format on

    if (!(addr >= 0x20 && addr < 0x24) && !(addr >= 0xf1 && addr < 0xf6)) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    if (!WUDIsLinkedWBC()) {
        return WPAD_ERR_COMMUNICATION_ERROR;
    }

    return WPADReadExtReg(chan, p_buf, 1, addr, cb);
}

// TODO: Proper AFK disconnect. For now just disable the AFK disconnect
cGCT_INSERT_B( //
    0x801BDB24
) BOOL IsControllerDataChanged()
{
    return TRUE;
}

bool WPADiIsWBC(WUDDevInfo* devInfo)
{
    return WUD_DEV_NAME_IS_WBC(devInfo->small.devName);
}

cGCT_IMPORT( //
    0x801396C4
) s32 btm_remove_acl(BD_ADDR_PTR devAddr);

WPADChannel WPADiRetrieveWBCChannel(BD_ADDR_PTR devAddr)
{
    if (__rvl_p_wpadcb[WPAD_CHAN3]->used) {
        btm_remove_acl(devAddr);
        return WPAD_CHAN_INVALID;
    }

    return WPAD_CHAN3;
}

void __parse_bl_data(WPADBLStatus* blStatus, u8* data)
{
#define MAKE_BL_WEIGHT(high_, low_)                                                                \
    ((u16) ((u16) ((high_) << 8) & 0xff00) | (u16) ((low_) & 0x00ff))

    blStatus->press[0] = MAKE_BL_WEIGHT(data[0], data[1]);
    blStatus->press[1] = MAKE_BL_WEIGHT(data[2], data[3]);
    blStatus->press[2] = MAKE_BL_WEIGHT(data[4], data[5]);
    blStatus->press[3] = MAKE_BL_WEIGHT(data[6], data[7]);

    blStatus->temp = data[8];
    blStatus->battery = (u8) data[10];

    blStatus->wpad.err = WPAD_ERR_OK;
#undef MAKE_BL_WEIGHT
}

cGCT_IMPORT(0x801D0288) void* _WUDGetDevAddr(void*);

const char* WBCDeviceName()
{
    return "Nintendo RVL-WBC";
}

GCT_ASM(
    // clang-format off

GCT_INSERT(0x801BF780, setupCallback_KPADConnectWBCCallback)
    cmpwi   r3, 3;
    bne+    L_setupCallback_KPADConnectWBCCallback_NotChan3;

    // Yes, calling a KPAD function from WPAD, i know
    // This is just to simulate a callback
    bl      KPADInitWBC;

    // We know these values, so just restore them
    li      r3, 3; // WPAD_CHAN3
    li      r4, 0; // WPAD_ERR_OK

L_setupCallback_KPADConnectWBCCallback_NotChan3:
    li      r0, 1; // Original instruction
GCT_INSERT_END(setupCallback_KPADConnectWBCCallback);

GCT_INSERT(0x801C01FC, WPADiRetrieveChannel_CheckWBC)
    mr      r31, r3;
    bl      WPADiIsWBC;
    mr      r30, r3;
    lbz     r3, 0x56(r31);
    bl      _WUDGetDevAddr;

    cmpwi   r30, 0;
    beq+    L_WPADiRetrieveChannel_CheckWBC_NotWBC;

    mr      r28, r3;
    bl      WPADiRetrieveWBCChannel;
    mr.     r29, r3;

    lis     r12, PORT(0x801C02D0)@h;
    ori     r12, r12, PORT(0x801C02D0)@l;
    blt+    L_WPADiRetrieveChannel_CheckWBC_Error;

    lis     r12, PORT(0x801C0284)@h;
    ori     r12, r12, PORT(0x801C0284)@l;

L_WPADiRetrieveChannel_CheckWBC_Error:;
    mtctr   r12;
    bctr;

L_WPADiRetrieveChannel_CheckWBC_NotWBC:;
GCT_INSERT_END(WPADiRetrieveChannel_CheckWBC);

GCT_INSERT(0x801C0320, WPADiConnCallback_CallRetrChanWithDevInfo)
    mr      r3, r27;
GCT_INSERT_END(WPADiConnCallback_CallRetrChanWithDevInfo)

// Set r11 to 0
GCT_WRITE_INSTR(0x801C0360, li r11, 0);
GCT_WRITE_INSTR(0x801C0364, stb r11, 0x8C1(r28));

GCT_INSERT(0x801C036C, WPADiConnCallback_SetWBCDataFormat)
    bl      WBCDeviceName;
    mr      r4, r27;
    li      r5, 16;
    bl      memcmp;
    cmpwi   r3, 0;

    li      r0, 0xFB;
    li      r11, 0;
    bne+    L_WPADiConnCallback_SetWBCDataFormat_NotWBC;

    li      r0, 3; // WPAD_DEV_BALANCE_CHECKER
    li      r11, 12; // WPAD_FMT_BALANCE_CHECKER

L_WPADiConnCallback_SetWBCDataFormat_NotWBC:;
GCT_INSERT_END(WPADiConnCallback_SetWBCDataFormat)

// Swap these two instructions around
GCT_WRITE_INSTR(0x801C0378, li r9, 0x64);
GCT_WRITE_INSTR(0x801C0388, li r11, 0);

GCT_INSERT(0x801C46A4, WPADiSendSetReportType_BalanceCheckerFmt)
    cmplwi  r4, 12; // WPAD_FMT_BALANCE_CHECKER
    bne+    L_WPADiSendSetReportType_BalanceCheckerFmt_NotWBC;

    li      r0, 0x34; // RPTID_DATA_BTN_EXT19
    stb     r0, 0x3D(r1);

L_WPADiSendSetReportType_BalanceCheckerFmt_NotWBC:;
    lwz     r31, 0x38(r1); // Original instruction
GCT_INSERT_END(WPADiSendSetReportType_BalanceCheckerFmt)

GCT_INSERT(0x801C5F38, getExtType_AddWBCType)
    bne     L_getExtType_AddWBCType_Out;

    // The actual SDK checks WUDIsLinkedWBC here
    li      r0, 3; // WPAD_DEV_BALANCE_CHECKER

    lis     r3, PORT(0x80386C54)@h;
    ori     r3, r3, PORT(0x80386C54)@l;

    lis     r12, PORT(0x801C5FCC)@h;
    ori     r12, r12, PORT(0x801C5FCC)@l;
    mtctr   r12;
    bctr;

L_getExtType_AddWBCType_Out:;
GCT_INSERT_END(getExtType_AddWBCType);

GCT_WRITE_INSTR(0x801C60CC, cmplwi r0, 2);

GCT_INSERT(0x801C6148, getExtType_InitWBCCertCheck)
    addis   r3, r26, PORT(0x80386C54)@ha;
    lbz     r0, PORT(0x80386C54)@l(r3);
    cmpwi   r0, 3; // WPAD_DEV_BALANCE_CHECKER
    bne+    L_getExtType_InitWBCCertCheck_NotWBC;

    mr      r3, r27;
    li      r4, 0xAA;
    lis     r31, (WM_REG_EXTENSION_CERT_CHALLENGE)@h;
    ori     r5, r31, (WM_REG_EXTENSION_CERT_CHALLENGE)@l;
    addi    r6, r29, PORT(0x801C4D60)@l;
    bl      WPADiSendWriteDataCmd;

    mr      r3, r27;
    li      r4, 0xAA;
    ori     r5, r31, (WM_REG_EXTENSION_CERT_CHALLENGE)@l;
    addi    r6, r29, PORT(0x801C4D60)@l;
    bl      WPADiSendWriteDataCmd;

    mr      r3, r27;
    li      r4, 0xAA;
    ori     r5, r31, (WM_REG_EXTENSION_CERT_CHALLENGE)@l;
    addi    r6, r29, PORT(0x801C4D60)@l;
    bl      WPADiSendWriteDataCmd;

L_getExtType_InitWBCCertCheck_NotWBC:;
    lwz     r7, 0x8A0(r28); // Original instruction
GCT_INSERT_END(getExtType_InitWBCCertCheck);

GCT_INSERT(0x801C5A48, getExConfig_WBCCheck)
    cmplwi  r3, 2; // WPAD_DEV_BALANCE_CHECKER - 1
    bne+    L_getExConfig_WBCCheck_NotWBC;

    lhz     r0, 0x980(r30);
    cmpwi   r0, 1;
    
    lis     r12, PORT(0x801C5EAC)@h;
    ori     r12, r12, PORT(0x801C5EAC)@l;
    beq     L_getExConfig_WBCCheck_Return;

    lis     r12, PORT(0x801C5E88)@h;
    ori     r12, r12, PORT(0x801C5E88)@l;

L_getExConfig_WBCCheck_Return:;
    mtctr   r12;
    bctr;

L_getExConfig_WBCCheck_NotWBC:;
    cmplwi  r3, 1; // Original instruction
GCT_INSERT_END(getExConfig_WBCCheck);

GCT_INSERT(0x801C832C, __a1_34_data_type_parse_bl)
    lbz     r0, 0x8C1(r31); // devType
    cmpwi   r0, 3; // WPAD_DEV_BALANCE_CHECKER
    bne+    L___a1_34_data_type_parse_bl_NotWBC;

    mr      r3, r30;
    addi    r4, r28, 3;
    bl      __parse_bl_data;

L___a1_34_data_type_parse_bl_NotWBC:;
    lwz     r0, 0x840(r31); // Original instruction
GCT_INSERT_END(__a1_34_data_type_parse_bl);

// Change Nintendo RVL-CNT to just RVL- to accept RVL-WBC devices
// sizeof "Nintendo RVL-CNT" = 16
// sizeof "Nintendo RVL-" = 16 - 3

GCT_WRITE_INSTR(0x801CCF98, li r5, 16 - 3);
GCT_WRITE_INSTR(0x801CCFF4, li r5, 16 - 3);
GCT_WRITE_INSTR(0x801CE854, li r5, 16 - 3);
GCT_WRITE_INSTR(0x801CE9A4, li r5, 16 - 3);

// Temporary: Enable WPAD and WUD debug printing
// GCT_WRITE_BRANCH(0x801CB988, 0x801A25D0);
// GCT_WRITE_BRANCH(0x801D09CC, 0x801A25D0);

    // clang-format on
);

} // extern "C"