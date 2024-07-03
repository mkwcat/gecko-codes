// [Gecko]
// $Connect All Pipes [mkwcat]
// *Connect all pipes within a certain area, so that rather than triggering a fade and screen transition,
// *the screen will simply scroll to the exit pipe before the player leaves it, and the area does not reload.
// *
// *This code _requires_ Fully Open Areas to be enabled, as it relies on the entire course being loaded at once.

#include <gct.h>

#define m_instance__10dScStage_c 0x8042A25C

GCT_ASM(
// clang-format off

// Apply Newer Wii's connected pipe fixes
GCT_STRING(0x800508F8, NewerWiiPipeFix01)
    lhz     r0, 0x10(r30);
    rlwinm. r0, r0, 0, 0x1;
    beq-    L_NewerWiiPipeFix01_0x1C;
    lhz     r7, 0x4(r3);
    subi    r0, r7, 0x2;
    sth     r0, 0x42C(r31);
    b       L_NewerWiiPipeFix01_0x24;

L_NewerWiiPipeFix01_0x1C:;
    li      r0, 0x1;
    sth     r0, 0x42C(r31);

L_NewerWiiPipeFix01_0x24:;
    lhz     r5, 0x2(r3);
    lha     r4, 0x42C(r31);
    lwz     r6, 0x3C(r6); // This gets patched later anyway
    add     r0, r5, r4;
    slwi    r0, r0, 4;
    add     r5, r6, r0;
    lha     r4, 0x2(r5);
GCT_STRING_END(NewerWiiPipeFix01)

GCT_WRITE_INSTR(0x80050A5C, lbz r0, 0x13(r3))
GCT_WRITE_INSTR(0x80050A60, cmpwi r0, 0)
GCT_WRITE_INSTR(0x80050A68, cmpwi r0, 1)
GCT_WRITE_INSTR(0x80050A70, cmpwi r0, 2)
GCT_WRITE_INSTR(0x80050A78, cmpwi r0, 3)
GCT_WRITE_INSTR(0x80050A88, addi r4, r31, 0x65C)


// Rewrite getNextGoto to store connected pipe locations
GCT_INSERT(0x8008E3D0, GetNextGoto)
    mr      r5, r3;
    li      r9, 0; // Doing again flag
L_GetNextGoto_DoItAgain:;
    lwz     r0, 0x90(r5);
    lwz     r3, 0x20(r5);
    mtctr   r0;
    cmpwi   r0, 0;
    ble-    L_GetNextGoto_ReturnNull;

L_GetNextGoto_Loop:;
    lbz     r0, 0x8(r3);
    cmplw   r0, r4;
    bne-    L_GetNextGoto_Next;

    // Check if we're just doing it again
    cmpwi   r9, 0;
    bnelr-;

    // OK this is the first goto thing
    // Check if the area is our area
    // 0 means current area
    lbz     r0, 0x9(r3);
    cmpwi   r0, 0;
    beq-    L_GetNextGoto_SameArea;

    // Or the value could just be set to this area's ID
    lis     r4, PORT(m_instance__10dScStage_c)@ha;
    lwz     r4, PORT(m_instance__10dScStage_c)@l(r4);
    cmpwi   r4, 0;
    beqlr-;

    lbz     r4, 0x120E(r4);
    addi    r4, r4, 1;
    cmpw    r0, r4;
    bnelr-;
L_GetNextGoto_SameArea:;

    // Now find the dest entrance
    lbz     r4, 0xA(r3);
    mflr    r6;
    mr      r7, r3;
    li      r9, 1;
    bl      L_GetNextGoto_DoItAgain;

    mtlr    r6;
    mr.     r8, r3;
    mr      r3, r7;
    beqlr-; // Can't find the dest entrance (this is ok, but it means we must return)

    // Check if the destination is a pipe
    lbz     r4, 0xB(r8);
    subi    r4, r4, 3;
    cmpwi   r4, 6 - 3;
    bgtlr-; // Not a pipe (so we dont touch it)

    // Set pipe out direction
    stb     r4, 0x13(r3);

    // Create a rail with ID 255
    // This works because this function is called with the
    // Entrance by RailDokan just before it gets the rail
    li      r0, 0xFF;
    // Set rail/path ID for both entrances
    stb     r0, 0xF(r7);
    stb     r0, 0xF(r8);

    // The first position for the rail is always the lower entry ID
    lbz     r0, 0x8(r7);
    lbz     r4, 0x8(r8);
    cmpw    r0, r4;
    blt-    L_GetNextGoto_AlreadyOrdered;
    mr      r7, r8;
    mr      r8, r3;
L_GetNextGoto_AlreadyOrdered:;

    // r8 gets reverse flag
    lhz     r0, 0x10(r8);
    ori     r0, r0, 0x9;
    sth     r0, 0x10(r8);

    // r7 just gets connected pipe
    lhz     r0, 0x10(r7);
    ori     r0, r0, 0x8;
    rlwinm  r0, r0, 0, ~0x1;
    sth     r0, 0x10(r7);

    lis     r0, 0xFF00;
    ori     r0, r0, 0x0000;
    PTR_STWU(r0, r4, LD_FakeRailData);
    lis     r0, 0x0002;
    stw     r0, 0x4(r4);

    // Node 0
    // Copy x and y
    lhz     r5, 0x0(r7);
    lhz     r10, 0x2(r7);
    lbz     r0, 0xB(r7);
    bl      L_GetNextGoto_GetOffsetFromExitDir;
    sth     r5, 0x8 + 0x0(r4);
    sth     r10, 0x8 + 0x2(r4);

    // Set speed, accel
    lis     r0, 0x3F80;
    stw     r0, 0x8 + 0x4(r4);
    stw     r0, 0x8 + 0x8(r4);

    // Set delay, var4
    lis     r0, 0;
    stw     r0, 0x8 + 0xC(r4);

    // Node 1
    // Copy x and y
    lhz     r5, 0x0(r8);
    lhz     r10, 0x2(r8);
    lbz     r0, 0xB(r8);
    bl      L_GetNextGoto_GetOffsetFromExitDir;
    sth     r5, 0x18 + 0x0(r4);
    sth     r10, 0x18 + 0x2(r4);

    // Set speed, accel
    lis     r0, 0x3F80;
    stw     r0, 0x18 + 0x4(r4);
    stw     r0, 0x18 + 0x8(r4);

    // Set delay, var4
    lis     r0, 0;
    stw     r0, 0x18 + 0xC(r4);

    // Done, yay
    mtlr    r6;
    blr;

L_GetNextGoto_GetOffsetFromExitDir:;
    cmpwi   r0, 3;
    beq-    L_GetNextGoto_IsUp;
    cmpwi   r0, 5;
    blt-    L_GetNextGoto_IsDown;
    bgt-    L_GetNextGoto_IsRight;
L_GetNextGoto_IsLeft:;
    // I don't think this ever exists in the base game
    addi    r5, r5, 0;
    addi    r10, r10, 32;
    blr;
L_GetNextGoto_IsRight:;
    addi    r5, r5, 0;
    addi    r10, r10, 32;
    blr;
L_GetNextGoto_IsUp:;
    addi    r5, r5, 16;
    addi    r10, r10, 32;
    blr;
L_GetNextGoto_IsDown:;
    addi    r5, r5, 16;
    addi    r10, r10, 8;
    blr;

L_GetNextGoto_Next:;
    addi    r3, r3, 0x14;
    bdnz+   L_GetNextGoto_Loop;

L_GetNextGoto_ReturnNull:;
    li      r3, 0;
    blr;

LD_FakeRailData:;
    .space  0x24;
GCT_INSERT_END(GetNextGoto)

// Patch RailDokan to use the fake rail

GCT_INSERT(0x800508CC, RailDokanGetRailStruct)
    PTR(r3, LD_FakeRailData);
GCT_INSERT_END(RailDokanGetRailStruct)

GCT_INSERT(0x80050B64, RailDokanGetRailStruct2)
    PTR(r3, LD_FakeRailData);
GCT_INSERT_END(RailDokanGetRailStruct2)

GCT_INSERT(0x80050924, RailDokanGetRailList)
    PTR(r6, LD_FakeRailData + 0x8);
GCT_INSERT_END(RailDokanGetRailList)

GCT_INSERT(0x80050B9C, RailDokanGetRailList2)
    PTR(r5, LD_FakeRailData + 0x8);
GCT_INSERT_END(RailDokanGetRailList2)

// Change connected pipe speed
GCT_INSERT(0x800509A8, ConnectedPipeSpeed)
    FLOAT_PTR(r5, SpeedFloat, 4.0, 0.25);
    lfs     f2, 0x0(r5);
    lfs     f4, 0x4(r5);
GCT_INSERT_END(ConnectedPipeSpeed)

// clang-format on
) // GCT_ASM