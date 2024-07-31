// [Gecko]
// $Fall Damage [mkwcat]
// *Adds aggressive fall damage to the game. Can currently be cheesed using
// *Yoshi or Mini Mushroom. Note some levels may be impossible without
// *[Allow-Movement-During-Cutscenes](Allow-Movement-During-Cutscenes.md).
// *
// *Adds unique death messages for [Death-Messages](Death-Messages.md).

#include <gct-use-cpp.h>
#include <gct.h>

#include <dol/bases/d_a_player.hpp>
#include <sound.hpp>
#include <types.h>

GCT_ASM(
    // clang-format off

GCT_INSERT(0x8004939C, GroundPoundLandHook)
    mr      r3, r31;
    bl      HandleGroundPoundLand;
    addi    r3, r31, 0xEA4; // Original instruction
GCT_INSERT_END(GroundPoundLandHook)

GCT_INSERT(0x800550B4, NormalLandHook)
    mr      r3, r31;
    bl      HandleNormalLand;
    lwz     r3, 0x10D4(r31); // Original instruction
GCT_INSERT_END(NormalLandHook)

// Disable twirl
GCT_WRITE_INSTR(0x80129B30, blr)

// Normal max fall speed
GCT_WRITE_FLOAT(0x8042BB44, -8.0)

// Mini Mario max fall speed
GCT_WRITE_FLOAT(0x8042DC0C, -8.0)

// Propeller Normal max fall speed
GCT_WRITE_FLOAT(0x802F5A20, -8.0)

// Propeller shaking max fall speed
GCT_WRITE_FLOAT(0x802F5A24, -7.0)

// Propeller fall acceleration rate
// Slower than the normal -0.34
GCT_WRITE_FLOAT(0x802F5A28, -0.2)

// Propeller drill max fall speed
GCT_WRITE_FLOAT(0x802F5A40, -9.0)
GCT_WRITE_FLOAT(0x8042DA3C, -9.0)

// Being thrown max fall speed
GCT_WRITE_FLOAT(0x8042DAB4, -8.0)

    // clang-format on
) // GCT_ASM

extern "C" void HandleGroundPoundLand(daPlBase_c* player)
{
    player->setDamage2(nullptr, daPlBase_c::DAMAGE_CRUSH);
}

extern "C" void HandleNormalLand(daPlBase_c* player)
{
    float accel = player->mSpeed.y;

    // Deal damage based on how fast the player was falling
    if (accel >= -3.5) {
        return;
    }

    player->setDamage2(nullptr, daPlBase_c::DAMAGE_BOUNCE);

    if (accel <= -7.4) {
        player->playSound(SE_PLY_PRPL_LETDOWN_FAST_LAND, 0);
        dAcPy_c::sDeathMessageOverride = "%s fell from a high place";
        player->setDamage2(nullptr, daPlBase_c::DAMAGE_CRUSH);
    } else if (accel <= -5.8) {
        dAcPy_c::sDeathMessageOverride = "%s hit the ground too hard";
        player->setDamage2(nullptr, daPlBase_c::DAMAGE_NORMAL);
    }
}

// Propeller drill acceleration rate
// Faster than the normal -0.34
const float s_drillRate = -0.6;

cGCT_INSERT_BL( //
    0x80126CC4
) const float* getDrillAccelRate(daPlBase_c* player)
{
    return &s_drillRate;
}