// [Gecko]
// $Duplicate Anything (NSMBU Version) [mkwcat]
// *Duplicate any actor in the game, based on the Lift Anything code.
// *Pick up a copy of whatever actor is nearest to you when you shake,
// *regardless of whether or not it can normally be picked up.

#define NO_ASM_CALL_THUNK
#include "gct.h"

#define ActorMgr_instance 0x101D0548
#define ActorMgr_GetActorByUniqueID 0x0200A5C8
#define ActorMgr_Create 0x02009A58

#define PlayerMgr_instance 0x101EF9D4
#define GetPlayerCenterPos 0x0290E2E8

#define IceBlockActor_GetCarryPos 0x0236F52C
#define IceBlockActor_SetCarryPos 0x0236F58C

#define OFFS_ACTOR_CARRIED 0x221
#define OFFS_ACTOR_CARRY_COOLDOWN 0x222
#define OFFS_ACTOR_PLAYER_ID 0x54
#define OFFS_ACTOR_IS_PLAYER 0x20D

#define OFFS_ACTOR_SETTINGS1 0x10
#define OFFS_ACTOR_SETTINGS2 0x14
#define OFFS_ACTOR_PROFILE_PTR 0x8
#define OFFS_ACTOR_POSITIONX 0x6C
#define OFFS_ACTOR_POSITIONY 0x70
#define OFFS_ACTOR_POSITIONZ 0x74
#define OFFS_ACTOR_LAYER 0x56
#define OFFS_ACTOR_EVENT_ID_1 0x213
#define OFFS_ACTOR_EVENT_ID_2 0x214
#define OFFS_ACTOR_MOVEMENT_ID 0x18
#define OFFS_ACTOR_LINK_ID 0x19
#define OFFS_ACTOR_INIT_STATE_FLAG 0x1A

GCT_ASM(
// clang-format off

// Always allow carry
GCT_WRITE_INSTR(0x02949B58, nop)

// Set default carry values in Actor constructor
GCT_INSERT(0x02000F14, ActorCtorTail)
    // Set carried and carry cooldown to 0
    li      r0, 0;
    stb     r0, OFFS_ACTOR_CARRIED(r3);
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r3);
    blr;
GCT_INSERT_END(ActorCtorTail)

// Override setSpinLiftupActor
GCT_INSERT(0x029491D0, SetSpinLiftupActor)
    cmpwi   r0, 0;
    beq-    L_SetSpinLiftupActor_Emulate;
    
    // Check for blr
    lwzx    r12, 0, r0; // Because we have to read from r0 lol
    xoris   r12, r12, 0x4E80;
    cmplwi  r12, 0x0020;
    beq-    L_SetSpinLiftupActor_Emulate;

    // Run original function
    bctrl;
    b       L_SetSpinLiftupActor_Out;

L_SetSpinLiftupActor_Emulate:;
    // Copy player ID
    lbz     r0, OFFS_ACTOR_PLAYER_ID(r4);
    stb     r0, OFFS_ACTOR_PLAYER_ID(r3);

    li      r0, 1;
    stb     r0, OFFS_ACTOR_CARRIED(r3);

L_SetSpinLiftupActor_Out:;
GCT_INSERT_END(SetSpinLiftupActor);

// Update the carry position and iframe timer every frame
GCT_INSERT(0x02001188, PostExecute)
    lbz     r0, OFFS_ACTOR_CARRIED(r30);
    cmpwi   r0, 1;
    bne-    L_PostExecute_NotCarried;

    // We are being carried
    li      r0, 30;
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r30);

    mr      r3, r30;
    stwu    sp, -0x20(sp);
    addi    r4, sp, 0x8;

    lwz     r0, 0x6C(r30);
    stw     r0, 0x8(sp);
    lwz     r0, 0x70(r30);
    stw     r0, 0xC(sp);
    lwz     r0, 0x74(r30);
    stw     r0, 0x10(sp);

    BL_CALL(IceBlockActor_GetCarryPos);
    mr      r3, r30;
    addi    r4, sp, 0x8;
    BL_CALL(IceBlockActor_SetCarryPos);
    addi    sp, sp, 0x20;

    b       L_PostExecute_Out;

L_PostExecute_NotCarried:;
    // Decrement cooldown
    lbz     r4, OFFS_ACTOR_CARRY_COOLDOWN(r30);
    cmpwi   r4, 0;
    beq-    L_PostExecute_Out;

    subi    r0, r4, 1;
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r30);

L_PostExecute_Out:;
    lwz     r0, 0x14(sp); // Original instruction
GCT_INSERT_END(PostExecute)

// Handle dropping the carried actor
GCT_INSERT(0x02948FA0, SetCarryFall)
    lis     r3, ActorMgr_instance@ha;
    lwz     r3, ActorMgr_instance@l(r3);
    addi    r3, r3, 0x6A20;
    addi    r4, r31, 0x2784;
    BL_CALL(ActorMgr_GetActorByUniqueID);
    cmpwi   r3, 0;
    beq-    L_SetCarryFall_Out;

    li      r0, 0;
    stb     r0, OFFS_ACTOR_CARRIED(r3);

L_SetCarryFall_Out:;
    lwz     r3, 0x2750(r31); // Original instruction
GCT_INSERT_END(SetCarryFall)

// Disable damage from carried actor
GCT_INSERT(0x0294D230, BlockCarryDamage)
    cmpwi   r4, 0;
    beq-    L_BlockCarryDamage_NoCarry;

    lwz     r11, 0x40(r4);
    cmpwi   r11, 0;
    beq-    L_BlockCarryDamage_NoCarry;

    // Only allow damage if the carry cooldown is 0
    lbz     r0, OFFS_ACTOR_CARRY_COOLDOWN(r11);
    cmpwi   r0, 0;
    beq-    L_BlockCarryDamage_NoCarry;

    lbz     r0, OFFS_ACTOR_PLAYER_ID(r11);
    lbz     r9, OFFS_ACTOR_PLAYER_ID(r3);
    cmpw    r9, r0;
    bne-    L_BlockCarryDamage_NoCarry;

    li      r3, 0; // No damage
    blr;

L_BlockCarryDamage_NoCarry:;
    mflr    r0; // Original instruction
GCT_INSERT_END(BlockCarryDamage)

// Refactor the lifting code to _always_ lift the nearest actor to the player
GCT_INSERT(0x02915C20, LiftNearest)
    // Check that it's a lifting action
    cmpwi   r8, 18;
    bne-    L_LiftNearest_NotLift;

    // We are doing a spin action
    // Instead of doing the typical collision check, let's find
    // the nearest actor to the player and lift that

    .set    rPlayer, 28; // Already set

    stwu    sp, -0x40(sp);
    stw     r26, 0x8(sp);
    .set    rList, 26;
    stw     r25, 0xC(sp);
    .set    rListEnd, 25;
    stw     r24, 0x10(sp);
    .set    rActor, 24;
    stw     r23, 0x14(sp);
    .set    rMatch, 23;
    stfd    f31, 0x18(sp);
    .set    fMatchDistance, 31;
    stfd    f30, 0x20(sp);
    .set    fMatchOffcenter, 30;

    lis     r3, PlayerMgr_instance@ha;
    lwz     r3, PlayerMgr_instance@l(r3);
    lbz     r5, OFFS_ACTOR_PLAYER_ID(rPlayer);
    addi    r4, sp, 0x28;
    BL_CALL(GetPlayerCenterPos);

    lis     r4, PORT(ActorMgr_instance)@ha;
    lwz     r4, PORT(ActorMgr_instance)@l(r4);
    lwz     rList, 0x6A24(r4);
    lwz     rListEnd, 0x6A2C(r4);

    FLOAT_PTR(r9, FloatMax, 1e+09);
    lfs     fMatchDistance, 0(r9); // Closest distance so far
    li      rMatch, 0; // Nullptr
    b       L_LiftNearest_LoopCond;

L_LiftNearest_LoopInner:;
    lwz     rActor, 0(rList);
    cmpwi   rActor, 0;
    beq-    L_LiftNearest_LoopNext;

    // Check if the actor is the player
    cmpw    rActor, rPlayer;
    beq-    L_LiftNearest_LoopNext;

        // Check if this actor is being held already
    lbz     r3, OFFS_ACTOR_CARRIED(rActor);
    cmpwi   r3, 1;
    bne-    L_LiftNearest_NotHeldAlready;

    // Allow picking it up if it's held by the current player anyway
    lbz     r3, OFFS_ACTOR_PLAYER_ID(rActor);
    lbz     r0, OFFS_ACTOR_PLAYER_ID(rPlayer);
    cmpw    r3, r0;
    bne-    L_LiftNearest_LoopNext;

L_LiftNearest_NotHeldAlready:;
    // Don't allow duplicating other players
    lbz     r0, OFFS_ACTOR_IS_PLAYER(rActor);
    cmpwi   r0, 1;
    beq-    L_LiftNearest_LoopNext;

    // Calc actor center pos
    psq_l   f0, 0x6C(rActor), 0, 0;
    psq_l   f13, 0x234(rActor), 0, 0;
    ps_add  f0, f0, f13;
    psq_st  f0, 0x34(sp), 0, 0;

    // Do nearest calculation
    lfs     f3, 0x28(sp); // Player X
    lfs     f2, 0x34(sp); // Actor X
    lfs     f1, 0x2C(sp); // Player Y
    lfs     f0, 0x38(sp); // Actor Y
    fsubs   f2, f3, f2;
    fsubs   f3, f1, f0;
    fmuls   f1, f2, f2;
    fmuls   f0, f3, f3;
    fadds   f0, f1, f0;
    fcmpo   cr0, fMatchDistance, f0;
    ble-    L_LiftNearest_LoopNext;

    // We found a closer actor
    fmr     fMatchDistance, f0;
    lfs     fMatchOffcenter, 0x34(sp);
    mr      rMatch, rActor;

L_LiftNearest_LoopNext:;
    addi    rList, rList, 0x4;
 L_LiftNearest_LoopCond:;
    cmplw   rList, rListEnd;
    bne+    L_LiftNearest_LoopInner;

    // Check if we found a match
    cmpwi   rMatch, 0;
    beq-    L_LiftNearest_NoMatch;

    // This is the only part modified from the Lift Anything code
    subi    sp, sp, 0x40;
    // Stack layout for this part:
    // first 0x10: reserved
    // 0x10-0x3B: ActorCreateParam instance
    // 0x3C-0x3F: the value stored across the function call

    // Store this value for after the function call
    lfs     f2, 0x6C(rPlayer);
    fsubs   f1, f2, fMatchOffcenter;
    fabs    f1, f1;
    frsp    f1, f1;
    stfs    f1, 0x3C(sp);

    // Spawn a new actor with the same parameters as the selected one

    lwz     r0, OFFS_ACTOR_SETTINGS1(rMatch);
    stw     r0, 0x10(sp);
    lwz     r0, OFFS_ACTOR_SETTINGS2(rMatch);
    stw     r0, 0x14(sp);
    // 0x8 is parent ID -- skip
    lwz     r0, OFFS_ACTOR_PROFILE_PTR(rMatch);
    stw     r0, 0x1C(sp);
    lwz     r0, OFFS_ACTOR_POSITIONX(rPlayer);
    stw     r0, 0x20(sp);
    lwz     r0, OFFS_ACTOR_POSITIONY(rPlayer);
    stw     r0, 0x24(sp);
    lwz     r0, OFFS_ACTOR_POSITIONZ(rPlayer);
    stw     r0, 0x28(sp);
    // 0x1C is angle -- 99% of actors don't use it, and those that do store it in different
    // places (usually angle.y, but sometimes angle.z or even other places). Skip
    lbz     r0, OFFS_ACTOR_LAYER(rMatch);
    stb     r0, 0x30(sp);
    lbz     r0, OFFS_ACTOR_EVENT_ID_1(rMatch);
    stb     r0, 0x31(sp);
    lbz     r0, OFFS_ACTOR_EVENT_ID_2(rMatch);
    stb     r0, 0x32(sp);
    // 0x23 is a pad byte, skip for now
    lbz     r0, OFFS_ACTOR_MOVEMENT_ID(rMatch);
    stb     r0, 0x34(sp);
    lbz     r0, OFFS_ACTOR_LINK_ID(rMatch);
    stb     r0, 0x35(sp);
    lbz     r0, OFFS_ACTOR_INIT_STATE_FLAG(rMatch);
    stb     r0, 0x36(sp);
    // 0x27-0x2B are pad bytes, skip for now

    lis     r3, ActorMgr_instance@ha;
    lwz     r3, ActorMgr_instance@l(r3);
    addi    r4, sp, 0x10;
    li      r5, 0; // 0 = cCreateOption_ActiveLater, 1 = cCreateOption_ActiveImmediately

    stw     r5, 0x18(sp); // take advantage of our zeroed r5 to fill
    stw     r5, 0x2C(sp); // in the zero or pad bytes from earlier
    stb     r5, 0x33(sp);
    stb     r5, 0x37(sp);
    stw     r5, 0x38(sp);

    BL_CALL(ActorMgr_Create);

    cmpwi   r3, 0;
    beq-    L_LiftNearest_Out;

    // Set the new actor as carried
    li      r0, 1;
    stb     r0, OFFS_ACTOR_CARRIED(r3);
    li      r0, 30;
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r3);

    // Copy player ID
    lbz     r0, OFFS_ACTOR_PLAYER_ID(rPlayer);
    stb     r0, OFFS_ACTOR_PLAYER_ID(r3);

    // Set the player's lift reserve
    lwz     r0, 0x4(r3);
    stw     r0, 0x2A50(rPlayer);
    lfs     f0, 0x3C(sp);
    stfs    f0, 0x2A54(rPlayer);

    addi    sp, sp, 0x40;

L_LiftNearest_NoMatch:;
    lfd     f30, 0x20(sp);
    lfd     f31, 0x18(sp);
    lwz     r23, 0x14(sp);
    lwz     r24, 0x10(sp);
    lwz     r25, 0xC(sp);
    lwz     r26, 0x8(sp);
    addi    sp, sp, 0x40;
    li      r3, 1;
    b       L_LiftNearest_Out;

L_LiftNearest_NotLift:;
    lis     r12, PORT(0x0219DE2C)@h;
    ori     r12, r12, PORT(0x0219DE2C)@l;
    mtctr   r12;
    bctrl;

L_LiftNearest_Out:;
GCT_INSERT_END(LiftNearest);


#define OFFS_WMACTOR_CARRIED 0xD9

#define WMPlayer_GetCarryPos 0x02211918
#define WMPlayer_PlayVoice 0x022248D0
#define WMPlayer_SetCarryFlag 0x0220F3EC

#define WMActorMgr_instance 0x101D978C
#define WMActorMgr_GetPlayer1 0x02231CD0

#define WMKeyMgr_instance 0x101D9294
#define WMKeyMgr_CheckButton 0x025990E0

// Set default carry values in WMActorBase constructor
GCT_INSERT(0x021B26F4, WMActorBaseCtorTail)
    // Set carried to 0
    li      r0, 0;
    stb     r0, OFFS_WMACTOR_CARRIED(r3);
    blr;
GCT_INSERT_END(WMActorBaseCtorTail)

// Worldmap lifting
GCT_INSERT(0x02009E04, WMSetCarryPos)
    bctrl; // Original instruction

    lis     r3, PORT(WMActorMgr_instance)@ha;
    lwz     r3, PORT(WMActorMgr_instance)@l(r3);
    cmpwi   r3, 0;
    beq-    L_WMSetCarryPos_Out;

    lbz     r0, OFFS_WMACTOR_CARRIED(r28);
    cmpwi   r0, 1;
    bne-    L_WMSetCarryPos_Out;

    // Get the player 1 actor
    BL_CALL(WMActorMgr_GetPlayer1);
    cmpwi   r3, 0;
    beq-    L_WMSetCarryPos_Out;

    cmpw    r3, r28;
    beq-    L_WMSetCarryPos_Out;

    // Are we still being held
    lwz     r0, 0x48C(r3);
    lwz     r4, 0x4(r28);
    cmpw    r0, r4;
    beq-    L_WMSetCarryPos_StillHeld;

    // We were dropped
    li      r0, 0;
    stb     r0, OFFS_WMACTOR_CARRIED(r28);
    b       L_WMSetCarryPos_Out;

L_WMSetCarryPos_StillHeld:;
    // Copy angle
    lwz     r0, 0xB4(r3);
    stw     r0, 0xB4(r28);
    stw     r0, 0xC0(r28);

    // Copy carry position
    addi    r4, r28, 0x50;
    BL_CALL(WMPlayer_GetCarryPos);

    // Copy to the other position values that exist for some reason
    lfs     f1, 0x50(r28);
    lfs     f2, 0x54(r28);
    lfs     f3, 0x58(r28);

    stfs    f1, 0x5C(r28);
    stfs    f2, 0x60(r28);
    stfs    f3, 0x64(r28);

    stfs    f1, 0x68(r28);
    stfs    f2, 0x6C(r28);
    stfs    f2, 0x70(r28);

    stfs    f1, 0x74(r28);
    stfs    f2, 0x78(r28);
    stfs    f3, 0x7C(r28);

L_WMSetCarryPos_Out:;
GCT_INSERT_END(WMSetCarryPos)

// Listen for minus button press on the worldmap
GCT_INSERT(0x0228EFA8, WMLiftCheck)
    lis     r4, PORT(WMKeyMgr_instance)@ha;
    lwz     r4, PORT(WMKeyMgr_instance)@l(r4);
    lwz     r0, 0xB3C + 0x2DE4(r4);
    rlwinm. r3, r0, 32-10, 0x1;
    bne-    L_WMLiftCheck_BtnPressed;

    lwz     r0, 0xB3C + 0x2DE8(r4);
    rlwinm. r3, r0, 32-10, 0x1;
    bne-    L_WMLiftCheck_BtnPressed;

L_WMLiftCheck_BtnPressed:;
    bl      L_WMLiftCheck_MFLR;
L_WMLiftCheck_MFLR:;
    mflr    r31;
    lwzu    r4, L_WMLiftCheck_Data - L_WMLiftCheck_MFLR(r31);
    stw     r3, 0(r31);

    cmpw    r3, r4;
    ble+    L_WMLiftCheck_Out;

    // Get the player 1 actor
    lis     r3, PORT(WMActorMgr_instance)@ha;
    lwz     r3, PORT(WMActorMgr_instance)@l(r3);
    cmpwi   r3, 0;
    beq-    L_WMLiftCheck_Out;

    .set    rPlayer, 29;
    .set    rList, 10;
    .set    rListEnd, 11;
    .set    rActor, 12;
    .set    rMatch, 28;
    .set    fMatchDistance, 10;

    BL_CALL(WMActorMgr_GetPlayer1);
    mr.     rPlayer, r3;
    beq-    L_WMLiftCheck_Out;

    // Check if carrying something already
    lbz     r0, OFFS_WMACTOR_CARRIED(rPlayer);
    cmpwi   r0, 1;
    bne-    L_WMLiftCheck_Lift;

    // Drop
    li      r0, 0;
    stb     r0, OFFS_WMACTOR_CARRIED(rPlayer);
    stw     r0, 0x48C(rPlayer);

    lwz     r7, 0x4C(rPlayer); // vtable
    lwz     r9, 0x2A4(r7);
    mtctr   r9;
    lis     r8, PORT(0x1004AE40)@ha;
    lfs     f1, PORT(0x1004AE40)@l(r8);
    li      r4, 0;
    mr      r3, rPlayer;
    li      r5, 0;
    bctrl;

    b       L_WMLiftCheck_Out;

L_WMLiftCheck_Data:;
    .long   0; // btnpress
    .float  1e+09;

L_WMLiftCheck_Lift:;
    // Find the nearest actor and lift it
    lis     r4, PORT(ActorMgr_instance)@ha;
    lwz     r4, PORT(ActorMgr_instance)@l(r4);
    lwz     rList, 0x6A24(r4);
    lwz     rListEnd, 0x6A2C(r4);

    and.    r0, rList, rListEnd;
    beq-    L_WMLiftCheck_Out;

    lfs     fMatchDistance, 4(r31); // Closest distance so far
    li      rMatch, 0; // Nullptr
    b       L_WMLiftCheck_LoopCond;

L_WMLiftCheck_LoopInner:;
    lwz     rActor, 0(rList);
    cmpwi   rActor, 0;
    beq-    L_WMLiftCheck_LoopNext;

    // Check if the actor is a player
    cmpw    rActor, rPlayer;
    beq-    L_WMLiftCheck_LoopNext;

L_WMLiftCheck_NotHeldAlready:;
    // Do nearest calculation
    lfs     f3, 0x50(rPlayer); // Player X
    lfs     f2, 0x50(rActor); // Actor X
    lfs     f1, 0x58(rPlayer); // Player Z
    lfs     f0, 0x58(rActor); // Actor Y
    fsubs   f2, f3, f2;
    fsubs   f3, f1, f0;
    fmuls   f1, f2, f2;
    fmuls   f0, f3, f3;
    fadds   f0, f1, f0;
    fcmpo   cr0, fMatchDistance, f0;
    ble-    L_WMLiftCheck_LoopNext;

    // We found a closer actor
    fmr     fMatchDistance, f0;
    mr      rMatch, rActor;

L_WMLiftCheck_LoopNext:;
    addi    rList, rList, 0x4;
L_WMLiftCheck_LoopCond:;
    cmplw   rList, rListEnd;
    bne+    L_WMLiftCheck_LoopInner;

    // Check if we found a match
    cmpwi   rMatch, 0;
    beq-    L_WMLiftCheck_NoMatch;

    // Set the lifted actor and leave
    // Play voice
    mr      r3, rPlayer;
    li      r4, 0x13;
    BL_CALL(WMPlayer_PlayVoice);

    lwz     r6, 0x4(rMatch);
    stw     r6, 0x48C(rPlayer);

    lwz     r7, 0x4C(rPlayer); // vtable
    lwz     r9, 0x2A4(r7);
    mtctr   r9;
    lis     r8, PORT(0x10049A24)@ha;
    lfs     f1, PORT(0x10049A24)@l(r8);
    li      r4, 0x24;
    mr      r3, rPlayer;
    li      r5, -1;
    bctrl;

    mr      r3, rPlayer;
    BL_CALL(WMPlayer_SetCarryFlag);

    li      r0, 1;
    stb     r0, OFFS_WMACTOR_CARRIED(rMatch);
    stb     r0, OFFS_WMACTOR_CARRIED(rPlayer);

L_WMLiftCheck_NoMatch:;
L_WMLiftCheck_Out:;
    lmw     r26, 0x0120(sp); // Original instruction
GCT_INSERT_END(WMLiftCheck);

// clang-format on
) // GCT_ASM
