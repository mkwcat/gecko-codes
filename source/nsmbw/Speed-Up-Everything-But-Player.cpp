// [Gecko]
// $Speed Up Everything But Player [mkwcat]

#define NO_ASM_CALL_THUNK
#include <gct-short-cpp.h>
#include <gct.h>
#include <types.h>

#define List_WalkList 0x80162CF0
#define isBgmAccentSign__6dAudioFUc 0x8006A200

#define SPEED_UP_ON_BAHP 1

GCT_ASM(
    // clang-format off

GCT_INSERT(0x80162FB4, ExecuteThing)
    stw     r30, 0x8(r1);

#if SPEED_UP_ON_BAHP
    li      r3, 1;
    BL_CALL(isBgmAccentSign__6dAudioFUc);
    cmpwi   r3, 0;
    BYTE_PTR(r3, D_SkipPlayerExecuteFlag, 0, -1, 60);
    beq-    L_ExecuteThing_NotBahp;

    lbz     r4, 0x2(r3);
    addi    r4, r4, 2;
    stb     r4, 0x2(r3);

L_ExecuteThing_NotBahp:;
#else
    BYTE_PTR(r3, D_SkipPlayerExecuteFlag, 0, -1, 60);
#endif

    li      r0, 1;
    stb     r0, 0x0(r3); // SkipPlayerExecuteFlag
    bl      CalcExtraTicks;

    mr.     r30, r3;
    beq-    L_ExecuteThing_End;

L_ExecuteThing_Loop:;
    addi    r3, r31, 0x24;
    BL_CALL(List_WalkList);

    subic.  r30, r30, 1;
    bne+    L_ExecuteThing_Loop;

L_ExecuteThing_End:;
    li      r0, 0;
    PTR_STB(r0, r9, D_SkipPlayerExecuteFlag);
    
    lwz     r30, 0x8(r1);

    // Call player executes
    addi    r3, r31, 0x24;
GCT_INSERT_END(ExecuteThing)


GCT_INSERT(0x80162260, SkipPlayerExecute)
    mflr    r0;
    PTR_LBZ(r10, D_SkipPlayerExecuteFlag);
    mtlr    r0;

    lhz     r11, 0x8(r3);

    // Always execute LASTACTORs
    cmpwi   r11, 748;
    bge-    L_SkipPlayerExecute_DoExecute;
    
    cmpwi   r11, 13; // PLAYER
    beq-    L_SkipPlayerExecute_SkippedActor;

    cmpwi   r11, 14; // YOSHI
    beq-    L_SkipPlayerExecute_SkippedActor;

    cmpwi   r10, 1;
    beq-    L_SkipPlayerExecute_DoExecute;
    blr;

L_SkipPlayerExecute_SkippedActor:;
    cmpwi   r10, 0;
    bnelr-;

L_SkipPlayerExecute_DoExecute:;
    stwu    r1, -0x40(r1);
GCT_INSERT_END(SkipPlayerExecute)

    // clang-format on
) // GCT_ASM

extern "C" int CalcExtraTicks(u8* data)
{
    int frame = data[1] = (data[1] + 1) % 60;
    int fps = data[2];

    int extraTicks = fps / 60;
    int remainder = fps % 60;
    if (remainder != 0) {
        extraTicks += (frame % (60 / remainder)) == 0 ? 1 : 0;
    }

    return extraTicks;
}