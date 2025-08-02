#pragma once

#include "d_system/d_a_player_base.h"

class dAcPy_c : public daPlBase_c
{
public:
    IMPORT_VIRTUAL_N(
        0x80139340, //
        void, setBalloonInDispOut, int
    )
    override;

    IMPORT_VIRTUAL_N(
        0x801463E0, //
        bool, setDamage2, dActor_c* source, DamageType_e type
    )
    override;

    // Added for Death-Messages
    void setDeathMessage(const char* message, const char* enemy);

    // Used for functions outside of Death-Messages to override the current
    // message. Uses function alignment padding to add a new global variable.
    static const char* sDeathMessageOverride asm("PORT(0x801463DC)");

    void setBalloonInDispOut_Override(int type);
    bool setDamage2_Override(dActor_c* source, DamageType_e type);

    s32 mCharacter;
};