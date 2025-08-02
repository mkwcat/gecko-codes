// [Gecko]
// $Midair Crouching [mkwcat]
// *This code allows you to crouch in midair, replacing the ground pound input.

#define NO_ASM_CALL_THUNK
#include <gct.h>

#include <sound.hpp>

GCT_ASM(
    // clang-format off

// Redirect dAcPy_c::setHipAttackAction to dAcPy_c::checkCrouch
GCT_WRITE_BRANCH(0x801267F0, 0x8012D5D0)

// Redirect daYoshi_c::setHipAttackAction to daYoshi_c::checkCrouch
GCT_WRITE_BRANCH(0x8014E620, 0x8014DCF0)

// Only reset fall speed if it's less than -2.0
GCT_INSERT(0x8012D9FC, ResetFallSpeed)
    FLOAT_PTR(r9, D_FallSpeedLimit, -2.0)

    lfs     f1, 0xEC(r31);
    lfs     f2, 0(r9);
    fcmpo   cr0, f1, f2;
    bge     cr0, L_ResetFallSpeed_Out;

    stfs    f0, 0xEC(r31);

L_ResetFallSpeed_Out:;
GCT_INSERT_END(ResetFallSpeed)

// Emulate Super Mario Galaxy 2's flutter jump crouch bug
GCT_INSERT(0x8014DEC4, YoshiCancelFlutterJumpFix)
    lbz     r0, 0x152C(r31); // mFlutterJumpStage
    cmpwi   r0, 2;
    bgt     L_YoshiCancelFlutterJumpFix_Out;

    li      r0, 1;
    stb     r0, 0x152D(r31); // mCanFlutterJump

L_YoshiCancelFlutterJumpFix_Out:;
    lwz     r0, 0x14(r1); // Original instruction
GCT_INSERT_END(YoshiCancelFlutterJumpFix)

    // clang-format on
)