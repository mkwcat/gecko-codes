#pragma once

#include <types.h>

namespace nw4r::lyt
{

class Pane
{
public:
    // Pad for CW compatibility
    virtual void _0();
    virtual void _4();

    FILL(0x4, 0x2C);

    /* 0x2C */ float m_translation[3];
    /* 0x38 */ float m_rotation[3];
    /* 0x44 */ float m_scale[2];
    /* 0x4C */ float m_width;
    /* 0x50 */ float m_height;
    /* 0x54 */ float m_localMtx[3][4];
    /* 0x84 */ float m_globalMtx[3][4];
    FILL(0xB4, 0xB8);
    /* 0xB8 */ u8 m_alpha;

    FILL(0xB9, 0xD8);
};

static_assert(sizeof(Pane) == 0xD8);

} // namespace nw4r::lyt