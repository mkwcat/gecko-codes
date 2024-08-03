// [Gecko]
// $Disable Powerup Change Pause [mkwcat]
// *Disables the short freeze that occurs when changing powerup state or dying.
// *The player still stop during the animation, but enemies and objects will
// *continue to move.

#include <gct.h>

GCT_ASM(
    // clang-format off

// Disable freeze when changing powerup state
// Patches in dAcPy_c::executeChangeInit
GCT_WRITE_INSTR(0x801410C4, nop)
GCT_WRITE_INSTR(0x801410D0, nop)
// Stub dAcPy_c::initChangeInit
GCT_WRITE_INSTR(0x80141020, blr)

// Disable freeze when a player dies in multiplayer
// Stub dAcPy_c::stopOtherDownDemo
GCT_WRITE_INSTR(0x8013DA30, blr)
// Stub dAcPy_c::playOtherDownDemo
GCT_WRITE_INSTR(0x8013DB30, blr)

// Disable freeze when Yoshi walks into a fruit
// Patches in daYoshi_c::executeLastAll
GCT_WRITE_INSTR(0x80150E54, li r3, 0)
GCT_WRITE_INSTR(0x80150E98, li r3, 0)

    // clang-format on
)