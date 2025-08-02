// [Gecko]
// $GameCube Controller Support [mkwcat]
// *Adds support for GameCube controllers, replacing the inputs of the Wii
// *Remote in the slot it's plugged into. Currently buggy and finished. Notably,
// *a Wii Remote must be connected in the slot the GameCube controller is
// *plugged into for the controller to be registered, and rumble and sounds are
// *still directed to the source Wii Remote.
// *
// *This code is long, and won't load using a standard gecko codehandler (such
// *as the one in Dolphin). To resolve this, I've created a gecko code to load
// *more gecko codes (see [Load-More-Gecko-Codes](Load-More-Gecko-Codes.md)).

#define NO_ASM_CALL_THUNK
#include <gct-use-cpp.h>
#include <gct.h>

#include "GameCube-Controller-Support/PAD.cpp"
#include "GameCube-Controller-Support/SI.cpp"
#include <d_system/d_a_player_manager.h>
#include <revolution/kpad.h>

GCT_ASM(
    // clang-format off

// Hook in mPad::beginPad
GCT_INSERT(0x8016F398, ReadPadsHook)
    bctrl;
    bl      ReadPads;
GCT_INSERT_END(ReadPadsHook)

// Hook in mPad::beginPad loop
GCT_INSERT(0x8016F3CC, AdjustPadsHook)
    stw     r3, 0x0(r28); // Original instruction
    mr      r4, r25;
    bl      AdjustPads;
GCT_INSERT_END(AdjustPadsHook)

// Get battery level patch
// mPad::getBatteryLevel_ch
GCT_WRITE_INSTR(0x8016F5C8, li r3, 4)

// Hook in dGameKeyCore::setShakeY
GCT_INSERT(0x800B62D0, SetShakeYHook)
    // Silly hacks to save the registers
    mflr    r4;
    bl      SetShakeY;
    mtlr    r4;
    cmpwi   cr7, r3, 0;
    BC_CALL(cr7, eq, 0x800B63E0);

    lbz     r4, 0x92(r3); // Original instruction
GCT_INSERT_END(SetShakeYHook)

    // clang-format on
) // GCT_ASM

static PADStatus s_pads[4];
static u16 s_lastButtons[4];
static bool s_lastShake[4];

extern "C" void ReadPads()
{
    // PADInit checks if it's already initialized so this only runs once in
    // practice
    PADInit();

    PADRead(&s_pads[0]);
    u32 resetMask = 0x00000000;

    for (int i = 0; i < 4; i++) {
        PADStatus* pad = &s_pads[i];

        if (pad->err == -1) {
            resetMask |= 0x80000000 >> i;
        }

        if (pad->err != 0) {
            s_lastButtons[i] = 0;
            continue;
        }
    }

    PADReset(resetMask);
}

extern "C" void* AdjustPads(void* wpad, s32 index)
{
    PADStatus* pad = &s_pads[index];

    if (pad->err != 0) {
        return wpad;
    }

    // Set connected flag
    Write32O(wpad, 0xB18, 1);
    Set8O(wpad, 0xB1C, 1);

    KPADStatus* kpad = (KPADStatus*) (u32(wpad) + 0x18);
    std::memset(kpad, 0, 0xB0);

    u16 from = pad->button;
    u16 to = 0;

    // Convert to sideways Wii Remote buttons
    if (from & PAD_BUTTON_LEFT) {
        to |= WPAD_BUTTON_UP;
    }
    if (from & PAD_BUTTON_RIGHT) {
        to |= WPAD_BUTTON_DOWN;
    }
    if (from & PAD_BUTTON_DOWN) {
        to |= WPAD_BUTTON_LEFT;
    }
    if (from & PAD_BUTTON_UP) {
        to |= WPAD_BUTTON_RIGHT;
    }

    if (from & PAD_BUTTON_A) {
        to |= WPAD_BUTTON_2;
    }
    if (from & PAD_BUTTON_B) {
        to |= WPAD_BUTTON_1;
    }
    if (from & PAD_BUTTON_Y) {
        to |= WPAD_BUTTON_MINUS;
    }
    if (from & PAD_TRIGGER_Z) {
        to |= WPAD_BUTTON_A;
    }
    if (from & PAD_BUTTON_START) {
        to |= WPAD_BUTTON_PLUS;
    }

    // Map stick to D-pad
    if (pad->stickX < -32 && !(to & WPAD_BUTTON_DOWN)) {
        to |= WPAD_BUTTON_UP;
    }
    if (pad->stickX > 32 && !(to & WPAD_BUTTON_UP)) {
        to |= WPAD_BUTTON_DOWN;
    }
    if (pad->stickY < -32 && !(to & WPAD_BUTTON_RIGHT)) {
        to |= WPAD_BUTTON_LEFT;
    }
    if (pad->stickY > 32 && !(to & WPAD_BUTTON_LEFT)) {
        to |= WPAD_BUTTON_RIGHT;
    }

    kpad->hold = to & s_lastButtons[index];
    kpad->trig = to & ~s_lastButtons[index];
    kpad->release = ~to & s_lastButtons[index];
    kpad->wpad_err = 0;

    if (from & PAD_BUTTON_Y) {
        kpad->hold |= WPAD_BUTTON_1;
    }

    s_lastButtons[index] = to;

    float analogA = pad->triggerL;
    float analogB = pad->triggerR;

    float tilt = analogA / 255;
    tilt -= analogB / 255;

    kpad->acc.x = 0;
    kpad->acc.y = 0;
    kpad->acc.z = tilt;

    kpad->acc_vertical.y = -tilt;

    kpad->dev_type = WPADDeviceType::WPAD_DEV_CORE;

    return wpad;
}

extern "C" u64 SetShakeY(u64 param)
{
    void* key = (void*) (param >> 32);

    u32 index = Read32O(key, 0x4);
    PADStatus* pad = &s_pads[index];

    if (pad->err != 0) {
        return param;
    }

    // Set shake flag
    // 0x8C = shake flag
    // 0x92 = shake cooldown

    Write8O(key, 0x8C, 0); // Clear it temporarily

    bool fullShake = pad->triggerL == 255 || pad->triggerR == 255 || pad->button & PAD_TRIGGER_L ||
                     pad->button & PAD_TRIGGER_R || pad->button & PAD_BUTTON_X;

    bool shake = fullShake || pad->triggerL > 170 || pad->triggerR > 170;

    bool lastShake = s_lastShake[index];
    s_lastShake[index] = shake;

    bool shakeResult = false;
    if (Read8O(key, 0x92) == 0) {
        if (fullShake || (shake && !lastShake)) {
            shakeResult = true;

            if (!fullShake) {
                dAcPy_c* player = daPyMng_c::getPlayer(index);

                // Check if on the ground
                shakeResult = player != nullptr && !(player->m_0x10D4 & 1);
            }
        }
    } else {
        Write8O(key, 0x92, Read8O(key, 0x92) - 1);
    }

    if (shakeResult) {
        // Set cooldown
        Write8O(key, 0x92, 5 + 3);
    }
    // Set shake
    Write8O(key, 0x8C, shakeResult);

    return param & 0xFFFFFFFF;
}