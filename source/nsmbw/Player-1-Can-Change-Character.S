// [Gecko]
// $Player 1 Can Change Character [mkwcat]
// *This code allows player 1 to select a character other than Mario,
// *and allows other players to select Mario.
// *Currently buggy and unfinished.

#include <gct.h>

.set mPlayerType__9daPyMng_c, PORT(0x80355160)

.set writeIntToTextBox, PORT(0x800B3B60)
.set DWORD_8093F520, PORT(0x8093F520)

// Load player 1 character for the world map model
GCT_INSERT(0x809027F8, WMLoadP1Char)
    lis     r4, mPlayerType__9daPyMng_c@ha
    lwz     r4, mPlayerType__9daPyMng_c@l(r4)
GCT_INSERT_END(WMLoadP1Char)

#define SETZERO16(addr) GCT_WRITE_16(addr + 2, 0)

// Fix FUN_807A0060
SETZERO16(0x807A0088)
SETZERO16(0x807A008C)

// Fix FUN_807A0170
SETZERO16(0x807A0190)
GCT_WRITE_16(0x807A018E, 0x0004)

// Fix dNumberOfPeopleChange_c::beginState_OnStageAnimeEndWait
SETZERO16(0x807A11F0)
SETZERO16(0x807A1204)

// Fix dNumberOfPeopleChange_c::beginState_NowEntrantRecruit
SETZERO16(0x807A14D8)
SETZERO16(0x807A14DC)

// Fix dNumberOfPeopleChange_c::executeState_NowEntrantRecruit
SETZERO16(0x807A1518)
SETZERO16(0x807A152C)
SETZERO16(0x807A1534)
SETZERO16(0x807A1628)
SETZERO16(0x807A162C)
SETZERO16(0x807A1634)
SETZERO16(0x807A1658)
SETZERO16(0x807A165C)
SETZERO16(0x807A15C8)
SETZERO16(0x807A15CC)

// Fix dNumberOfPeopleChange_c::beginState_StoryModeCourseSelectExitWait
SETZERO16(0x807A16D8)
SETZERO16(0x807A16DC)

// Fix dNumberOfPeopleChange_c::executeState_StoryModeCourseSelectExitWait
SETZERO16(0x807A1718)
SETZERO16(0x807A172C)
SETZERO16(0x807A1734)
SETZERO16(0x807A17AC)
SETZERO16(0x807A17B0)

// Don't force set P1 mario position
GCT_WRITE_INSTR(0x807A068C, nop)
GCT_WRITE_INSTR(0x807A0694, nop)
GCT_WRITE_INSTR(0x807A069C, nop)

// Don't make Mario automatically registered
GCT_WRITE_INSTR(0x80918B28, li r0, 0)

// Allow player 1 to register
GCT_WRITE_INSTR(0x8076FBC8, nop)

// Prevent cancelling Add/Drop players when player 1 is not registered

GCT_INSERT(0x807A0D24, WMCancelAddDropSaveRegister)
    // Save a register for a later hook
    mflr    r0 // Original instruction
    mr      r4, r3
GCT_INSERT_END(WMCancelAddDropSaveRegister)

GCT_INSERT(0x807A0D40, WMCancelAddDropFix)
    // Return 0 if player 1 is not registered
    lwz     r4, 0x74(r4)
    lbz     r4, 0x29A(r4)
    cmpwi   r4, 0
    bne-    L_WMCancelAddDropFixOut

    li      r3, 0

    // Return the function
    lwz     r0, 0x14(r1)
    mtlr    r0
    addi    r1, r1, 0x10
    blr

L_WMCancelAddDropFixOut:
    li      r4, 0x900 // Original instruction
GCT_INSERT_END(WMCancelAddDropFix)

// Allow selecting Mario
GCT_WRITE_INSTR(0x80770A8C, cmpwi r3, 4)

// Remove Mario's "here we go"
GCT_WRITE_INSTR(0x807AC8CC, li r0, 1)

// Don't skip character select with 1 player
GCT_WRITE_INSTR(0x80918B50, nop)

// Fix arrows
GCT_WRITE_INSTR(0x8076EAB8, cmpwi r3, 4)

// Change Mario icon
GCT_INSERT(0x8076FDC4, CharacterChangeSelectBase_FixInUseCheck)
    cmpwi   r31, 4
    bne     CharacterChangeSelectBase_FixInUseCheck_Out

    li      r4, 0

CharacterChangeSelectBase_FixInUseCheck_Out:
    mr      r3, r29
GCT_INSERT_END(CharacterChangeSelectBase_FixInUseCheck)

GCT_INSERT(0x8076FDD4, CharacterChangeSelectBase_FixInUseCheck2)
    cmpwi   r31, 4
    bne     CharacterChangeSelectBase_FixInUseCheck2_Out

    li      r31, 8

CharacterChangeSelectBase_FixInUseCheck2_Out:
    cmpwi   r31, 0
GCT_INSERT_END(CharacterChangeSelectBase_FixInUseCheck2)

// Read array differently
GCT_WRITE_INSTR(0x8076FD8C, lbzx r31, r6, r5)
GCT_STRING(0x80933CF0, CharacterChangeSelectBase_ExpandIconArray)
    .byte   6, 0, 1, 2, 7
GCT_STRING_END(CharacterChangeSelectBase_ExpandIconArray)

GCT_INSERT(0x8077359C, CharacterChangeSelectBase_FixIconUpdate)
    cmpwi   r4, 7
    beq     CharacterChangeSelectBase_FixIconUpdate_MarioUnlocked

    cmpwi   r4, 8
    bne     CharacterChangeSelectBase_FixIconUpdate_Else

CharacterChangeSelectBase_FixIconUpdate_MarioLocked:
    // Make the gray out thing visible
    slwi    r0, r5, 2
    add     r5, r3, r0
    lwz     r5, 0x254(r5)
    lbz     r0, 0xBB(r5)
    ori     r0, r0, 0x01
    stb     r0, 0xBB(r5)
    // Fall through

CharacterChangeSelectBase_FixIconUpdate_MarioUnlocked:
    addi    r3, r3, 0x2A4 // param 1
    slwi    r0, r31, 2
    lis     r4, DWORD_8093F520@ha
    addi    r4, r4, DWORD_8093F520@l // param 2
    add     r5, r30, r0 // param 3
    lwz     r5, 0x274(r5)
    li      r6, 0 // param 4

    // Call the function
    lis     r12, writeIntToTextBox@h
    ori     r12, r12, writeIntToTextBox@l
    mtctr   r12
    bctrl

    // addi    r0, r31, 6
    // slwi    r0, r0, 2
    // add     r3, r30, r0
    // lwz     r3, 0x254(r3)
    // lbz     r0, 0xBB(r3)
    // ori     r0, r0, 0x01
    // stb     r0, 0xBB(r3)

CharacterChangeSelectBase_FixIconUpdate_Else:
    cmpwi   r4, 4
CharacterChangeSelectBase_FixIconUpdate_Out:
GCT_INSERT_END(CharacterChangeSelectBase_FixIconUpdate)