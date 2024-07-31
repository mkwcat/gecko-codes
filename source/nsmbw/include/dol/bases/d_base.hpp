#pragma once

#include <dol/framework/f_base.hpp>

/**
 * An extension of fBase_c with base kind and name strings. All bases should
 * inherit from this class, not from fBase_c.
 */

class dBase_c : public fBase_c
{
public:
    dBase_c();

    DECL_VIRTUAL_N(int, preCreate) override;
    DECL_VIRTUAL_N(void, postCreate, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preDelete) override;
    DECL_VIRTUAL_N(void, postDelete, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preExecute) override;
    DECL_VIRTUAL_N(void, postExecute, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preDraw) override;
    DECL_VIRTUAL_N(void, postDraw, MAIN_STATE_e state) override;

    DECL_VIRTUAL_N(void, __dt, int type) override;

    /**
     * Gets a string describing the kind of this base.
     */
    DECL_VIRTUAL(0x4C, const char*, getKindString) const;

    /* 0x64 */ u32 m_0x64;

    /**
     * A string describing the kind of this base.
     */
    /* 0x68 */ const char* mpKindString;

    /**
     * A string representing the name of the profile this base belongs to.
     */
    /* 0x6C */ const char* mpNameString;
};

static_assert(sizeof(dBase_c) == 0x70);