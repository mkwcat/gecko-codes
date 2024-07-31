// [Gecko]
// $Allow Movement During Cutscenes [mkwcat]
// *Allows player inputs during certain types of cutscenes. May cause softlocks
// *and/or unintended side effects depending on how it's used.

#include <gct.h>

GCT_ASM(
    // clang-format off

// Disable pre-applying forced input
GCT_WRITE_INSTR(0x8005E0A8, b 0x20);
GCT_WRITE_INSTR(0x8005E16C, b 0x14);

GCT_INSERT(0x8005E1B8, AddForcedMovement)
    // Check flags
    lhz     r4, 0x12(r31);
    rlwinm. r0, r4, 0, 0x1; // Input disabled
    bne-    L_AddForcedMovement_Out;

    rlwinm. r0, r4, 0, 0x80; // Forced
    beq-    L_AddForcedMovement_Out;

    // Add forced buttons
    lhz     r5, 0xC(r31); // Permanent
    lhz     r6, 0xE(r31); // Transient
    or      r5, r5, r6;

    lhz     r6, 0x4(r31); // Held buttons
    or      r0, r5, r6;
    sth     r0, 0x4(r31);

    andc    r5, r5, r6;
    lhz     r6, 0x6(r31); // Pressed buttons
    or      r0, r5, r6;
    sth     r0, 0x6(r31); // Pressed buttons

    // Now do shaking
    rlwinm. r0, r4, 0, 0x100; // Disable shaking
    bne-    L_AddForcedMovement_Out;

    lhz     r5, 0x10(r31); // Forced shake value
    lbz     r6, 0x18(r31);
    lbz     r7, 0x19(r31);
    or      r6, r6, r5;
    or      r7, r7, r5;
    stb     r6, 0x18(r31);
    stb     r7, 0x19(r31);

L_AddForcedMovement_Out:;
    lbz     r0, 0x15(r31);
GCT_INSERT_END(AddForcedMovement)

    // clang-format on
) // GCT_ASM