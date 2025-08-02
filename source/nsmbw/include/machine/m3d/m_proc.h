#pragma once

#include "m_scene_leaf.h"
#include "machine/m_allocator.h"
#include <revolution/types.h>

cGCT_IMPORT_DECL(0x80165110);
cGCT_IMPORT_DECL(0x8000F720);

namespace m3d
{

class proc_c : public scnLeaf_c
{
public:
    cGCT_IMPORT_ATTR( //
        0x80165110
    ) bool create(mAllocator_c* allocator = 0, u32* size = 0);

    cGCT_IMPORT_ATTR( //
        0x8000F720
    ) virtual int getType() const override;

    virtual void drawOpa() = 0;
    virtual void drawXlu() = 0;
};

} // namespace m3d