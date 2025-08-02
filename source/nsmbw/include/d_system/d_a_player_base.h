#pragma once

#include "d_actor.h"

class daPlBase_c : public dActor_c
{
public:
    enum DamageType_e {
        DAMAGE_NORMAL = 0,
        DAMAGE_NORMAL2 = 1,
        DAMAGE_KNOCKBACK_AND_HURT = 2,
        DAMAGE_KNOCKBACK_LONG = 3, // Player does not take damage
        DAMAGE_KNOCKBACK_LONG2 = 4, // Player does not take damage
        DAMAGE_KNOCKBACK_SHORT = 5, // Player does not take damage
        DAMAGE_KNOCKBACK_SHORT2 = 6, // Player does not take damage
        DAMAGE_LAVA = 7,
        DAMAGE_FIRE = 8, // Looks the same as DAMAGE_NORMAL?
        DAMAGE_ELEC_SHOCK = 9,
        DAMAGE_POISON_WATER = 10,
        DAMAGE_CRUSH = 11,
        DAMAGE_EAT_DIE = 12,
        DAMAGE_EAT_DIE2 = 13, // Like DAMAGE_EAT_DIE but does not stop the music
        DAMAGE_UNKNOWN = 14,
        DAMAGE_FREEZE = 15,
        DAMAGE_FREEZE2 = 16,
        DAMAGE_BOUNCE = 17, // Makes the player do a squishy animation
        DAMAGE_POISON_FOG = 18,
    };

    // 0x0D4 - 0x380
    IMPORT_VIRTUAL(
        0x80046DF0, 0x0D4, //
        void, VT_0x0D4
    );
    IMPORT_VIRTUAL(
        0x800588F0, 0x0D8, //
        void, executeLastPlayer
    );
    IMPORT_VIRTUAL(
        0x800588E0, 0x0DC, //
        void, executeLastAll
    );
    IMPORT_VIRTUAL(
        0x80020BE0, 0x0E0, //
        bool, isItemKinopio
    );
    IMPORT_VIRTUAL(
        0x80056BC0, 0x0E4, //
        void, VT_0x0E4
    );
    IMPORT_VIRTUAL(
        0x80056D40, 0x0E8, //
        void, VT_0x0E8
    );
    IMPORT_VIRTUAL(
        0x80048080, 0x0EC, //
        void, VT_0x0EC
    );
    IMPORT_VIRTUAL(
        0x80054EE0, 0x0F0, //
        void, VT_0x0F0
    );
    IMPORT_VIRTUAL(
        0x80056BB0, 0x0F4, //
        void, VT_0x0F4
    );
    IMPORT_VIRTUAL(
        0x80048090, 0x0F8, //
        void, VT_0x0F8
    );
    IMPORT_VIRTUAL(
        0x800544E0, 0x0FC, //
        void, VT_0x0FC
    );
    IMPORT_VIRTUAL(
        0x80054F80, 0x100, //
        void, VT_0x100
    );
    IMPORT_VIRTUAL(
        0x800552C0, 0x104, //
        void, VT_0x104
    );
    IMPORT_VIRTUAL(
        0x80053F90, 0x108, //
        void, VT_0x108
    );
    IMPORT_VIRTUAL(
        0x80053FE0, 0x10C, //
        void, VT_0x10C
    );
    IMPORT_VIRTUAL(
        0x80054070, 0x110, //
        void, VT_0x110
    );
    IMPORT_VIRTUAL(
        0x800540D0, 0x114, //
        void, VT_0x114
    );
    IMPORT_VIRTUAL(
        0x800564F0, 0x118, //
        void, VT_0x118
    );
    IMPORT_VIRTUAL(
        0x80056360, 0x11C, //
        void, setBalloonInDispOut, int
    );
    IMPORT_VIRTUAL(
        0x8004DCF0, 0x120, //
        void, VT_0x120
    );
    IMPORT_VIRTUAL(
        0x8004DF70, 0x124, //
        void, VT_0x124
    );
    IMPORT_VIRTUAL(
        0x8004F260, 0x128, //
        void, VT_0x128
    );
    IMPORT_VIRTUAL(
        0x80051080, 0x12C, //
        void, VT_0x12C
    );
    IMPORT_VIRTUAL(
        0x80050E60, 0x130, //
        void, VT_0x130
    );
    IMPORT_VIRTUAL(
        0x800588D0, 0x134, //
        void, VT_0x134
    );
    IMPORT_VIRTUAL(
        0x8004E450, 0x138, //
        void, VT_0x138
    );
    IMPORT_VIRTUAL(
        0x8004E4D0, 0x13C, //
        void, VT_0x13C
    );
    IMPORT_VIRTUAL(
        0x8004E4C0, 0x140, //
        void, VT_0x140
    );
    IMPORT_VIRTUAL(
        0x8004E700, 0x144, //
        void, VT_0x144
    );
    IMPORT_VIRTUAL(
        0x8004E710, 0x148, //
        void, VT_0x148
    );
    IMPORT_VIRTUAL(
        0x8004E7A0, 0x14C, //
        void, VT_0x14C
    );
    IMPORT_VIRTUAL(
        0x8004E7B0, 0x150, //
        void, VT_0x150
    );
    IMPORT_VIRTUAL(
        0x8004E820, 0x154, //
        void, VT_0x154
    );
    IMPORT_VIRTUAL(
        0x8004E810, 0x158, //
        void, VT_0x158
    );
    IMPORT_VIRTUAL(
        0x8004F100, 0x15C, //
        void, VT_0x15C
    );
    IMPORT_VIRTUAL(
        0x8004F120, 0x160, //
        void, VT_0x160
    );
    IMPORT_VIRTUAL(
        0x8004F110, 0x164, //
        void, VT_0x164
    );
    IMPORT_VIRTUAL(
        0x8004F130, 0x168, //
        void, VT_0x168
    );
    IMPORT_VIRTUAL(
        0x8004F150, 0x16C, //
        void, VT_0x16C
    );
    IMPORT_VIRTUAL(
        0x8004F140, 0x170, //
        void, VT_0x170
    );
    IMPORT_VIRTUAL(
        0x8004F190, 0x174, //
        void, VT_0x174
    );
    IMPORT_VIRTUAL(
        0x8004F1B0, 0x178, //
        void, VT_0x178
    );
    IMPORT_VIRTUAL(
        0x8004F1A0, 0x17C, //
        void, VT_0x17C
    );
    IMPORT_VIRTUAL(
        0x8004F160, 0x180, //
        void, VT_0x180
    );
    IMPORT_VIRTUAL(
        0x8004F180, 0x184, //
        void, VT_0x184
    );
    IMPORT_VIRTUAL(
        0x8004F170, 0x188, //
        void, VT_0x188
    );
    IMPORT_VIRTUAL(
        0x8004FE70, 0x18C, //
        void, VT_0x18C
    );
    IMPORT_VIRTUAL(
        0x8004FE90, 0x190, //
        void, VT_0x190
    );
    IMPORT_VIRTUAL(
        0x8004FE80, 0x194, //
        void, VT_0x194
    );
    IMPORT_VIRTUAL(
        0x8004FEA0, 0x198, //
        void, VT_0x198
    );
    IMPORT_VIRTUAL(
        0x8004FEC0, 0x19C, //
        void, VT_0x19C
    );
    IMPORT_VIRTUAL(
        0x8004FEB0, 0x1A0, //
        void, VT_0x1A0
    );
    IMPORT_VIRTUAL(
        0x8004FF00, 0x1A4, //
        void, VT_0x1A4
    );
    IMPORT_VIRTUAL(
        0x8004FF20, 0x1A8, //
        void, VT_0x1A8
    );
    IMPORT_VIRTUAL(
        0x8004FF10, 0x1AC, //
        void, VT_0x1AC
    );
    IMPORT_VIRTUAL(
        0x8004FED0, 0x1B0, //
        void, VT_0x1B0
    );
    IMPORT_VIRTUAL(
        0x8004FEF0, 0x1B4, //
        void, VT_0x1B4
    );
    IMPORT_VIRTUAL(
        0x8004FEE0, 0x1B8, //
        void, VT_0x1B8
    );
    IMPORT_VIRTUAL(
        0x8004FF30, 0x1BC, //
        void, VT_0x1BC
    );
    IMPORT_VIRTUAL(
        0x8004FFA0, 0x1C0, //
        void, VT_0x1C0
    );
    IMPORT_VIRTUAL(
        0x8004FF90, 0x1C4, //
        void, VT_0x1C4
    );
    IMPORT_VIRTUAL(
        0x800502A0, 0x1C8, //
        void, VT_0x1C8
    );
    IMPORT_VIRTUAL(
        0x80050380, 0x1CC, //
        void, VT_0x1CC
    );
    IMPORT_VIRTUAL(
        0x80050370, 0x1D0, //
        void, VT_0x1D0
    );
    IMPORT_VIRTUAL(
        0x800504B0, 0x1D4, //
        void, VT_0x1D4
    );
    IMPORT_VIRTUAL(
        0x80050540, 0x1D8, //
        void, VT_0x1D8
    );
    IMPORT_VIRTUAL(
        0x80050530, 0x1DC, //
        void, VT_0x1DC
    );
    IMPORT_VIRTUAL(
        0x80050870, 0x1E0, //
        void, VT_0x1E0
    );
    IMPORT_VIRTUAL(
        0x80050AF0, 0x1E4, //
        void, VT_0x1E4
    );
    IMPORT_VIRTUAL(
        0x800509F0, 0x1E8, //
        void, VT_0x1E8
    );
    IMPORT_VIRTUAL(
        0x80050E30, 0x1EC, //
        void, VT_0x1EC
    );
    IMPORT_VIRTUAL(
        0x80050E50, 0x1F0, //
        void, VT_0x1F0
    );
    IMPORT_VIRTUAL(
        0x80050E40, 0x1F4, //
        void, VT_0x1F4
    );
    IMPORT_VIRTUAL(
        0x80052FE0, 0x1F8, //
        void, VT_0x1F8
    );
    IMPORT_VIRTUAL(
        0x80053090, 0x1FC, //
        void, VT_0x1FC
    );
    IMPORT_VIRTUAL(
        0x80053040, 0x200, //
        void, VT_0x200
    );
    IMPORT_VIRTUAL(
        0x80051400, 0x204, //
        void, VT_0x204
    );
    IMPORT_VIRTUAL(
        0x80051AB0, 0x208, //
        void, VT_0x208
    );
    IMPORT_VIRTUAL(
        0x80051410, 0x20C, //
        void, VT_0x20C
    );
    IMPORT_VIRTUAL(
        0x80052730, 0x210, //
        void, VT_0x210
    );
    IMPORT_VIRTUAL(
        0x80052900, 0x214, //
        void, VT_0x214
    );
    IMPORT_VIRTUAL(
        0x80052880, 0x218, //
        void, VT_0x218
    );
    IMPORT_VIRTUAL(
        0x8004D820, 0x21C, //
        void, VT_0x21C
    );
    IMPORT_VIRTUAL(
        0x8004D830, 0x220, //
        void, VT_0x220
    );
    IMPORT_VIRTUAL(
        0x8004D840, 0x224, //
        void, VT_0x224
    );
    IMPORT_VIRTUAL(
        0x8004D850, 0x228, //
        void, VT_0x228
    );
    IMPORT_VIRTUAL(
        0x8004D860, 0x22C, //
        void, VT_0x22C
    );
    IMPORT_VIRTUAL(
        0x8004D870, 0x230, //
        void, VT_0x230
    );
    IMPORT_VIRTUAL(
        0x8004D880, 0x234, //
        void, VT_0x234
    );
    IMPORT_VIRTUAL(
        0x8004D890, 0x238, //
        void, VT_0x238
    );
    IMPORT_VIRTUAL(
        0x8004D8A0, 0x23C, //
        void, VT_0x23C
    );
    IMPORT_VIRTUAL(
        0x8004D8B0, 0x240, //
        void, VT_0x240
    );
    IMPORT_VIRTUAL(
        0x8004D8F0, 0x244, //
        void, VT_0x244
    );
    IMPORT_VIRTUAL(
        0x8004D8E0, 0x248, //
        void, VT_0x248
    );
    IMPORT_VIRTUAL(
        0x8004D8C0, 0x24C, //
        void, VT_0x24C
    );
    IMPORT_VIRTUAL(
        0x8004D8D0, 0x250, //
        void, VT_0x250
    );
    IMPORT_VIRTUAL(
        0x8004D960, 0x254, //
        void, VT_0x254
    );
    IMPORT_VIRTUAL(
        0x8004D9C0, 0x258, //
        void, VT_0x258
    );
    IMPORT_VIRTUAL(
        0x8004DA20, 0x25C, //
        void, VT_0x25C
    );
    IMPORT_VIRTUAL(
        0x8004DA70, 0x260, //
        void, VT_0x260
    );
    IMPORT_VIRTUAL(
        0x8004DA80, 0x264, //
        void, VT_0x264
    );
    IMPORT_VIRTUAL(
        0x8004DA30, 0x268, //
        void, VT_0x268
    );
    IMPORT_VIRTUAL(
        0x8004DA90, 0x26C, //
        void, VT_0x26C
    );
    IMPORT_VIRTUAL(
        0x8004DB00, 0x270, //
        void, VT_0x270
    );
    IMPORT_VIRTUAL(
        0x8004E280, 0x274, //
        void, VT_0x274
    );
    IMPORT_VIRTUAL(
        0x80055EE0, 0x278, //
        void, setFallDownDemo
    );
    IMPORT_VIRTUAL(
        0x8004F300, 0x27C, //
        void, VT_0x27C
    );
    IMPORT_VIRTUAL(
        0x8004F670, 0x280, //
        void, VT_0x280
    );
    IMPORT_VIRTUAL(
        0x80051CF0, 0x284, //
        void, VT_0x284
    );
    IMPORT_VIRTUAL(
        0x80051240, 0x288, //
        void, VT_0x288
    );
    IMPORT_VIRTUAL(
        0x80051A90, 0x28C, //
        void, VT_0x28C
    );
    IMPORT_VIRTUAL(
        0x80052870, 0x290, //
        void, VT_0x290
    );
    IMPORT_VIRTUAL(
        0x80047390, 0x294, //
        void, VT_0x294
    );
    IMPORT_VIRTUAL(
        0x800475B0, 0x298, //
        void, initializeState_None
    );
    IMPORT_VIRTUAL(
        0x800475D0, 0x29C, //
        void, executeState_None
    );
    IMPORT_VIRTUAL(
        0x800475C0, 0x2A0, //
        void, finalizeState_None
    );
    IMPORT_VIRTUAL(
        0x80047790, 0x2A4, //
        void, initializeState_Walk
    );
    IMPORT_VIRTUAL(
        0x800477B0, 0x2A8, //
        void, executeState_Walk
    );
    IMPORT_VIRTUAL(
        0x800477A0, 0x2AC, //
        void, finalizeState_Walk
    );
    IMPORT_VIRTUAL(
        0x800479A0, 0x2B0, //
        void, initializeState_Jump
    );
    IMPORT_VIRTUAL(
        0x80047AC0, 0x2B4, //
        void, executeState_Jump
    );
    IMPORT_VIRTUAL(
        0x80047A00, 0x2B8, //
        void, finalizeState_Jump
    );
    IMPORT_VIRTUAL(
        0x80047BE0, 0x2BC, //
        void, initializeState_SitJump
    );
    IMPORT_VIRTUAL(
        0x80047C00, 0x2C0, //
        void, executeState_SitJump
    );
    IMPORT_VIRTUAL(
        0x80047BF0, 0x2C4, //
        void, finalizeState_SitJump
    );
    IMPORT_VIRTUAL(
        0x80047C40, 0x2C8, //
        void, initializeState_Fall
    );
    IMPORT_VIRTUAL(
        0x80047D60, 0x2CC, //
        void, executeState_Fall
    );
    IMPORT_VIRTUAL(
        0x80047D10, 0x2D0, //
        void, finalizeState_Fall
    );
    IMPORT_VIRTUAL(
        0x80047D80, 0x2D4, //
        void, initializeState_Land
    );
    IMPORT_VIRTUAL(
        0x80047DA0, 0x2D8, //
        void, executeState_Land
    );
    IMPORT_VIRTUAL(
        0x80047D90, 0x2DC, //
        void, finalizeState_Land
    );
    IMPORT_VIRTUAL(
        0x800480A0, 0x2E0, //
        void, initializeState_Crouch
    );
    IMPORT_VIRTUAL(
        0x800480C0, 0x2E4, //
        void, executeState_Crouch
    );
    IMPORT_VIRTUAL(
        0x800480B0, 0x2E8, //
        void, finalizeState_Crouch
    );
    IMPORT_VIRTUAL(
        0x800480D0, 0x2EC, //
        void, initializeState_Slip
    );
    IMPORT_VIRTUAL(
        0x800481C0, 0x2F0, //
        void, executeState_Slip
    );
    IMPORT_VIRTUAL(
        0x80048170, 0x2F4, //
        void, finalizeState_Slip
    );
    IMPORT_VIRTUAL(
        0x80048C00, 0x2F8, //
        void, initializeState_Turn
    );
    IMPORT_VIRTUAL(
        0x80048C20, 0x2FC, //
        void, executeState_Turn
    );
    IMPORT_VIRTUAL(
        0x80048C10, 0x300, //
        void, finalizeState_Turn
    );
    IMPORT_VIRTUAL(
        0x800497E0, 0x304, //
        void, initializeState_HipAttack
    );
    IMPORT_VIRTUAL(
        0x80049940, 0x308, //
        void, executeState_HipAttack
    );
    IMPORT_VIRTUAL(
        0x80049880, 0x30C, //
        void, finalizeState_HipAttack
    );
    IMPORT_VIRTUAL(
        0x80049A10, 0x310, //
        void, initializeState_Swim
    );
    IMPORT_VIRTUAL(
        0x80049A30, 0x314, //
        void, executeState_Swim
    );
    IMPORT_VIRTUAL(
        0x80049A20, 0x318, //
        void, finalizeState_Swim
    );
    IMPORT_VIRTUAL(
        0x80049AB0, 0x31C, //
        void, initializeState_JumpDai
    );
    IMPORT_VIRTUAL(
        0x80049BB0, 0x320, //
        void, executeState_JumpDai
    );
    IMPORT_VIRTUAL(
        0x80049B60, 0x324, //
        void, finalizeState_JumpDai
    );
    IMPORT_VIRTUAL(
        0x80049E10, 0x328, //
        void, initializeState_PlayerJumpDai
    );
    IMPORT_VIRTUAL(
        0x80049FA0, 0x32C, //
        void, executeState_PlayerJumpDai
    );
    IMPORT_VIRTUAL(
        0x80049F50, 0x330, //
        void, finalizeState_PlayerJumpDai
    );
    IMPORT_VIRTUAL(
        0x8004A400, 0x334, //
        void, initializeState_Funsui
    );
    IMPORT_VIRTUAL(
        0x8004A510, 0x338, //
        void, executeState_Funsui
    );
    IMPORT_VIRTUAL(
        0x8004A4D0, 0x33C, //
        void, finalizeState_Funsui
    );
    IMPORT_VIRTUAL(
        0x8004A670, 0x340, //
        void, initializeState_Kani
    );
    IMPORT_VIRTUAL(
        0x8004A690, 0x344, //
        void, executeState_Kani
    );
    IMPORT_VIRTUAL(
        0x8004A680, 0x348, //
        void, finalizeState_Kani
    );
    IMPORT_VIRTUAL(
        0x8004A830, 0x34C, //
        void, initializeState_Cloud
    );
    IMPORT_VIRTUAL(
        0x8004AA50, 0x350, //
        void, executeState_Cloud
    );
    IMPORT_VIRTUAL(
        0x8004A8C0, 0x354, //
        void, finalizeState_Cloud
    );
    IMPORT_VIRTUAL(
        0x8004AA60, 0x358, //
        void, initializeState_AnimePlay
    );
    IMPORT_VIRTUAL(
        0x8004AAC0, 0x35C, //
        void, executeState_AnimePlay
    );
    IMPORT_VIRTUAL(
        0x8004AA80, 0x360, //
        void, finalizeState_AnimePlay
    );
    IMPORT_VIRTUAL(
        0x8004AFE0, 0x364, //
        void, initializeState_WaitJump
    );
    IMPORT_VIRTUAL(
        0x8004B0D0, 0x368, //
        void, executeState_WaitJump
    );
    IMPORT_VIRTUAL(
        0x8004B090, 0x36C, //
        void, finalizeState_WaitJump
    );
    IMPORT_VIRTUAL(
        0x800588C0, 0x370, //
        void, VT_0x370
    );
    IMPORT_VIRTUAL(
        0x8004AB80, 0x374, //
        void, VT_0x374
    );
    IMPORT_VIRTUAL(
        0x800475E0, 0x378, //
        void, VT_0x378
    );
    IMPORT_VIRTUAL(
        0x800475F0, 0x37C, //
        void, VT_0x37C
    );
    IMPORT_VIRTUAL(
        0x80047600, 0x380, //
        void, VT_0x380
    );
    IMPORT_VIRTUAL(
        0x80047610, 0x384, //
        void, VT_0x384
    );
    IMPORT_VIRTUAL(
        0x80047620, 0x388, //
        void, VT_0x388
    );
    IMPORT_VIRTUAL(
        0x80047630, 0x38C, //
        void, VT_0x38C
    );
    IMPORT_VIRTUAL(
        0x80047DB0, 0x390, //
        void, VT_0x390
    );
    IMPORT_VIRTUAL(
        0x80047DC0, 0x394, //
        void, VT_0x394
    );
    IMPORT_VIRTUAL(
        0x8004B220, 0x398, //
        void, VT_0x398
    );
    IMPORT_VIRTUAL(
        0x80048630, 0x39C, //
        void, VT_0x39C
    );
    IMPORT_VIRTUAL(
        0x80047C10, 0x3A0, //
        void, VT_0x3A0
    );
    IMPORT_VIRTUAL(
        0x80048F30, 0x3A4, //
        void, VT_0x3A4
    );
    IMPORT_VIRTUAL(
        0x8004A3E0, 0x3A8, //
        void, VT_0x3A8
    );
    IMPORT_VIRTUAL(
        0x8004A7A0, 0x3AC, //
        void, VT_0x3AC
    );
    IMPORT_VIRTUAL(
        0x800588B0, 0x3B0, //
        void, VT_0x3B0
    );
    IMPORT_VIRTUAL(
        0x800588A0, 0x3B4, //
        void, VT_0x3B4
    );
    IMPORT_VIRTUAL(
        0x80058890, 0x3B8, //
        void, VT_0x3B8
    );
    IMPORT_VIRTUAL(
        0x80058880, 0x3BC, //
        void, VT_0x3BC
    );
    IMPORT_VIRTUAL(
        0x80049D60, 0x3C0, //
        void, VT_0x3C0
    );
    IMPORT_VIRTUAL(
        0x80049040, 0x3C4, //
        void, VT_0x3C4
    );
    IMPORT_VIRTUAL(
        0x80048B90, 0x3C8, //
        void, VT_0x3C8
    );
    IMPORT_VIRTUAL(
        0x80053DE0, 0x3CC, //
        void, VT_0x3CC
    );
    IMPORT_VIRTUAL(
        0x80056C20, 0x3D0, //
        void, VT_0x3D0
    );
    IMPORT_VIRTUAL(
        0x80022170, 0x3D4, //
        void, VT_0x3D4
    );
    IMPORT_VIRTUAL(
        0x80058190, 0x3D8, //
        void, VT_0x3D8
    );
    IMPORT_VIRTUAL(
        0x80051360, 0x3DC, //
        void, VT_0x3DC
    );
    IMPORT_VIRTUAL(
        0x80058870, 0x3E0, //
        void, VT_0x3E0
    );
    IMPORT_VIRTUAL(
        0x800581D0, 0x3E4, //
        void, VT_0x3E4
    );
    IMPORT_VIRTUAL(
        0x8002D970, 0x3E8, //
        void, VT_0x3E8
    );
    IMPORT_VIRTUAL(
        0x800581E0, 0x3EC, //
        void, VT_0x3EC
    );
    IMPORT_VIRTUAL(
        0x80057B30, 0x3F0, //
        bool, isNoDamage
    );
    IMPORT_VIRTUAL(
        0x80057B40, 0x3F4, //
        bool, setDamage, dActor_c* source, DamageType_e type
    );
    IMPORT_VIRTUAL(
        0x80057B50, 0x3F8, //
        void, VT_0x3F8
    );
    IMPORT_VIRTUAL(
        0x80057B70, 0x3FC, //
        void, VT_0x3FC
    );
    IMPORT_VIRTUAL(
        0x80057B80, 0x400, //
        void, VT_0x400
    );
    IMPORT_VIRTUAL(
        0x80057B90, 0x404, //
        void, VT_0x404
    );
    IMPORT_VIRTUAL(
        0x80048C30, 0x408, //
        void, VT_0x408
    );
    IMPORT_VIRTUAL(
        0x80051350, 0x40C, //
        void, VT_0x40C
    );
    IMPORT_VIRTUAL(
        0x80057C30, 0x410, //
        void, VT_0x410
    );
    IMPORT_VIRTUAL(
        0x8004C990, 0x414, //
        void, VT_0x414
    );
    IMPORT_VIRTUAL(
        0x80058860, 0x418, //
        void, VT_0x418
    );
    IMPORT_VIRTUAL(
        0x80058850, 0x41C, //
        void, VT_0x41C
    );
    IMPORT_VIRTUAL(
        0x80048C70, 0x420, //
        void, VT_0x420
    );
    IMPORT_VIRTUAL(
        0x800576A0, 0x424, //
        void, VT_0x424
    );
    IMPORT_VIRTUAL(
        0x80057760, 0x428, //
        void, VT_0x428
    );
    IMPORT_VIRTUAL(
        0x800577D0, 0x42C, //
        void, VT_0x42C
    );
    IMPORT_VIRTUAL(
        0x80057810, 0x430, //
        void, VT_0x430
    );
    IMPORT_VIRTUAL(
        0x80058050, 0x434, //
        void, VT_0x434
    );
    IMPORT_VIRTUAL(
        0x800580B0, 0x438, //
        void, VT_0x438
    );
    IMPORT_VIRTUAL(
        0x80058280, 0x43C, //
        void, VT_0x43C
    );
    IMPORT_VIRTUAL(
        0x800582A0, 0x440, //
        void, VT_0x440
    );
    IMPORT_VIRTUAL(
        0x80047480, 0x444, //
        void, VT_0x444
    );
    IMPORT_VIRTUAL(
        0x80047490, 0x448, //
        void, VT_0x448
    );
    IMPORT_VIRTUAL(
        0x8004C470, 0x44C, //
        void, VT_0x44C
    );
    IMPORT_VIRTUAL(
        0x80057B60, 0x450, //
        bool, setDamage2, dActor_c* source, DamageType_e type
    );

    cGCT_IMPORT_ATTR( //
        0x8004B290
    ) bool checkSlip();

    cGCT_IMPORT_ATTR( //
        0x80056CF0
    ) bool isStatus(int flag);

    cGCT_IMPORT_ATTR( //
        0x80057E70
    ) void playSound(u32 sound, int param_2);

    FILL(0x394, 0x10D4);
    u32 m_0x10D4;
    FILL(0x10D8, 0x14D4);
};

static_assert(sizeof(daPlBase_c) == 0x14D4);