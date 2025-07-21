// [Gecko]
// $Balance Board Support [mkwcat]
// *This is the (unfinished) Balance Board code for Mario Kart Wii.
// *It's only been tested on emulator and currently only supports the PAL version of the game.
// *
// *Upon connecting the Balance Board, it will immediately start calibration. Please wait like 10
// *seconds to make sure it's completed before stepping on the Balance Board.

#include <gct-use-cpp.h>
#include <gct.h>

#include "Balance-Board-Support/WBCSupport.cpp"
#include "mkw/Balance-Board-Support/KPAD.h"
#include "revolution/kpad.h"
#include <math.h>

GCT_ASM(
    // clang-format off

GCT_INSERT(0x8051FDD8, WiiController_calcInner_handleWBC)
    cmplwi  r3, 3; // WPAD_DEV_BALANCE_CHECKER
    bne+    L_WiiController_calcInner_handleWBC_NotWBC;

    li      r25, 0;
    li      r24, 0;

    mr      r3, r26;
    mr      r4, r27;
    mr      r5, r28;
    bl      WiiController_calcBalance;

    stw     r25, 0x8F8(r26); // r25 must be 0 here
    lis     r12, PORT(0x8051FE88)@h;
    ori     r12, r12, PORT(0x8051FE88)@l;
    mtctr   r12;
    bctr;

L_WiiController_calcInner_handleWBC_NotWBC:;
    cmplwi  r3, 2; // Original instruction
GCT_INSERT_END(WiiController_calcInner_handleWBC)

    // clang-format on
); // GCT_ASM

extern f32 STICK_SCALE asm("PORT(0x809C2C3C)");

cGCT_IMPORT_DECL(0x80745DE4);
cGCT_IMPORT_DECL(0x8051E960);
cGCT_IMPORT_DECL(0x8051EA60);
cGCT_IMPORT_DECL(0x8051EB68);

class RaceInputState
{
public:
    cGCT_IMPORT_ATTR( //
        0x8051E960
    ) void setX(u8 x);

    cGCT_IMPORT_ATTR( //
        0x8051EA60
    ) void setY(u8 y);

    /**
     * Set trick/wheelie input
     */
    cGCT_IMPORT_ATTR( //
        0x8051EB68
    ) void setActionDir(u8 direction);
};

class UIInputState
{
public:
    enum Btn {
        FRONT = 0x1,
        BACK = 0x2,
        OPTION = 0x4,
        UP = 0x8,
        DOWN = 0x10,
        LEFT = 0x20,
        RIGHT = 0x40,
        HOME = 0x80,
        EXTRA = 0x100,
    };

    FILL(0x00, 0x04);

    /* 0x04 */ u16 m_btn;
    /* 0x06 */ u16 m_rawBtn;
    /* 0x08 */ Vec2 m_stick;
    /* 0x10 */ f32 m_0x10;
    /* 0x14 */ f32 m_0x14;
    /* 0x18 */ u8 m_discreteStickX;
    /* 0x19 */ u8 m_discreteStickY;
    /* 0x1C */ Vec2 m_pointer;
    /* 0x24 */ Vec2 m_pointerHorizon;
    /* 0x2C */ f32 m_pointerDist;
    /* 0x30 */ u8 m_0x30;
};

class WiiControllerData
{
public:
    cGCT_IMPORT_ATTR( //
        0x80745DE4
    ) void calcCore(RaceInputState* raceInput, KPADStatus* kpadStatus);
};

class WiiController
{
public:
    void
    calcBalance(RaceInputState* raceInput, UIInputState* uiInput) asm("WiiController_calcBalance");

public:
    FILL(0x000, 0x91C);

    WiiControllerData* mpData;
};

static u32 s_prevHold = 0;
static u16 s_prevUiBtn = 0;
static u16 s_prevUiRawBtn = 0;

enum {
    RIGHT_FRONT = 0,
    RIGHT_REAR = 1,
    LEFT_FRONT = 2,
    LEFT_REAR = 3,
};

f64 fmax(f64 v1, f64 v2)
{
    return v1 > v2 ? v1 : v2;
}

void WiiController::calcBalance(RaceInputState* raceInput, UIInputState* uiInput)
{
    f64 weights[4] = {
        fmax(g_blStatus.weight[0], 0.0),
        fmax(g_blStatus.weight[1], 0.0),
        fmax(g_blStatus.weight[2], 0.0),
        fmax(g_blStatus.weight[3], 0.0),
    };

    const f64 weight = weights[0] + weights[1] + weights[2] + weights[3];
    const f64 leftWeight = weights[LEFT_FRONT] + weights[LEFT_REAR];
    const f64 rightWeight = weights[RIGHT_FRONT] + weights[RIGHT_REAR];

    KPADStatus kpadStatus = {};
    kpadStatus.dev_type = WPAD_DEV_CORE;

    f64 leftRight = 0;
    f64 upDown = 0;
    if (weight > 20.0) {
        kpadStatus.hold |= WPADButton::WPAD_BUTTON_2;

        // Get direction
        leftRight = (rightWeight - leftWeight) / weight;

        // Calc if leaning forawrd
        upDown = (weights[LEFT_FRONT] + weights[RIGHT_FRONT] - weights[LEFT_REAR] -
                  weights[RIGHT_REAR]) /
                 weight;

        // Because the heel tends to have more weight by default
        upDown += 0.2;
    }

    f64 fx = (leftRight * 1.7) * STICK_SCALE;
    if (fx < -7.0) {
        fx = -7.0;
    } else if (fx > 7.0) {
        fx = 7.0;
    }

    u8 x = (u8) (fx + 7);

    f64 fy = (upDown * 0.7) * STICK_SCALE;
    if (fy < -7.0) {
        fy = -7.0;
    } else if (fy > 7.0) {
        fy = 7.0;
    }

    u8 y = (u8) (fy + 7);

    if ((fy + 7) <= 3) {
        kpadStatus.hold |= WPADButton::WPAD_BUTTON_LEFT;
    } else if ((fy + 7) >= 11) {
        kpadStatus.hold |= WPADButton::WPAD_BUTTON_RIGHT;
    }

    kpadStatus.trig = kpadStatus.hold & ~s_prevHold;
    kpadStatus.release = ~kpadStatus.hold & s_prevHold;
    s_prevHold = kpadStatus.hold;

    mpData->calcCore(raceInput, &kpadStatus);

    // OSReport("X: %d\n", x);
    raceInput->setX(x);

    // OSReport("Y: %d\n", y);
    raceInput->setY(y);

    int trick = 0;
    if (weight > 95.0) {
        trick = 1;
    }
    raceInput->setActionDir(trick);

    // To UI input
    u16 uiBtn = 0;
    u16 rawBtn = 0;

    if (weight > 9.0) {
        // Get leaning direction (up down left right center)

        f64 upDownLeft = g_blStatus.weight[LEFT_FRONT] - g_blStatus.weight[LEFT_REAR];
        f64 upDownRight = g_blStatus.weight[RIGHT_FRONT] - g_blStatus.weight[RIGHT_REAR];

        f64 upDown = upDownLeft + upDownRight;

        f64 leftRightFront = g_blStatus.weight[LEFT_FRONT] - g_blStatus.weight[RIGHT_FRONT];
        f64 leftRightRear = g_blStatus.weight[LEFT_REAR] - g_blStatus.weight[RIGHT_REAR];

        f64 leftRight = leftRightFront + leftRightRear;

        if (upDown > 6.0) {
            uiBtn |= UIInputState::Btn::UP;
            rawBtn |= 0x2;
        } else if (upDown < -6.0) {
            uiBtn |= UIInputState::Btn::DOWN;
            rawBtn |= 0x1;
        }

        if (leftRight > 6.0) {
            uiBtn |= UIInputState::Btn::LEFT;
            rawBtn |= 0x4;
        } else if (leftRight < -6.0) {
            uiBtn |= UIInputState::Btn::RIGHT;
            rawBtn |= 0x8;
        }

        if (rawBtn == 0) {
            uiBtn |= UIInputState::Btn::FRONT;
            rawBtn |= 0x900;
        }
    }

    uiInput->m_btn = uiBtn & ~s_prevUiBtn;
    uiInput->m_rawBtn = rawBtn & ~s_prevUiBtn;

    s_prevUiBtn = uiBtn;
    s_prevUiRawBtn = rawBtn;
}