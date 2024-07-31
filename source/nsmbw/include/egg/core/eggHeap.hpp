#pragma once

#include <types.h>

cGCT_IMPORT( //
    0x802B9350
) void* __nw(unsigned long size);

cGCT_IMPORT( //
    0x802B93C0
) void __dl(void* ptr);

void* operator new(unsigned int size)
{
    return __nw(size);
}

void operator delete(void* ptr, unsigned int size)
{
    __dl(ptr);
}

void operator delete(void* ptr)
{
    __dl(ptr);
}

namespace EGG
{

class Heap
{
public:
    // Pad for CW compatibility
    virtual void _0();
    virtual void _4();

    virtual void VT_0x08();
    virtual void VT_0x0C();
    virtual void VT_0x10();
    virtual void* alloc(u32 size, u32 align);
    virtual void free(void* ptr);
};

} // namespace EGG