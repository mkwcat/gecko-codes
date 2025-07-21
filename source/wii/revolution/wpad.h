// From
// https://github.com/muff1n1634/wpad_11dec2009/blob/3c3aa5416d15ccc5867517d9ed4aaf6d25a70e9c/include/revolution/WPAD/WPAD.h

#pragma once

#include <revolution/types.h>

extern "C" {

// ---------
// Constants
// ---------

#define WPAD_MAX_DPD_OBJECTS 4

#define WPAD_MIN_DPD_SENS 1
#define WPAD_MAX_DPD_SENS 5

#define WPAD_BLCLB_BLK1_ADDR 0x24
#define WPAD_BLCLB_BLK1_LEN 0x10
#define WPAD_BLCLB_BLK2_ADDR 0x34
#define WPAD_BLCLB_BLK2_LEN 0x8
#define WPAD_BLCLB_BLK3_ADDR 0x3C
#define WPAD_BLCLB_BLK3_LEN 0x4
#define WPAD_BLCLB_BLK4_ADDR 0x50
#define WPAD_BLCLB_BLK4_LEN 0x10 // Guess
#define WPAD_BLCLB_BLK5_ADDR 0x60
#define WPAD_BLCLB_BLK5_LEN 0x10


// ------------
// Enumerations
// ------------

enum WUDChannel {
    WUD_CHAN0 = 0,
    WUD_CHAN1 = 1,
    WUD_CHAN2 = 2,
    WUD_CHAN3 = 3,

    WUD_MAX_CHANNELS = 4,
    WUD_CHAN_INVALID = -1
};

enum WPADChannel {
    WPAD_CHAN0 = WUDChannel::WUD_CHAN0,
    WPAD_CHAN1 = WUDChannel::WUD_CHAN1,
    WPAD_CHAN2 = WUDChannel::WUD_CHAN2,
    WPAD_CHAN3 = WUDChannel::WUD_CHAN3,

    WPAD_MAX_CHANNELS = WUDChannel::WUD_MAX_CHANNELS,
    WPAD_CHAN_INVALID = WUDChannel::WUD_CHAN_INVALID,
};

enum WPADResult {
    WPAD_ERR_OK = 0,
    WPAD_ERR_NO_CONTROLLER = -1,
    WPAD_ERR_COMMUNICATION_ERROR = -2,
    WPAD_ERR_3 = -3,
    WPAD_ERR_INVALID = -4,
    WPAD_ERR_CORRUPTED = -7,
};

enum WPADDeviceType : u8 {
    WPAD_DEV_CORE = 0,
    WPAD_DEV_FS = 1,
    WPAD_DEV_CLASSIC = 2,
    WPAD_DEV_BALANCE_CHECKER = 3,
    WPAD_DEV_VSM = 4,

    WPAD_DEV_MOTION_PLUS = 5,
    WPAD_DEV_MPLS_PT_FS = 6,
    WPAD_DEV_MPLS_PT_CLASSIC = 7,

    WPAD_DEV_TRAIN = 16,
    WPAD_DEV_GUITAR = 17,
    WPAD_DEV_DRUM = 18,
    WPAD_DEV_TAIKO = 19,
    WPAD_DEV_TURNTABLE = 20,

    WPAD_DEV_BULK_1 = 21,
    WPAD_DEV_BULK_2 = 22,
    WPAD_DEV_BULK_3 = 23,
    WPAD_DEV_BULK_4 = 24,
    WPAD_DEV_BULK_5 = 25,
    WPAD_DEV_BULK_6 = 26,
    WPAD_DEV_BULK_7 = 27,
    WPAD_DEV_BULK_8 = 28,

    WPAD_DEV_MPLS_PT_UNKNOWN = 250,

    WPAD_DEV_251 = 251,
    WPAD_DEV_252 = 252,
    WPAD_DEV_NONE = 253,
    WPAD_DEV_INITIALIZING = 255,
};

enum WPADDataFormat {
    WPAD_FMT_CORE_BTN = 0,
    WPAD_FMT_CORE_BTN_ACC = 1,
    WPAD_FMT_CORE_BTN_ACC_DPD = 2,

    WPAD_FMT_FS_BTN = 3,
    WPAD_FMT_FS_BTN_ACC = 4,
    WPAD_FMT_FS_BTN_ACC_DPD = 5,

    WPAD_FMT_CLASSIC_BTN = 6,
    WPAD_FMT_CLASSIC_BTN_ACC = 7,
    WPAD_FMT_CLASSIC_BTN_ACC_DPD = 8,

    WPAD_FMT_BTN_ACC_DPD_EXTENDED = 9,

    // Extensions

    WPAD_FMT_TRAIN = 10,
    WPAD_FMT_GUITAR = 11,
    WPAD_FMT_BALANCE_CHECKER = 12,
    WPAD_FMT_VSM = 13,
    WPAD_FMT_DRUM = 15,
    WPAD_FMT_MOTION_PLUS = 16,
    WPAD_FMT_TAIKO = 17,
    WPAD_FMT_TURNTABLE = 18,
    WPAD_FMT_BULK = 19,
};

enum WPADDeviceMode {
    WPAD_DEV_MODE_NORMAL = 0,

    WPAD_DEV_MODE_CLASSIC_REDUCED = 1,
    WPAD_DEV_MODE_CLASSIC_EXTENDED = 2,
    WPAD_DEV_MODE_CLASSIC_STANDARD = 3,

    WPAD_DEV_MODE_MPLS_PT_MAIN = 4,
    WPAD_DEV_MODE_MPLS_PT_FS = 5,
    WPAD_DEV_MODE_MPLS_PT_CLASSIC = 7,
};

/**
 * Wii Remote (+ Nunchuk) button bitfield.
 */

enum WPADButton : u16 {
    WPAD_BUTTON_LEFT = (1 << 0),
    WPAD_BUTTON_RIGHT = (1 << 1),
    WPAD_BUTTON_DOWN = (1 << 2),
    WPAD_BUTTON_UP = (1 << 3),
    WPAD_BUTTON_PLUS = (1 << 4),

    WPAD_BUTTON_2 = (1 << 8),
    WPAD_BUTTON_1 = (1 << 9),
    WPAD_BUTTON_B = (1 << 10),
    WPAD_BUTTON_A = (1 << 11),
    WPAD_BUTTON_MINUS = (1 << 12),
    WPAD_BUTTON_FS_Z = (1 << 13),
    WPAD_BUTTON_FS_C = (1 << 14),
    WPAD_BUTTON_HOME = (1 << 15),

    WPAD_BUTTON_ALL = 0xFF1F,
};

/**
 * Classic Controller button bitfield.
 */
enum WPADClassicButton : u16 {
    WPAD_BUTTON_CL_UP = (1 << 0),
    WPAD_BUTTON_CL_LEFT = (1 << 1),
    WPAD_BUTTON_CL_ZR = (1 << 2),
    WPAD_BUTTON_CL_X = (1 << 3),
    WPAD_BUTTON_CL_A = (1 << 4),
    WPAD_BUTTON_CL_Y = (1 << 5),
    WPAD_BUTTON_CL_B = (1 << 6),
    WPAD_BUTTON_CL_ZL = (1 << 7),
    WPAD_BUTTON_CL_FULL_R = (1 << 9),
    WPAD_BUTTON_CL_PLUS = (1 << 10),
    WPAD_BUTTON_CL_HOME = (1 << 11),
    WPAD_BUTTON_CL_MINUS = (1 << 12),
    WPAD_BUTTON_CL_FULL_L = (1 << 13),
    WPAD_BUTTON_CL_DOWN = (1 << 14),
    WPAD_BUTTON_CL_RIGHT = (1 << 15),

    WPAD_BUTTON_CL_ALL = 0xFEFF,
};

enum WPADMotorCommand {
    WPAD_MOTOR_STOP = 0,
    WPAD_MOTOR_RUMBLE = 1,
};

enum WPADSpeakerCommand {
    WPAD_SPEAKER_DISABLE = 0,
    WPAD_SPEAKER_ENABLE = 1,
    WPAD_SPEAKER_MUTE = 2,
    WPAD_SPEAKER_UNMUTE = 3,
    WPAD_SPEAKER_PLAY = 4,
    WPAD_SPEAKER_CMD_05 = 5,
};

enum WPADDpdCommand {
    WPAD_DPD_DISABLE = 0x00,
    WPAD_DPD_BASIC = 0x01,
    WPAD_DPD_STANDARD = 0x03,
    WPAD_DPD_EXTENDED = 0x05,
};

enum WPADBLCCommand : u8 {
    WPAD_BLC_DISABLE = 0x00,
    WPAD_BLC_CMD_55 = 0x55,
    WPAD_BLC_ENABLE = 0xAA,
};

enum WPADMPStatusFlags : u8 {
    WPAD_MPLS_STATUS_DATA_VALID = (1 << 7),
    WPAD_MPLS_STATUS_EXTENSION_DATA_VALID = (1 << 6),

    WPAD_MPLS_STATUS_YAW_SLOW_MODE = (1 << 3),
    WPAD_MPLS_STATUS_PITCH_SLOW_MODE = (1 << 2),
    WPAD_MPLS_STATUS_ROLL_SLOW_MODE = (1 << 1),

    // Just that it's present; not that it's actually being used.
    WPAD_MPLS_STATUS_EXTENSION_CONNECTED = (1 << 0),
};

enum WPADAccGravityUnitType {
    WPAD_ACC_GRAVITY_UNIT_CORE,
    WPAD_ACC_GRAVITY_UNIT_FS,
};

// ----------------
// Type Definitions
// ----------------

typedef void* WUDAllocFunc(size_t size);
typedef int WUDFreeFunc(void* ptr);

typedef void WUDSyncDeviceCallback(s32 result, s32 num);
typedef void WUDClearDeviceCallback(signed);

typedef WUDAllocFunc WPADAllocFunc;
typedef WUDFreeFunc WPADFreeFunc;

typedef void WPADInitFunc(void); // see __wpadInitSub in WPAD.c

typedef void WPADCallback(WPADChannel chan, WPADResult result);

typedef void WPADSamplingCallback(WPADChannel chan);
typedef void WPADConnectCallback(WPADChannel chan, s32 result);
typedef void WPADExtensionCallback(WPADChannel chan, WPADDeviceType devType);

typedef WUDSyncDeviceCallback WPADSimpleSyncCallback;

// ----------
// Structures
// ----------

struct DPDObject {
    SIZE_ASSERT(0x8);

    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ u16 size;
    /* 0x6 */ u8 traceId;
};

struct DPDObjEx {
    SIZE_ASSERT(0xC);

    /* 0x0 */ s16 range_x1;
    /* 0x2 */ s16 range_y1;
    /* 0x4 */ s16 range_x2;
    /* 0x6 */ s16 range_y2;
    /* 0x8 */ u16 pixel;
    /* 0xA */ s8 radius;
};

struct WPADInfo {
    SIZE_ASSERT(0x18);

    /* 0x00 */ BOOL dpd;
    /* 0x04 */ BOOL speaker;
    /* 0x08 */ BOOL attach;
    /* 0x0C */ BOOL lowBat;
    /* 0x10 */ BOOL nearempty;
    /* 0x14 */ u8 battery;
    /* 0x15 */ u8 led;
    /* 0x16 */ u8 protocol;
    /* 0x17 */ u8 firmware;
};

struct WPADStatus {
    SIZE_ASSERT(0x2A);

    /* 0x00 */ WPADButton button;
    /* 0x02 */ s16 accX;
    /* 0x04 */ s16 accY;
    /* 0x06 */ s16 accZ;
    /* 0x08 */ DPDObject obj[WPAD_MAX_DPD_OBJECTS];
    /* 0x28 */ u8 dev;
    /* 0x29 */ s8 err;
};

struct WPADStatusEx {
    SIZE_ASSERT(0x5A);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ DPDObjEx exp[WPAD_MAX_DPD_OBJECTS];
};

struct WPADFSStatus {
    SIZE_ASSERT(0x32);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ s16 fsAccX;
    /* 0x2C */ s16 fsAccY;
    /* 0x2E */ s16 fsAccZ;
    /* 0x30 */ s8 fsStickX;
    /* 0x31 */ s8 fsStickY;
};

struct WPADCLStatus {
    SIZE_ASSERT(0x36);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ WPADClassicButton clButton;
    /* 0x2C */ s16 clLStickX;
    /* 0x2E */ s16 clLStickY;
    /* 0x30 */ s16 clRStickX;
    /* 0x32 */ s16 clRStickY;
    /* 0x34 */ u8 clTriggerL;
    /* 0x35 */ u8 clTriggerR;
};

struct WPADTRStatus {
    SIZE_ASSERT(0x2E);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ WPADClassicButton trButton;
    /* 0x2C */ u8 brake;
    /* 0x2D */ u8 mascon;
};

struct WPADBLStatus {
    SIZE_ASSERT(0x34);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ u16 press[4];
    /* 0x32 */ s8 temp;
    /* 0x33 */ u8 battery;
};

struct WPADMPStatus {
    SIZE_ASSERT(0x3E);

    /* 0x00 */ WPADStatus wpad;

    union {
        // Takes its members from WPADFSStatus
        struct {
            /* 0x2A */ s16 fsAccX;
            /* 0x2C */ s16 fsAccY;
            /* 0x2E */ s16 fsAccZ;
            /* 0x30 */ s8 fsStickX;
            /* 0x31 */ s8 fsStickY;
        } fs;

        // Takes its members from WPADCLStatus
        struct {
            /* 0x2A */ WPADClassicButton clButton;
            /* 0x2C */ s16 clLStickX;
            /* 0x2E */ s16 clLStickY;
            /* 0x30 */ s16 clRStickX;
            /* 0x32 */ s16 clRStickY;
            /* 0x34 */ u8 clTriggerL;
            /* 0x35 */ u8 clTriggerR;
        } cl;
    } ext;

    /* 0x36 */ WPADMPStatusFlags stat;
    /* 0x37 */ u8 reserved;

    /* 0x38 */ s16 pitch;
    /* 0x3A */ s16 yaw;
    /* 0x3C */ s16 roll;
};

struct WPADBKStatus {
    SIZE_ASSERT(0x40);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ u8 bulk[21];
    /* 0x3F */ u8 padding[1];
};

struct WPADVSStatus {
    SIZE_ASSERT(0x46);

    /* 0x00 */ WPADStatus wpad;

    /* 0x2A */ u16 m0x2A[5];
    /* 0x34 */ u8 m0x34;

    /* 0x36 */ u16 m0x36[5];
    /* 0x40 */ u8 m0x40;

    /* 0x42 */ u16 m0x42;
    /* 0x44 */ u8 m0x44;
};

struct WPADAccGravityUnit {
    SIZE_ASSERT(0x6);

    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
};

// ---------
// Functions
// ---------

BOOL WPADIsEnabledVSM(void);
BOOL WPADIsEnabledTRN(void);
BOOL WPADIsEnabledGTR(void);
BOOL WPADIsEnabledDRM(void);
BOOL WPADIsEnabledTKO(void);
BOOL WPADIsEnabledTBL(void);
BOOL WPADIsEnabledBLK(void);

void WPADInit(void);
void WPADShutdown(void);

BOOL WPADStartFastSimpleSync(void);
BOOL WPADStopSimpleSync(void);
BOOL WPADCancelSyncDevice(void);
WPADSimpleSyncCallback* WPADSetSimpleSyncCallback(WPADSimpleSyncCallback* cb);
void WPADRegisterAllocator(WPADAllocFunc* alloc, WPADFreeFunc* free);
// WPADLibStatus WPADGetStatus(void);
u8 WPADGetRadioSensitivity(WPADChannel chan);
// void WPADGetAddress(WPADChannel chan, BD_ADDR_PTR addr);
u8 WPADGetSensorBarPosition(void);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801DFD10)
) BOOL WPADSetAcceptConnection(BOOL accept);

void WPADGetAccGravityUnit(WPADChannel chan, WPADAccGravityUnitType type, WPADAccGravityUnit* acc);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801E0F80)
) void WPADDisconnect(WPADChannel chan);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801E1080)
) WPADResult WPADProbe(WPADChannel chan, WPADDeviceType* devTypeOut);

WPADSamplingCallback* WPADSetSamplingCallback(WPADChannel chan, WPADSamplingCallback* cb);
WPADConnectCallback* WPADSetConnectCallback(WPADChannel chan, WPADConnectCallback* cb);
WPADExtensionCallback* WPADSetExtensionCallback(WPADChannel chan, WPADExtensionCallback* cb);
WPADDataFormat WPADGetDataFormat(WPADChannel chan);
WPADResult WPADSetDataFormat(WPADChannel chan, WPADDataFormat fmt);

WPADResult WPADGetInfoAsync(WPADChannel chan, WPADInfo* info, WPADCallback* cb);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801E1620)
) void WPADControlMotor(WPADChannel chan, WPADMotorCommand command);

void WPADEnableMotor(BOOL enabled);
BOOL WPADIsMotorEnabled(void);
WPADResult WPADControlLed(WPADChannel chan, u8 ledFlags, WPADCallback cb);
// BOOL WPADSaveConfig(SCFlushCallback* cb);

/**
 * @param status should match the WPADStatus type for the channel; a check
 * against the currently assigned device type is made to know how much to copy
 *
 * for example, if devType[chan] == WPAD_DEV_CLASSIC, status should be a
 * pointer to WPADCLStatus cast to a pointer to WPADStatus
 */
void WPADRead(WPADChannel chan, WPADStatus* status);
void WPADSetAutoSamplingBuf(WPADChannel chan, void* buf, u32 length);

BOOL WPADIsSpeakerEnabled(WPADChannel chan);
WPADResult WPADControlSpeaker(WPADChannel chan, WPADSpeakerCommand command, WPADCallback* cb);
u8 WPADGetSpeakerVolume(void);
void WPADSetSpeakerVolume(u8 vol);

BOOL WPADCanSendStreamData(WPADChannel chan);
WPADResult WPADSendStreamData(WPADChannel chan, void* p_buf, u16 len);
u8 WPADGetDpdSensitivity(void);
BOOL WPADIsDpdEnabled(WPADChannel chan);

WPADResult WPADControlDpd(WPADChannel chan, WPADDpdCommand command, WPADCallback* cb);
WPADResult WPADControlBLC(WPADChannel chan, WPADBLCCommand command, WPADCallback* cb);

void WPADRecalibrate(WPADChannel chan);
BOOL WPADIsUsedCallbackByKPAD(void);
void WPADSetCallbackByKPAD(BOOL isKPAD);

} // extern "C"