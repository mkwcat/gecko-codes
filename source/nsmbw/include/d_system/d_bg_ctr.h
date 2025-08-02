#pragma once

#include "machine/m_vec.h"

class dActor_c;

class dBg_ctr_c
{
public:
    // ----------------
    // Static Functions
    // ----------------

    /* 0x80080630 */
    static void updateObjBg();

public:
    // ------------
    // Member Data
    // ------------

    /* 0x00 */ dActor_c* mpOwner;
    /* 0x04 */ dBg_ctr_c* mpNext;
    /* 0x08 */ dBg_ctr_c* mpPrev;
    FILL(0x0C, 0x60);

    /* 0x60 */ mVec2_c mPoint[4];

public:
    // -----------
    // Static Data
    // -----------

    static dBg_ctr_c* mEntryN asm("PORT(0x8042A0C0)");
    static dBg_ctr_c* mEntryB asm("PORT(0x8042A0C4)");
};