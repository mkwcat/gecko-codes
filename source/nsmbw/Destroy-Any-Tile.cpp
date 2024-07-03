// [Gecko]
// $Destroy Any Tile [mkwcat]
// *Destroy any tile in the game, or convert to a coin block if you're small.

#include <gct.h>

GCT_ASM(
// clang-format off

GCT_INSERT(0x8006DA14, ExpandBreakableBlockCheck)
    andis.  r0, r30, 0x1;
    bne-    L_ExpandBreakableBlockCheck_NoMatch;

    andis.  r0, r30, 0x021A;
    bne-    L_ExpandBreakableBlockCheck_End;
    andi.   r0, r30, 0xA871;
    b       L_ExpandBreakableBlockCheck_End;

L_ExpandBreakableBlockCheck_NoMatch:;
    cmpw    r0, r0; // Set condition
L_ExpandBreakableBlockCheck_End:;
GCT_INSERT_END(ExpandBreakableBlockCheck)

// Set a custom value when calling the function so we can check it later
GCT_WRITE_INSTR(0x8006DA50, li r0, 2)

// Check that custom value and make the tile always breakable
GCT_INSERT(0x800885CC, CheckCustomBrkValue)
    lbz     r0, 0xD(r17);
    cmpwi   r0, 0x2;
    beq-    L_CheckCustomBrkValue_IsCustom;

    cmplwi  r24, 1; // Original instruction
    b       L_CheckCustomBrkValue_End;

L_CheckCustomBrkValue_IsCustom:;
    li      r0, 0x0;
    stb     r0, 0xD(r17);

L_CheckCustomBrkValue_End:;
GCT_INSERT_END(CheckCustomBrkValue)

// clang-format on
) // GCT_ASM