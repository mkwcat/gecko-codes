// [Gecko]
// $Arbitrary Course Loader
// *Load a course arbitrarily using an external tool.

#define NO_ASM_CALL_THUNK
#include <gct.h>

#define WMThing_LoadLevel 0x0223B820
#define CSSeqMgr 0x101D9044
#define CSSeqMgr_addToQueue 0x021E48E0

#define StartGameInfo 0x101DED6C
#define FadeMgr 0x101F577C
#define FadeMgr_FadeToLevelForSecret 0x029B3B18
#define FadeMgr_FadeToLevel 0x029B39F8
#define FadeMgr_StartFade 0x029B398C

#define FadeThing 0x101D9DEC
#define FadeThing_CallFade 0x029A3620

#define ENTER_LEVEL_NORMAL 0x1
#define ENTER_LEVEL_MORTON_CASTLE_SECRET 0x26

GCT_ASM(
// clang-format off

GCT_INSERT(0x0220F850, CheckAndLoadLevel)
    bl      L_CheckAndLoadLevel_Code;
    // Magic
    .long   0, 0, 0;

    .byte   0; // Valid
    .byte   0; // {Padding}
    .byte   0; // Area
    .byte   1; // Entrance

    .long   1 - 1; // World
    .long   2 - 1; // Level

L_CheckAndLoadLevel_Code:;
    mflr    r31;
    // 09 86 54 45 24 2F B6 7F 21 31 DA 7C
    // This is just so an external program can search and find the memory address
    lis     r10, 0x0986;
    ori     r10, r10, 0x5445;
    stw     r10, 0(r31);
    lis     r10, 0x242F;
    ori     r10, r10, 0xB67F;
    stw     r10, 4(r31);
    lis     r10, 0x2131;
    ori     r10, r10, 0xDA7C;
    stw     r10, 8(r31);

    // Check if valid
    lbz     r0, 0xC(r31);
    cmpwi   r0, 0;
    beq-    L_CheckAndLoadLevel_Out;

    li      r0, 0;
    stb     r0, 0xC(r31);

    // Load level
    mr      r3, r30;
    lwz     r4, 0x10(r31);
    lwz     r5, 0x14(r31);
    BL_CALL(WMThing_LoadLevel);

    // Start transition
    // lis     r31, PORT(CSSeqMgr)@ha;
    // lwz     r3, PORT(CSSeqMgr)@l(r31);

    // li      r0, 0x26;
    // stw     r0, 0x8(sp);
    // stw     r0, 0xC(sp);

    // addi    r4, r1, 0x8;
    // BL_CALL(0x021E4D20);

    // li      r0, 0;
    // stw     r0, 0x18(r1);
    // stw     r3, 0x10(r1);
    // stw     r0, 0x14(r1);
    // addi    r4, r1, 0xC;
    // lwz     r3, PORT(CSSeqMgr)@l(r31);
    // BL_CALL(CSSeqMgr_addToQueue);

    // lis        r3, PORT(FadeMgr)@ha;
    // lwz        r3, PORT(FadeMgr)@l(r3);
    // lis        r4, PORT(0x101D9DEC)@ha;
    // lwz        r4, PORT(0x101D9DEC)@l(r4);
    // lwz        r5, 0x10(r31);
    // lwz        r6, 0x14(r31);
    // li r5, 0;
    // li r6, 1;
    // BL_CALL(FadeMgr_FadeToLevel);

    lis        r3, PORT(StartGameInfo)@ha;
    lwz        r3, PORT(StartGameInfo)@l(r3);


    //lwz        r0, 0x10(r31);
    li         r0, 0;
    stb        r0, 0x1C(r3);
    //lwz        r0, 0x14(r31);
    li         r0, 0;
    stb        r0, 0x1D(r3);

    // Do Fade
    lis        r3, PORT(FadeMgr)@ha;
    lwz        r3, PORT(FadeMgr)@l(r3);
    lis        r4, PORT(FadeThing)@ha;
    lwz        r4, PORT(FadeThing)@l(r4);

    li         r10, 2;
    lis        r9, PORT(0x0202B644)@h;
    ori        r9, r9, PORT(0x0202B644)@l;
    stw        r10, 0x8(r1);
    stw        r9, 0xC(r1);

    addi       r5, r1, 0x8;
    li         r6, 0;
    li         r7, 0;
    BL_CALL(FadeMgr_StartFade);
    cmpwi      r3, 0;
    beq-       L_CheckAndLoadLevel_Out;

    lis        r3, PORT(StartGameInfo)@ha;
    lwz        r3, PORT(StartGameInfo)@l(r3);

    lbz        r0, 0xE(r31);
    stb        r0, 0x1E(r3);
    lbz        r0, 0xF(r31);
    stb        r0, 0x1F(r3);

    li         r0, 1;
    stw        r0, 0x20(r3);

    

L_CheckAndLoadLevel_Out:;
    lmw     r25, 0x24(r1); // Original instruction
GCT_INSERT_END(CheckAndLoadLevel)

// clang-format on
) // GCT_ASM