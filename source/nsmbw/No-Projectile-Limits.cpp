// [Gecko]
// $No Projectile Limits [mkwcat]
// *Disables the projectile use limits for fireballs and iceballs, allowing you
// *to use more than two at once.

#include <gct.h>

GCT_ASM(
    // clang-format off

GCT_WRITE_INSTR(0x8011B0A4, li r3, 1) // Fireballs
GCT_WRITE_INSTR(0x80124744, li r3, 1) // Iceballs

    // clang-format on
)