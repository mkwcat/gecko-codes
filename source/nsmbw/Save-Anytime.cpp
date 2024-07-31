// [Gecko]
// $Save Anytime [mkwcat]
// *Add the Save button to the pause menu regardless of whether the game has
// *been completed.

#include <gct.h>

GCT_ASM(
    // clang-format off

GCT_WRITE_INSTR(0x8077AA7C, nop) // Message
GCT_WRITE_INSTR(0x8092FD00, li r0, 0x2) // Button behavior

    // clang-format on
) // GCT_ASM