#pragma once

namespace m3d
{

cGCT_IMPORT_DECL(0x8016A150);
cGCT_IMPORT_DECL(0x8016A170);
cGCT_IMPORT_DECL(0x8016A1D0);
cGCT_IMPORT_DECL(0x8016A220);

class scnLeaf_c
{
public:
    // Vtable 0x803299B8

    // Padding for CW compatibility
    virtual void _0() asm("0");
    virtual void _4() asm("0");

    cGCT_IMPORT_ATTR( //
        0x8016A150
    ) scnLeaf_c();

    cGCT_IMPORT_ATTR( //
        0x8016A170
    ) virtual void __dt(int type);

    virtual int getType() const = 0;

    cGCT_IMPORT_ATTR( //
        0x8016A1D0
    ) virtual void remove();

    cGCT_IMPORT_ATTR( //
        0x8016A220
    ) virtual bool entry();

    void* mScnObj;
};

static_assert(sizeof(scnLeaf_c) == 0x8);

} // namespace m3d