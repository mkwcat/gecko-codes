// [Gecko]
// $Exit Course Anytime [mkwcat]
// *Allows the player to exit any course, regardless of whether or not the level
// *has been completed.

#include <gct.h>

GCT_ASM(
    // clang-format off

// dGameCom::isNowCourseClear()
GCT_WRITE_INSTR(0x800B4EA8, li r3, 1);

    // clang-format on
) // GCT_ASM