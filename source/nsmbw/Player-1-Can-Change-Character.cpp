// [Gecko]
// $Player 1 Can Change Character [mkwcat]
// *This code allows player 1 to select a character other than Mario,
// *and allows other players to select Mario.
// *Currently buggy and unfinished.

#define NO_ASM_CALL_THUNK
#include <gct.h>

#include <sound.hpp>

GCT_ASM(
        // clang-format off

.set mPlayerType__9daPyMng_c, PORT(0x80355160);
.set writeIntToTextBox, PORT(0x800B3B60);
.set DWORD_8093CDF0, PORT(0x8093CDF0);
.set DWORD_8093F520, PORT(0x8093F520);

#define WMPlayer_loadSubplayers 0x80902BD0
#define nw4r_snd_detail_BasicSound_SetVolume 0x80267560

// Skip constructing the base player 1 model
GCT_WRITE_INSTR(0x809027F8, li r4, -1)

// Skip destructing it then
GCT_WRITE_INSTR(0x809028F8, nop)

// Then don't load the subplayers afterwards
GCT_WRITE_INSTR(0x809029A8, nop)

// Copy subplayer model into player 1 model
GCT_INSERT(0x80902B7C, WMPlayerUseSubplayerModel)
    mr      r3, r31;
    BL_CALL(WMPlayer_loadSubplayers);

    // Find the subplayer model
    lis     r9, mPlayerType__9daPyMng_c@ha;
    lwz     r9, mPlayerType__9daPyMng_c@l(r9); // Player 1 character
    lwz     r10, 0x184(r31);

L_WMPlayerUseSubplayerModel_Loop:;
    lwz     r4, 0x4(r10); // Character index
    rlwinm  r4, r4, 2, 0x3C;
    addis   r4, r4, DWORD_8093CDF0@ha;
    lwz     r4, DWORD_8093CDF0@l(r4);

    cmpw    r9, r4;
    bne     L_WMPlayerUseSubplayerModel_LoopCond;

    // Copy the model
    lwz     r4, 0x1CC(r10);
    lwz     r4, 0x4(r4);
    stw     r4, 0x1CC + 0x4(r31);
    b       L_WMPlayerUseSubplayerModel_Out;

L_WMPlayerUseSubplayerModel_LoopCond:;
    lwz     r10, 0x184(r10);
    cmpwi   r10, 0;
    bne     L_WMPlayerUseSubplayerModel_Loop;

L_WMPlayerUseSubplayerModel_Out:;
GCT_INSERT_END(WMPlayerUseSubplayerModel)

// Load Mario model for WM subplayer
GCT_WRITE_INSTR(0x80902BE8, li r30, 0)


// Only call this thing with 1 on subplayers

GCT_INSERT(0x80902D58, WMSubplayerFix01)
    subic   r5, r23, 1;
    subfe   r5, r5, r23;
GCT_INSERT_END(WMSubplayerFix01)

GCT_INSERT(0x80902D6C, WMSubplayerFix02)
    subic   r5, r23, 1;
    subfe   r5, r5, r23;
GCT_INSERT_END(WMSubplayerFix02)


// Fix some float thing in WM subplayer
GCT_INSERT(0x80902E14, WMSubplayerFixFloat)
    stw     r3, 0x294(r4); // Original instruction
    lwz     r4, 0x184(r4);
    stw     r3, 0x294(r4);

    // Then copy player 1's status thing
    lwz     r9, 0x384(r31);
    lis     r10, mPlayerType__9daPyMng_c@ha;
    lwz     r10, mPlayerType__9daPyMng_c@l(r10); // Player 1 character
    slwi    r10, r10, 2;
    stwx    r9, r6, r10;
GCT_INSERT_END(WMSubplayerFixFloat)


// Pre-copy the status as well when loading in from the menu
GCT_INSERT(0x80909410, WMSubplayerFixStatusOnLoad)
    lwz     r0, 0x0(r6);
    slwi    r0, r0, 2;

    lwz     r8, 0x384(r5);
    cmpwi   r8, 0x3;
    beq     L_WMSubplayerFixStatusOnLoad_NotActive;

    mr      r8, r4;
L_WMSubplayerFixStatusOnLoad_NotActive:;
    stwx    r8, r3, r0;

    addi    r7, r5, 0x8; // Original instruction
GCT_INSERT_END(WMSubplayerFixStatusOnLoad)


// Fix wrong array used in WM subplayer load model function
// (This is a base game bug I guess)
GCT_WRITE_INSTR(0x808EB7D8, lis r5, DWORD_8093CDF0@ha)
GCT_WRITE_INSTR(0x808EB7E4, addi r5, r5, DWORD_8093CDF0@l)


// Play voice on add if param 4 is 1 regardless
GCT_WRITE_INSTR(0x80902E78, nop)
GCT_WRITE_INSTR(0x80902E84, stb r29, 0x124(r31))


// Change Mario's DECIDE_JOIN sound to one that actually exists
GCT_WRITE_32(0x802F62C8, SE_VOC_MA_HUKKATSU)

// Luigi's COURSE_IN
GCT_WRITE_32(0x802F63D0, SE_VOC_LU_CS_COURSE_IN_MULTI)
GCT_WRITE_32(0x802F63D8, SE_VOC_LU_CS_COURSE_IN_MULTI)

// Blue Toad's COURSE_IN
GCT_WRITE_32(0x802F64EC, SE_VOC_KO_CS_COURSE_IN_MULTI)
GCT_WRITE_32(0x802F64F4, SE_VOC_KO_CS_COURSE_IN_MULTI)

// Yellow Toad's COURSE_IN
GCT_WRITE_32(0x802F6608, SE_VOC_KO2_CS_COURSE_IN_MULTI)
GCT_WRITE_32(0x802F6610, SE_VOC_KO2_CS_COURSE_IN_MULTI)


// Make a PLAYER_DECIDE sound for every other player as well
GCT_WRITE_INSTR(0x807A1C8C, nop)
GCT_INSERT(0x807A1C94, PlayerDecideSound)
    SHORT_PTR(r10, D_PlayerDecideSound_Sounds,
        SE_VOC_MA_PLAYER_DECIDE,
        SE_VOC_LU_CS_DECIDE_JOIN,
        SE_VOC_KO_CS_DECIDE_JOIN,
        SE_VOC_KO2_CS_DECIDE_JOIN
    );

    lis     r9, mPlayerType__9daPyMng_c@ha;
    lwz     r9, mPlayerType__9daPyMng_c@l(r9); // Player 1 character
    slwi    r9, r9, 1;
    lhzx    r4, r10, r9;

    // TODO: Can't think of a good way to do mini sounds for other characters
    bne     L_PlayerDecideSound_NotMini;
    cmpwi   r9, 0;
    bne     L_PlayerDecideSound_NotMini;

    li      r4, SE_VOC_MA_PLAYER_DECIDE_MAME;

L_PlayerDecideSound_NotMini:;
GCT_INSERT_END(PlayerDecideSound)


// Save param 2 for following hook
GCT_INSERT(0x8019AAB4, IncreaseCourseInVolumeSaveParam2)
    mflr    r0;
    stw     r4, 0x8(r1);
GCT_INSERT_END(IncreaseCourseInVolumeSaveParam2)

// Increase volume of course in to adjust for the multiplayer sound being quieter
GCT_INSERT(0x8019AB8C, IncreaseCourseInVolume)
    // Effectively the original instruction
    stfs    f1, 0xBC(r3);

    // Check if Mario
    lwz     r0, 0xAC(r29);
    cmpwi   r0, 0;
    beq     cr7, IncreaseCourseInVolume_Out;

    // Check if COURSE_IN sound
    lwz     r0, 0x8(r1);
    cmpwi   r0, 5;
    beq     IncreaseCourseInVolume_CorrectSound;

    cmpwi   r0, 7;
    bne     IncreaseCourseInVolume_Out;

IncreaseCourseInVolume_CorrectSound:;
    FLOAT_PTR(r4, D_IncreaseCourseInVolume_Volume, 2.0);
    lfs     f1, 0(r4);
    li      r4, 0;
    BL_CALL(nw4r_snd_detail_BasicSound_SetVolume);

IncreaseCourseInVolume_Out:;
GCT_INSERT_END(IncreaseCourseInVolume)


// Save WMPlayer class
GCT_INSERT(0x80902ED4, WMChangeP1Model_SaveReg)
    mflr    r0;
    stw     r3, 0x8(r1);
GCT_INSERT_END(WMChangeP1Model_SaveReg)

// Set the player 1 model after changing characters
GCT_INSERT(0x80902F20, WMChangeP1Model)
    lis     r9, mPlayerType__9daPyMng_c@ha;
    lwz     r9, mPlayerType__9daPyMng_c@l(r9); // Player 1 character
    cmpw    r9, r28;
    bne     WMChangeP1Model_Out;

    lwz     r9, 0x8(r1);
    stw     r3, 0x1CC + 0x4(r9);  

WMChangeP1Model_Out:;
    lwz     r12, 0x0(r3); // Original instruction
GCT_INSERT_END(WMChangeP1Model)


#define SETZERO16(addr) GCT_WRITE_16(addr + 2, 0)

// Fix FUN_80902ca0
SETZERO16(0x80902D1C)
SETZERO16(0x80902D20)
SETZERO16(0x80902D24)
SETZERO16(0x80902D2C)

// Fix FUN_80902fa0
GCT_WRITE_INSTR(0x8090301C, nop) // Don't set P1 powerup to Mario's
SETZERO16(0x8090302C)
SETZERO16(0x80903030)
SETZERO16(0x80903038)

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
GCT_WRITE_INSTR(0x800BB1B4, stw r7, 0x384(r3))


// Allow player 1 to register
GCT_WRITE_INSTR(0x807A0FE8, nop)


// Prevent cancelling Add/Drop players when player 1 is not registered

GCT_INSERT(0x807A0D24, CancelAddDropCheckPlayer1)
    mflr    r0; // Original instruction
    // Check if player 1 is registered
    lwz     r4, 0x74(r3);
    lbz     r4, 0x29A(r4);
GCT_INSERT_END(CancelAddDropCheckPlayer1)

// For pressing 2 in an inappropiate time at the World Map
GCT_INSERT(0x807A0D40, CancelAddDropFix)
    // Return 0 if player 1 is not registered
    cmpwi   r4, 0;
    bne-    L_CancelAddDropFix_Out;

    li      r3, 0;

    // Return the function
    lwz     r0, 0x14(r1);
    mtlr    r0;
    addi    r1, r1, 0x10;
    blr;

L_CancelAddDropFix_Out:;
    li      r4, 0x900; // Original instruction
GCT_INSERT_END(CancelAddDropFix)

// For pressing 1 in an inappropiate time in the Main Menu
GCT_INSERT(0x807A0D68, CancelAfterRegisteredFix)
    li      r3, 0; // Original instruction

    // Return 0 if player 1 is registered
    cmpwi   r4, 0;
    beq-    L_CancelAfterRegisteredFix_Out;

    // Return the function
    lwz     r0, 0x14(r1);
    mtlr    r0;
    addi    r1, r1, 0x10;
    blr;

L_CancelAfterRegisteredFix_Out:;
GCT_INSERT_END(CancelAfterRegisteredFix)


// Disable preventing 2D Mario from jumping here
GCT_WRITE_INSTR(0x807A12A8, nop)


// Prevent 2D player 1 from jumping here
GCT_INSERT(0x808C5DFC, PreventJumpOnWM)
    lis     r9, mPlayerType__9daPyMng_c@ha;
    lwz     r9, mPlayerType__9daPyMng_c@l(r9);
    lwz     r10, 0x4(r3);
    rlwinm. r10, r10, 0, 0xF;
    // r9 == r10
    xor     r10, r10, r9;
    cntlzw  r10, r10;
    srwi.   r10, r10, 5;
GCT_INSERT_END(PreventJumpOnWM)


// Allow selecting Mario
GCT_WRITE_INSTR(0x80770A8C, cmpwi r3, 4)


// Remove Mario's "here we go"
GCT_WRITE_INSTR(0x807AC8CC, li r0, 1)


// Don't skip character select with 1 player
GCT_WRITE_INSTR(0x80918B50, nop)


// Fix arrows
GCT_WRITE_INSTR(0x8076E630, cmpwi r0, 4)
GCT_WRITE_INSTR(0x8076EAB8, cmpwi r3, 4)


// Fix 2 button not glowing for P1
GCT_WRITE_INSTR(0x8076DDE8, nop)


// Fix in use select
GCT_WRITE_INSTR(0x80770988, rlwinm r0, r0, 2, 0xC)
GCT_WRITE_INSTR(0x807709A8, nop)
GCT_WRITE_INSTR(0x807709B0, rlwinm r4, r0, 0, 1)


// Similar fixes
GCT_WRITE_INSTR(0x8076FE4C, rlwinm r0, r0, 2, 0xC)


// Fix default selection for Mario
GCT_INSERT(0x8076FF0C, CharacterChangeSelectBase_FixDefaultSelection)
    cmpwi   r0, 0;
    bne     CharacterChangeSelectBase_FixDefaultSelection_Out;

    li      r0, 4;

CharacterChangeSelectBase_FixDefaultSelection_Out:;
    li      r4, 0; // Original instruction
GCT_INSERT_END(CharacterChangeSelectBase_FixDefaultSelection)


GCT_INSERT(0x8076FE84, CharacterChangeSelectBase_FixDefaultSelection2)
    cmpwi   r0, 0;
    bne     CharacterChangeSelectBase_FixDefaultSelection2_Out;

    li      r0, 4;

CharacterChangeSelectBase_FixDefaultSelection2_Out:;
    stw     r0, 0x2E0(r3); // Original instruction
GCT_INSERT_END(CharacterChangeSelectBase_FixDefaultSelection2)


// Fix Mario's voice on select
GCT_INSERT(0x8077117C, CharacterChangeSelectBase_FixMarioVoice)
    li      r4, 1010; // SE_VOC_KO2_PLAYER_JOIN

    lwz     r0, 0x2D4(r31);
    cmpwi   r0, 0;
    bne     CharacterChangeSelectBase_FixMarioVoice_NotMario;

    li      r4, 797; // SE_VOC_MA_PLAYER_JOIN
CharacterChangeSelectBase_FixMarioVoice_NotMario:;
GCT_INSERT_END(CharacterChangeSelectBase_FixMarioVoice)


GCT_INSERT(0x80771164, CharacterChangeSelectBase_FixMarioVoiceMini)
    li      r4, 1011; // SE_VOC_KO2_PLAYER_JOIN_MAME

    lwz     r0, 0x2D4(r31);
    cmpwi   r0, 0;
    bne     CharacterChangeSelectBase_FixMarioVoiceMini_NotMario;

    li      r4, 798; // SE_VOC_MA_PLAYER_JOIN_MAME
CharacterChangeSelectBase_FixMarioVoiceMini_NotMario:;
GCT_INSERT_END(CharacterChangeSelectBase_FixMarioVoiceMini)


// Change Mario icon
GCT_INSERT(0x8076FDC4, CharacterChangeSelectBase_FixInUseCheck)
    cmpwi   r31, 7;
    bne     CharacterChangeSelectBase_FixInUseCheck_Out;

    li      r4, 0;

CharacterChangeSelectBase_FixInUseCheck_Out:;
    mr      r3, r29;
GCT_INSERT_END(CharacterChangeSelectBase_FixInUseCheck)


GCT_INSERT(0x8076FDD4, CharacterChangeSelectBase_FixInUseCheck2)
    cmpwi   r31, 7;
    bne     CharacterChangeSelectBase_FixInUseCheck2_Out;

    li      r31, 8;

CharacterChangeSelectBase_FixInUseCheck2_Out:;
    cmpwi   r31, 0;
GCT_INSERT_END(CharacterChangeSelectBase_FixInUseCheck2)


// Read array differently
GCT_WRITE_INSTR(0x8076FD8C, lbzx r31, r6, r5)
GCT_STRING(0x80933CF0, CharacterChangeSelectBase_ExpandIconArray)
    .byte   6, 0, 1, 2, 7;
GCT_STRING_END(CharacterChangeSelectBase_ExpandIconArray)

GCT_INSERT(0x8077359C, CharacterChangeSelectBase_FixIconUpdate)
    cmpwi   r4, 7;
    beq     CharacterChangeSelectBase_FixIconUpdate_MarioUnlocked;

    cmpwi   r4, 8;
    bne     CharacterChangeSelectBase_FixIconUpdate_Else;

CharacterChangeSelectBase_FixIconUpdate_MarioLocked:;
    // Make the gray out thing visible
    slwi    r0, r5, 2;
    add     r5, r3, r0;
    lwz     r5, 0x254(r5);
    lbz     r0, 0xBB(r5);
    ori     r0, r0, 0x01;
    stb     r0, 0xBB(r5);
    // Fall through

CharacterChangeSelectBase_FixIconUpdate_MarioUnlocked:;
    addi    r3, r3, 0x2A4; // param 1
    slwi    r0, r31, 2;
    lis     r4, DWORD_8093F520@ha;
    addi    r4, r4, DWORD_8093F520@l; // param 2
    add     r5, r30, r0; // param 3
    lwz     r5, 0x274(r5);
    li      r6, 0; // param 4

    // Call the function
    lis     r12, writeIntToTextBox@h;
    ori     r12, r12, writeIntToTextBox@l;
    mtctr   r12;
    bctrl;

CharacterChangeSelectBase_FixIconUpdate_Else:;
    cmpwi   r4, 4;
CharacterChangeSelectBase_FixIconUpdate_Out:;
GCT_INSERT_END(CharacterChangeSelectBase_FixIconUpdate)

    // clang-format on
) // GCT_ASM