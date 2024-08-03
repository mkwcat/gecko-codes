// [Gecko]
// $Lift Anything [mkwcat]
// *Lift literally anything in the game.
// *Pick up whatever actor is nearest to you when you shake,
// *regardless of whether or not it can normally be picked up.

#include <gct.h>

#define daLightBlock_c_calcCarryPos 0x808555D0
#define getCenterPos__12dBaseActor_cCFv 0x8006CED0
#define ActorList 0x8042A080
#define FloatMax 0x8042BB38
#define ActivePhysics_AddToList 0x8008C330

#define OFFS_ACTOR_CARRIED 0x59
#define OFFS_ACTOR_CARRY_COOLDOWN 0x5A
#define OFFS_ACTOR_IS_PLAYER 0x38C
#define OFFS_ACTOR_PLAYER_ID 0x38D

// #define ENABLE_SILLY_ROTATION 1

GCT_ASM(
    // clang-format off

// Always allow carry patches

GCT_INSERT(0x8012E3E0, SetSpinLiftupReservePatch)
    bctrl;
    li      r3, 1;
GCT_INSERT_END(SetSpinLiftupReservePatch)

GCT_INSERT(0x8012E3E0, AllowCarryPatch)
    bctrl;
    li      r3, 1;
GCT_INSERT_END(AllowCarryPatch)

GCT_WRITE_INSTR(0x8012E2C4, nop)

GCT_WRITE_INSTR(0x8012E2E0, nop)

// Override setSpinLiftupActor for dActor_c
GCT_INSERT(0x80065820, SetSpinLiftupActor)
    // Copy player ID
    lbz     r0, OFFS_ACTOR_PLAYER_ID(r4);
    stb     r0, OFFS_ACTOR_PLAYER_ID(r3);

    li      r0, 1;
    stb     r0, OFFS_ACTOR_CARRIED(r3);
    blr;
GCT_INSERT_END(SetSpinLiftupActor)

// Update the carry position and iframe timer every frame
GCT_INSERT(0x80162250, FinalUpdate)
    lbz     r0, OFFS_ACTOR_CARRIED(r3);
    cmpwi   r0, 0;
    beq-    L_FinalUpdate_NotCarried;

    // We are being carried
    li      r0, 30;
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r3);

    lis     r12, PORT(daLightBlock_c_calcCarryPos)@h;
    ori     r12, r12, PORT(daLightBlock_c_calcCarryPos)@l;
asm_call_thunk:;
    mtctr   r12;
    bctr;

L_FinalUpdate_NotCarried:;
    // Decrement cooldown
    lbz     r4, OFFS_ACTOR_CARRY_COOLDOWN(r3);
    cmpwi   r4, 0;
    beqlr-;

    subi    r0, r4, 1;
    stb     r0, OFFS_ACTOR_CARRY_COOLDOWN(r3);
    blr;
GCT_INSERT_END(FinalUpdate)

#if ENABLE_SILLY_ROTATION

// Rotate the carried actor to match the player
GCT_INSERT(0x80855610, RotateCarriedActor)
    lwz     r0, 0x100(r31); // mAngle.x, mAngle.y
    stw     r0, 0x100(r30);
    lhz     r0, 0x104(r31); // mAngle.z
    sth     r0, 0x104(r30);

    mr      r4, r31; // Original instruction
GCT_INSERT_END(RotateCarriedActor)

#endif // ENABLE_SILLY_ROTATION

// Handle dropping the carried actor
GCT_INSERT(0x8012E6B8, SetCarryFall)
    stw     r0, 0x24(r4); // Original instruction

    // Sometimes the carried actor can be null
    // if it's destroyed while being carried
    cmpwi   r31, 0;
    beq-    L_SetCarryFall_Out;
    // r0 is 0
    stb     r0, OFFS_ACTOR_CARRIED(r31);
L_SetCarryFall_Out:;
GCT_INSERT_END(SetCarryFall)

// Disable damage from carried actor
GCT_INSERT(0x80146230, BlockCarryDamage)
    cmpwi   r4, 0;
    beq-    L_BlockCarryDamage_NoCarry;

    // Only allow damage if the carry cooldown is 0
    lbz     r0, OFFS_ACTOR_CARRY_COOLDOWN(r4);
    cmpwi   r0, 0;
    beq-    L_BlockCarryDamage_NoCarry;

    lbz     r0, OFFS_ACTOR_PLAYER_ID(r4);
    lbz     r9, OFFS_ACTOR_PLAYER_ID(r3);
    cmpw    r9, r0;
    bne-    L_BlockCarryDamage_NoCarry;

    li      r3, 0; // No damage
    blr;

L_BlockCarryDamage_NoCarry:;
    stwu    sp, -0x20(sp); // Original instruction
GCT_INSERT_END(BlockCarryDamage)

// Disable collecting an item while you're carrying it
GCT_INSERT(0x80A2831C, BlockCarryCollect)
    lbz     r0, 0xDFF(r3); // Original instruction

    // Check either the actor is being carried or has a carry cooldown
    lbz     r4, 0x59(r3);
    lbz     r5, OFFS_ACTOR_CARRY_COOLDOWN(r3);
    or.     r4, r4, r5;
    beq-    L_BlockCarryCollect_NoCarry; // Equal to 0

    // Check if the holding player is the same as the player collecting
    lbz     r4, OFFS_ACTOR_PLAYER_ID(r3);
    cmpw    r4, r0;
    bne-    L_BlockCarryCollect_NoCarry;

    li      r0, 0xFF; // Invalid value that will cause a return
L_BlockCarryCollect_NoCarry:;
GCT_INSERT_END(BlockCarryCollect)

// Refactor the lifting code to _always_ lift the nearest actor to the player
GCT_INSERT(0x80053898, LiftNearest)
    // Check that it's a lifting action
    cmpwi   r0, 18;
    bne-    L_LiftNearest_NotLift;

    // Instead of doing the typical collision check, let's find
    // the nearest actor to the player and lift that
    
    lwz     r5, 0x4(r3); // Player actor

    lis     r6, PORT(ActorList)@ha;
    lwz     r6, PORT(ActorList)@l(r6);

    FLOAT_PTR(r9, FloatMax, 1e+09);
    lfs     f5, 0(r9); // Closest distance so far
    li      r9, 0; // Nullptr
    b       L_LiftNearest_LoopStart;

L_LiftNearest_LoopInner:
    lwz     r10, 0x8(r6); // Actor pointer

    // Check if it's the player
    cmpw    r10, r5;
    beq-    L_LiftNearest_LoopNext;

    // Check if it's a stage actor
    // Really weird hacky way to do this, check ActivePhysics vtable
    lwz     r3, 0x148(r10);
    xoris   r3, r3, PORT(0x803110F0)@ha;
    cmplwi  r3, PORT(0x803110F0)@l;
    bne-    L_LiftNearest_LoopNext;
    // (That's kinda what I meant when I said garbage code)

    // Check if this actor is being held already
    lbz     r3, OFFS_ACTOR_CARRIED(r10);
    cmpwi   r3, 0;
    beq-    L_LiftNearest_NotHeldAlready;

    // Allow picking it up if it's held by the current player anyway
    lbz     r3, OFFS_ACTOR_PLAYER_ID(r10);
    lbz     r0, OFFS_ACTOR_PLAYER_ID(r5);
    cmpw    r3, r0;
    bne-    L_LiftNearest_LoopNext;

L_LiftNearest_NotHeldAlready:;
    mr      r4, r10;
    subi    r3, sp, 0x80;

    // Aaaa depending on register luck hehe
    // Best GHS immitation
    BL_CALL(getCenterPos__12dBaseActor_cCFv);

    // Do nearest calculation
    // I stole this from some other function
    lfs     f3, 0xAC(r5);
    lfs     f2, 0xD0(r5);
    lfs     f1, 0xB0(r5);
    lfs     f0, 0xD4(r5);
    fadds   f3, f3, f2;
    lfs     f2, -0x80+0x0(sp);
    fadds   f1, f1, f0;
    lfs     f0, -0x80+0x4(sp);
    fsubs   f2, f3, f2;
    fsubs   f3, f1, f0;
    fmuls   f1, f2, f2;
    fmuls   f0, f3, f3;
    fadds   f0, f1, f0;
    fcmpo   cr0, f5, f0;
    ble-    L_LiftNearest_LoopNext;

    // We've found a better match
    fmr     f5, f0; // Set nearest distance
    lfs     f6, -0x80+0x0(sp);
    mr      r9, r10; // Set nearest actor

L_LiftNearest_LoopNext:;
    lwz     r6, 0x4(r6); // Next in list
L_LiftNearest_LoopStart:;
    cmpwi   r6, 0;
    bne-    L_LiftNearest_LoopInner;

    // Loop over, check if we found any actor at all
    cmpwi   r9, 0;
    bne+    L_LiftNearest_LiftActor;

L_LiftNearest_NotLift:;
    // Call the add to list function anyway
    BL_CALL(ActivePhysics_AddToList);
    b       L_LiftNearest_Out;

L_LiftNearest_LiftActor:
    // Set the lifted object and leave
    lfs     f2, 0xAC(r29);
    fsubs   f1, f2, f6;
    fabs    f1, f1;
    frsp    f1, f1;
    lwz     r0, 0x0(r9);
    stw     r0, 0x27D4(r5);
    stfs    f1, 0x27D8(r5);
    b       L_LiftNearest_Out;

L_LiftNearest_Out:;
GCT_INSERT_END(LiftNearest)

// Make held objects intangible to the holding player
GCT_INSERT(0x8008C728, MakeHeldIntangible)
    // Make sure we're comparing against a player
    lbz     r0, OFFS_ACTOR_IS_PLAYER(r6);
    cmpwi   r0, 1;
    bne-    L_MakeHeldIntangible_End;

    // Check if the actor is being carried or has a carry cooldown
    lbz     r0, OFFS_ACTOR_CARRIED(r5);
    lbz     r7, OFFS_ACTOR_CARRY_COOLDOWN(r5);
    or.     r0, r7, r0;
    beq-    L_MakeHeldIntangible_End;

    // Check if the player we're checking is the one carrying the actor
    lbz     r0, OFFS_ACTOR_PLAYER_ID(r5);
    lbz     r7, OFFS_ACTOR_PLAYER_ID(r6);
    cmpw    r0, r7;
    bne-    L_MakeHeldIntangible_End;

    // Just to trick the next condition
    // :3 silly
    xori    r0, r5, 1;
    b       L_MakeHeldIntangible_EndSkip;

L_MakeHeldIntangible_End:;
    lwz     r0, 0x8(r3);
L_MakeHeldIntangible_EndSkip:;
GCT_INSERT_END(MakeHeldIntangible)

// Make intangible via a different physics function
.set    rCount, 29;
.set    rPhysics, 28;
.set    rStoreActor, 27;

#define SearchActorByID 0x80162E40
#define PhysicsList 0x8042A0C0
#define Physics_AddToList 0x8007F900
#define Physics_RemoveFromList 0x8007F950

// Remove held actors before the collision check
GCT_INSERT(0x8005443C, RemoveHeldActorPhys)
    // Push stack even more
    stwu    sp, -0x200(sp);
    stw     r29, 0x1FC(sp);
    stw     r28, 0x1F8(sp);
    stw     r27, 0x1F4(sp);
    
    li      rCount, 0;

    // Get held actor
    lwz     r3, 0x2A78(r30);
    BL_CALL(SearchActorByID);

    mr.     rStoreActor, r3;
    beq-    L_RemoveHeldActorPhys_Out; // Equal to 0; no actor held

    // Search for all physics belonging to this actor
    lis     rPhysics, PORT(PhysicsList)@ha;
    lwz     rPhysics, PORT(PhysicsList)@l(rPhysics);
    b       L_RemoveHeldActorPhys_LoopStart;

L_RemoveHeldActorPhys_LoopInner:;
    lwz     r4, 0x0(rPhysics);
    cmpw    r4, rStoreActor;
    bne-    L_RemoveHeldActorPhys_LoopIncrement;

    // Store pointer for restoring later
    slwi    r4, rCount, 2;
    addi    r4, r4, 0x8;
    stwx    rPhysics, sp, r4;

    // Remove from list
    mr      r3, rPhysics;
    BL_CALL(Physics_RemoveFromList);

    addi    rCount, rCount, 1;

L_RemoveHeldActorPhys_LoopIncrement:;
    lwz     rPhysics, 0x4(rPhysics);

L_RemoveHeldActorPhys_LoopStart:;
    // Enforce maximum number of physics
    cmpwi   rCount, 120;
    bge-    L_RemoveHeldActorPhys_Out;

    cmpwi   rPhysics, 0;
    bne-    L_RemoveHeldActorPhys_LoopInner;

L_RemoveHeldActorPhys_Out:;
    mr      r3, r30;
GCT_INSERT_END(RemoveHeldActorPhys)

// Restore held actors after the collision check
GCT_INSERT(0x800544C4, RestoreHeldActorPhys)
    // Restore physics
    addi    rPhysics, sp, 0x8;
    b       L_RestoreHeldActorPhys_LoopStart;
L_RestoreHeldActorPhys_LoopInner:;
    lwz     r3, 0(rPhysics);

    addi    rPhysics, rPhysics, 4;
    BL_CALL(Physics_AddToList);

    subi    rCount, rCount, 1;

L_RestoreHeldActorPhys_LoopStart:;
    cmpwi   rCount, 0;
    bgt-    L_RestoreHeldActorPhys_LoopInner;

    lwz     r27, 0x1F4(sp);
    lwz     r28, 0x1F8(sp);
    lwz     r29, 0x1FC(sp);
    addi    sp, sp, 0x200;

    lwz     r0, 0x14(sp); // Original instruction
GCT_INSERT_END(RestoreHeldActorPhys)

    // clang-format on
) // GCT_ASM