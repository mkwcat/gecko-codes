// [Gecko]
// $Cycle Through Powerups [mkwcat, TCAtrevor]
// *Press Minus (-) in-game to cycle through all powerup states.

#define NO_ASM_CALL_THUNK
#include <gct.h>

GCT_ASM(
    // clang-format off

GCT_INSERT(0x80145560, PlayerExecuteHook)
    lhz     r0, 0xEA4 + 0x4(r30); // Held buttons
    rlwinm. r0, r0, 0, 0x1000;
    beq+    L_NotHeldMinus;

    lwz     r4, 0x14E0(r30); // powerup
    addi    r4, r4, 1;
    cmpwi   r4, 7;
    blt-    L_ValidPowerup;
    li      r4, 0;

L_ValidPowerup:;
    mr      r3, r30;
    BL_CALL(0x80145C00); // dAcPy_c::setPowerup

L_NotHeldMinus:;
    lwz     r0, 0x14(r1); // Original instruciton
GCT_INSERT_END(PlayerExecuteHook)

    // clang-format on
) // GCT_ASM