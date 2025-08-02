#pragma once

#include <egg/core/eggHeap.h>

namespace mHeap
{

/* 0x8016E630 */
EGG::Heap* setCurrentHeap(EGG::Heap* heap);

extern EGG::Heap* g_gameHeaps[3] asm("PORT(0x80377F48)");

} // namespace mHeap