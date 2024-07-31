// [Gecko]
// $Save Anytime [mkwcat]
// *Add the Save button to the pause menu without beating the game.

#include <gct.h>

GCT_ASM(
    // clang-format off

GCT_WRITE_INSTR(0x025A4AB8, nop)

    // clang-format on
) // GCT_ASM