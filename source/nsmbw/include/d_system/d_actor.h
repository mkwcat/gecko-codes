#pragma once

#include "d_base_actor.h"

class dActor_c : public dBaseActor_c
{
public:
    FILL(0x125, 0x38C);

    /**
     * 0 = Default, 1 = Player, 2 = Yoshi, 3 = Enemy
     */
    /* 0x38C */ u8 mActorType;
    /* 0x38D */ u8 mPlayerNo;
    /* 0x38E */ u8 m_0x38E;
    /* 0x38F */ u8 mLayer;
    /* 0x390 */ u8 m_0x390;
    /* 0x391 */ u8 mOnBackFence;

    // 0x60 - 0xD4
    IMPORT_VIRTUAL(
        0x80065080, 0x60, //
        bool, ActorDrawCullCheck
    );
    IMPORT_VIRTUAL(
        0x80065650, 0x64, //
        void, block_hit_init
    );
    IMPORT_VIRTUAL(
        0x8001D210, 0x68, //
        void, VT_0x68
    );
    IMPORT_VIRTUAL(
        0x8001D200, 0x6C, //
        u8*, getPlrNo
    );
    IMPORT_VIRTUAL(
        0x80065620, 0x70, //
        mVec3_c, getLookatPos
    )
    const;
    IMPORT_VIRTUAL(
        0x8001D1F0, 0x74, //
        bool, isSpinLiftUpEnable
    );
    IMPORT_VIRTUAL(
        0x80065820, 0x78, //
        void, setSpinLiftUpActor, dActor_c* pActor
    );
    IMPORT_VIRTUAL(
        0x8001D1E0, 0x7C, //
        void, VT_0x7C
    );
    IMPORT_VIRTUAL(
        0x80065830, 0x80, //
        void, VT_0x80
    );
    IMPORT_VIRTUAL(
        0x80065850, 0x84, //
        void, VT_0x84
    );
    IMPORT_VIRTUAL(
        0x80065860, 0x88, //
        void, VT_0x88
    );
    IMPORT_VIRTUAL(
        0x80065870, 0x8C, //
        void, VT_0x8C
    );
    IMPORT_VIRTUAL(
        0x80065880, 0x90, //
        void, VT_0x90
    );
    IMPORT_VIRTUAL(
        0x80065AC0, 0x94, //
        void, VT_0x94
    );
    IMPORT_VIRTUAL(
        0x80066080, 0x98, //
        void, VT_0x98
    );
    IMPORT_VIRTUAL(
        0x80066090, 0x9C, //
        void, VT_0x9C
    );
    IMPORT_VIRTUAL(
        0x80065950, 0xA0, //
        void, VT_0xA0
    );
    IMPORT_VIRTUAL(
        0x80065970, 0xA4, //
        void, VT_0xA4
    );
    IMPORT_VIRTUAL(
        0x80065A00, 0xA8, //
        void, VT_0xA8
    );
    IMPORT_VIRTUAL(
        0x80065A40, 0xAC, //
        void, VT_0xAC
    );
    IMPORT_VIRTUAL(
        0x80065660, 0xB0, //
        void, VT_0xB0
    );
    IMPORT_VIRTUAL(
        0x80065B40, 0xB4, //
        void, VT_0xB4
    );
    IMPORT_VIRTUAL(
        0x8001D1D0, 0xB8, //
        void, VT_0xB8
    );
    IMPORT_VIRTUAL(
        0x80065B50, 0xBC, //
        void, VT_0xBC
    );
    IMPORT_VIRTUAL(
        0x80065B60, 0xC0, //
        void, VT_0xC0
    );
    IMPORT_VIRTUAL(
        0x80065B70, 0xC4, //
        void, VT_0xC4
    );
    IMPORT_VIRTUAL(
        0x80065CC0, 0xC8, //
        void, VT_0xC8
    );
    IMPORT_VIRTUAL(
        0x80065DC0, 0xCC, //
        void, VT_0xCC
    );
    IMPORT_VIRTUAL(
        0x80065EC0, 0xD0, //
        void, VT_0xD0
    );
};

static_assert(sizeof(dActor_c) == 0x394);
