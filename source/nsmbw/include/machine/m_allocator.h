#pragma once

#include <cstddef>
#include <egg/core/eggAllocator.h>
#include <egg/core/eggHeap.h>

cGCT_IMPORT_DECL(0x8016A7D0);
cGCT_IMPORT_DECL(0x8016A820);
cGCT_IMPORT_DECL(0x8016A830);
cGCT_IMPORT_DECL(0x8016A880);

class mAllocator_c : public EGG::Allocator
{
public:
    mAllocator_c();
    // vtable at 0x10

    /* VT+0x08 */
    // virtual ~mAllocator_c();

    cGCT_IMPORT_ATTR( //
        0x8016A7D0
    ) bool attach(EGG::Heap* heap, int align);

    /* VT+0x0C */
    cGCT_IMPORT_ATTR( //
        0x8016A820
    ) void* alloc(u32 size); // override

    /* VT+0x10 */
    cGCT_IMPORT_ATTR( //
        0x8016A830
    ) void free(void* block); // override
};

class mHeapAllocator_c : public mAllocator_c
{
public:
    cGCT_IMPORT_ATTR( //
        0x8016A880
    ) mHeapAllocator_c();
    // vtable at 0x10
    /* 0x08 */ // virtual ~mHeapAllocator_c();
    /* 0x0C */ // virtual void* alloc(u32 size);  // see mAlloctor::alloc
    /* 0x10 */ // virtual void free(void* block); // see mAlloctor::free
    bool replaceWithNewFrmHeap(s32 size, EGG::Heap* newHeap, char* heapName, s32 align, u32 unk);
    bool replaceWithNewExpHeap(s32 size, EGG::Heap* newHeap, char* heapName, s32 align, u32 unk);
    void destroyHeap();
    s32 adjustFrmHeap();
    s32 adjustExpHeap();
    bool createNewTempFrmHeap(s32 size, EGG::Heap* newHeap, char* heapName, s32 align, u32 unk);
    void adjustFrmHeapRestoreCurrent();
};

void* operator new[](size_t size, mAllocator_c*);