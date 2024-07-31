// [Gecko]
// $World Map Expansion Support [mkwcat]
// *A simple support code to fix various issues with the base game's world map
// *code, to allow easier expansion and modification.

#include <gct.h>

GCT_ASM(
    // clang-format off

// Fix an offset error in parsing the point name with atoi
GCT_WRITE_INSTR(0x800F969C, addi r3, r30, 2);

// Fix digit issue with recreating the point name
GCT_INSERT(0x800F9508, FixPointDigitName)
    li      r0, 10;
    addi    r3, r30, 1;
    divw    r3, r3, r0;
    addi    r0, r3, '0';
    stb     r0, 2(r31);

    mulli   r0, r3, 10;
GCT_INSERT_END(FixPointDigitName)

// Display A - P for courses 10 - 20
GCT_INSERT(0x800E6CA8, DisplayLetterCourseName)
    subi    r4, r31, 10;
    cmplwi  r4, 20;
    bge     L_DisplayLetterCourseName_Out;

    addi    r4, r4, 'A';
    sth     r4, 0x4(r30);
    li      r3, 1;

L_DisplayLetterCourseName_Out:;
    lwz     r0, 0x14(r1); // Original instruction
GCT_INSERT_END(DisplayLetterCourseName)

    // clang-format on
) // GCT_ASM