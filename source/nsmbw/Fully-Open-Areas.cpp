// [Gecko]
// $Fully Open Areas [mkwcat]
// *Load the entire course at once and remove the screen scroll limits.
// *This puts the entire course in one big area, allowing you to jump between sections as if it's one big area.
// *This code can break the camera in certain stages, and may cause actors to not load entirely (see 4-Castle).

#include <gct.h>

// Loop through all zones and choose the largest boundaries

GCT_ASM(
// clang-format off

GCT_INSERT(0x8008E428, ZoneLoopStart)
    li      r8, 0;
    li      r9, 0x7FFF;
    li      r10, 0x7FFF;
    li      r11, 0;
    li      r12, 0;
    mtctr   r6;
GCT_INSERT_END(ZoneLoopStart)

GCT_INSERT(0x8008E43C, ZoneLoopInner)
    cmplw   r0, r4;
    bne-    L_ZoneLoopInner_NotMatch;
    mr      r8, r3;

L_ZoneLoopInner_NotMatch:;
    lhz     r6, 0(r3);
    cmplw   r6, r9;
    bgt-    L_ZoneLoopInner_NotNewLeft;
    mr      r9, r6;

L_ZoneLoopInner_NotNewLeft:;
    lhz     r7, 2(r3);
    cmplw   r7, r10;
    bgt-    L_ZoneLoopInner_NotNewTop;
    mr      r10, r7;

L_ZoneLoopInner_NotNewTop:;
    lhz     r0, 4(r3);
    add     r0, r6, r0;
    cmplw   r0, r11;
    blt+    L_ZoneLoopInner_NotNewRight;
    mr      r11, r0;

L_ZoneLoopInner_NotNewRight:;
    lhz     r0, 6(r3);
    add     r0, r7, r0;
    cmplw   r0, r12;
    blt+    L_ZoneLoopInner_NotNewBottom;
    mr      r12, r0;

L_ZoneLoopInner_NotNewBottom:;
    // Modify condition to always skip returning
    li      r0, 0;
    cmpwi   r0, 1;
GCT_INSERT_END(ZoneLoopInner)

GCT_INSERT(0x8008E4B8, ZoneLoopEnd)
    // Fix end of loop return value
    mr.     r3, r8;
#define LoopMatch 0x8008E444
    lis     r6, PORT(LoopMatch)@h;
    ori     r6, r6, PORT(LoopMatch)@l;
    mtctr   r6;
    bnectr-;
GCT_INSERT_END(ZoneLoopEnd)

// Fix return values
GCT_WRITE_INSTR(0x8008E44C, mr r0, r9)
GCT_WRITE_INSTR(0x8008E468, mr r0, r10)
GCT_WRITE_INSTR(0x8008E484, sub r0, r11, r9)
GCT_WRITE_INSTR(0x8008E498, sub r0, r12, r10)

// Fix not found return value in get zone containing position
GCT_WRITE_INSTR(0x8008E7D4, li r3, 0) // Force to zone 1

// Skip actor out of zone checks
GCT_WRITE_INSTR(0x80064DA0, li r3, 0)
GCT_WRITE_INSTR(0x80064DA4, blr)

// Spawn actors from every zone
GCT_WRITE_INSTR(0x8006889C, li r0, 0)
GCT_WRITE_INSTR(0x800688A8, lwz r0, 0x94(r5))
GCT_WRITE_INSTR(0x800688B8, lwz r30, 0x24(r5))
GCT_WRITE_INSTR(0x800688CC, li r6, 0x0)
GCT_WRITE_INSTR(0x800684E4, nop)

// Fix goal pole destroying itself if out of zone
GCT_IF_EQUAL_32(0x80A099BC, 0x90030AA0)
GCT_WRITE_INSTR(0x80A099C0, li r3, 1)
GCT_ENDIF(1)

// Change zone scroll and zoom mode
GCT_WRITE_INSTR(0x8008E548, li r3, 3)
GCT_WRITE_INSTR(0x8008E578, li r3, 1)

// clang-format on
) // GCT_ASM