#pragma once

#include "d_player/d_a_player.h"

cGCT_IMPORT_DECL(0x8005F900);

class daPyMng_c : public dBase_c
{
public:
    cGCT_IMPORT_ATTR( //
        0x8005F900
    ) static dAcPy_c* getPlayer(int index);
};