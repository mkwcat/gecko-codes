// [Gecko]
// $Random Worldmap Animations [mkwcat]
// *This is a silly code that doesn't make much sense to release,
// *but it's part of the repo anyway.

#include <gct.h>

#define getRandom__8dGameComFUl 0x800B2EE0
#define WMScriptsStart 0x802F3778
#define WMScriptsEnd 0x802F4CD0
#define WMCmdCount ((0x802F4CD0 - 0x802F3778) >> 3)

GCT_ASM(
// clang-format off

GCT_INSERT(0x80101814, ChangeCmd)
    stw     r31, 0x8(r1);
    stw     r30, 0xC(r1);

    lis     r31, PORT(WMScriptsStart - 8)@h;
    ori     r31, r31, PORT(WMScriptsStart - 8)@l;
    li      r30, WMCmdCount;
    
L_ChangeCmd_Loop:;
    lwzu    r4, 8(r31);
    // 11, 57, 70, 107, 108, 156
    cmpwi   r4, 11;
    beq     L_ChangeCmd_Rand;
    cmpwi   r4, 57;
    beq     L_ChangeCmd_Rand;
    cmpwi   r4, 70;
    beq     L_ChangeCmd_Rand;
    cmpwi   r4, 107;
    beq     L_ChangeCmd_Rand;
    cmpwi   r4, 108;
    beq     L_ChangeCmd_Rand;
    cmpwi   r4, 156;
    bne     L_ChangeCmd_LoopCond;

L_ChangeCmd_Rand:;
    // 11, 57, 70, 107, 108, 156
    li      r3, 7 + 1 + 1 + 1 + 1 + 1;
    lis     r12, PORT(getRandom__8dGameComFUl)@h;
    ori     r12, r12, PORT(getRandom__8dGameComFUl)@l;
    mtctr   r12;
    bctrl;
    bl      L_ChangeCmd_Code;
    .byte   11, 11, 11, 11, 11, 11, 11, 57, 70, 107, 108, 156;
    .balign 0x4;
L_ChangeCmd_Code:;
    mflr    r4;
    lbzx    r4, r3, r4;
    stw     r4, 0(r31);

    li      r0, 0;
    cmpwi   r4, 11;
    bne     L_ChangeCmd_NotCmd11;

    mr      r0, r3;

L_ChangeCmd_NotCmd11:;
    stw     r0, 0x4(r31);

L_ChangeCmd_LoopCond:;
    subic.  r30, r30, 1;
    bne+    L_ChangeCmd_Loop;

L_ChangeCmd_End:;
    lwz     r30, 0xC(r1);
    lwz     r31, 0x8(r1);
    lwz     r0, 0x14(r1);
GCT_INSERT_END(ChangeCmd)

// clang-format on
) // GCT_ASM