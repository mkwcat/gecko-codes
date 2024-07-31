#pragma once

#include <types.h>

namespace nw4r::ut
{

struct Node {
    void* prev;
    void* next;
};

static_assert(sizeof(Node) == 0x8);

struct List {
    Node* m_head;
    Node* m_tail;
    u16 m_count;
    u16 m_offset;
};

static_assert(sizeof(List) == 0xC);

cGCT_IMPORT( //
    0x80228F10
) void List_Init(List* list, u16 offset);

cGCT_IMPORT( //
    0x80228F30
) void List_Append(List* list, void* obj);

// Not linked in the final game
// void List_Prepend(List* list, void* obj);

cGCT_IMPORT( //
    0x80228FA0
) void List_Insert(List* list, void* target, void* obj);

cGCT_IMPORT( //
    0x802290C0
) void List_Remove(List* list, void* obj);

cGCT_IMPORT( //
    0x80229130
) void* List_GetNext(const List* list, const void* obj);

cGCT_IMPORT( //
    0x80229150
) void* List_GetPrev(const List* list, const void* obj);

cGCT_IMPORT( //
    0x8018A820 // Actually nw4hbm::ut::List_GetNth(nw4hbm::ut::List*, u16) but
               // it's identical to the normal nw4r one
) void* List_GetNth(const List* list, u16 index);

} // namespace nw4r::ut