#pragma once

#include "gct.h"

extern "C" void __eabi()
{
}

#define GCT_USE_CPP

asm("    .section .c0_start\n"
    "    .global _L_TextAddr\n"
    "_L_TextAddr:\n"
    // r15 points to _L_TextAddr
    // Check if we've already done the relocation
    "    lwz     %r3, _L_StoredTextAddr - _L_TextAddr(%r15)\n"
    "    cmplw   %r3, %r15\n"
    "    beqlr+\n"
    "    stw     %r15, _L_StoredTextAddr - _L_TextAddr(%r15)\n"
    // Now setup and run the loop
    "    addi    %r3, %r15, (_G_GOTStart - 4)@l\n"
    "    addi    %r10, %r15, (_G_GOTEnd - 4)@l\n"
    "    addi    %r11, %r15, (_G_FixupEnd - 4)@l\n"
    "    addi    %r12, %r15, (_G_ImportEnd - 4)@l\n"
    "_L_LoopStart:\n"
    "    cmplw   %r3, %r10\n"
    "    bge-    _L_LoopFixupStart\n"
    "    lwzu    %r9, 0x4(%r3)\n"
    "    rlwinm. %r0, %r9, 0, 0x80000000\n"
    "    bne-    _L_LoopStart\n"
    "    add     %r9, %r9, %r15\n"
    "    stw     %r9, 0(%r3)\n"
    "    b       _L_LoopStart\n"
    "_L_LoopFixupStart:\n"
    "    cmplw   %r3, %r11\n"
    "    bge-    _L_LoopImportStart\n"
    "    lwzu    %r9, 0x4(%r3)\n"
    "    lwzx    %r10, %r9, %r15\n"
    "    add     %r10, %r10, %r15\n"
    "    stwx    %r10, %r9, %r15\n"
    "    b       _L_LoopFixupStart\n"
    "_L_LoopImportStart:\n"
    "    cmplw   %r3, %r12\n"
    "    bgelr-\n"
    "    lwzu    %r9, 0x4(%r3)\n"
    "    sub     %r9, %r9, %r3\n"
    "    rlwinm  %r9, %r9, 0, 0x03FFFFFC\n"
    "    oris    %r9, %r9, 0x4800\n"
    "    stw     %r9, 0(%r3)\n"
    "    dcbf    0, %r3\n"
    "    sync\n"
    "    icbi    0, %r3\n"
    "    isync\n"
    "    b       _L_LoopImportStart\n"
    "_L_StoredTextAddr:\n"
    "    .long   0x0\n");

#undef cGCT_INSERT_BX
#define cGCT_INSERT_BX(_ADDRESS, _BRANCH)                                      \
    asm("    .section .gct_data\n"                                             \
        "    .global _GCT_C2_" #_ADDRESS "\n"                                  \
        "_GCT_C2_" #_ADDRESS ":\n"                                             \
                                                                               \
        /* C4 tells build.py to fix up this hook */                            \
        "    .long   0xC4000000 | (PORT(" #_ADDRESS ") & 0x01FFFFFF)\n"        \
        "    .long   0x00000001\n"                                             \
        "    .long   _GCT_Hook_" #_ADDRESS "\n"                                \
        "    " _BRANCH " 0x0\n"                                                \
        "    .size   _GCT_C2_" #_ADDRESS ", . - _GCT_C2_" #_ADDRESS "\n");     \
    /* Hack to place the pointer directly before the next function in the      \
     * .text section. This will be off by one if the function uses the GOT,    \
     * but this can be fixed up in build.py */                                 \
    extern "C" __attribute__((__section__(".insert." #_ADDRESS))               \
    ) void _GCT_Hook_##_ADDRESS()                                              \
    {                                                                          \
        __builtin_unreachable();                                               \
    }                                                                          \
    __attribute__((__section__(".insert." #_ADDRESS)))

#undef cGCT_INSERT_BL
#define cGCT_INSERT_BL(_ADDRESS) cGCT_INSERT_BX(_ADDRESS, "bl")
#undef cGCT_INSERT_B
#define cGCT_INSERT_B(_ADDRESS) cGCT_INSERT_BX(_ADDRESS, "b")

#undef cGCT_INSERT_POINTER
#define cGCT_INSERT_POINTER(_ADDRESS)                                          \
    asm("    .section .gct_data\n"                                             \
        "    .global _GCT_PTR_" #_ADDRESS "\n"                                 \
        "_GCT_PTR_" #_ADDRESS ":\n"                                            \
        "    .long   0x4E00FFFC\n"                                             \
        "    .long   _GCT_Hook_" #_ADDRESS "\n"                                \
        "    .long   0x4C000000\n"                                             \
        "    .long   PORT(" #_ADDRESS ") & 0x01FFFFFF\n"                       \
        "    .size   _GCT_PTR_" #_ADDRESS ", . - _GCT_PTR_" #_ADDRESS "\n");   \
    /* Hack to place the pointer directly before the next function in the      \
     * .text section. This will be off by one if the function uses the GOT,    \
     * but this can be fixed up in build.py */                                 \
    extern "C" __attribute__((__section__(".insert." #_ADDRESS))               \
    ) void _GCT_Hook_##_ADDRESS()                                              \
    {                                                                          \
        __builtin_unreachable();                                               \
    }                                                                          \
    __attribute__((__section__(".insert." #_ADDRESS)))

#undef cGCT_IMPORT
#define cGCT_IMPORT(_ADDRESS)                                                  \
    cGCT_IMPORT_DECL(_ADDRESS) cGCT_IMPORT_ATTR(_ADDRESS)

#undef cGCT_IMPORT_DECL
#define cGCT_IMPORT_DECL(_ADDR_P)                                              \
    extern "C" __attribute__((__section__(".gct_import." #_ADDR_P), __weak__)  \
    ) void _GCT_Decl_##_ADDR_P()                                               \
    {                                                                          \
        asm volatile("    .long   PORT(" #_ADDR_P ")");                        \
        __builtin_unreachable();                                               \
    }

#undef cGCT_IMPORT_ATTR
#define cGCT_IMPORT_ATTR(_ADDR_P)                                              \
    __attribute__((__alias__("_GCT_Decl_" #_ADDR_P), __weak__))

extern "C" void _GCT_Blr()
{
}

#define cGCT_STUB __attribute__((__alias__("_GCT_Blr")))

#define FILL(_START, _END) u8 _##_START[_END - _START]

#define ASM_(_CODE...) asm volatile(#_CODE)
#define ASM(_CODE...) ASM_(_CODE)

#define Read32(_ADDR) (*(volatile unsigned int*) (_ADDR))
#define Write32(_ADDR, _VALUE)                                                 \
    (*(volatile unsigned int*) (_ADDR) = (unsigned int) (_VALUE))
#define Read16(_ADDR) (*(volatile unsigned short*) (_ADDR))
#define Write16(_ADDR, _VALUE)                                                 \
    (*(volatile unsigned short*) (_ADDR) = (unsigned short) (_VALUE))
#define Read8(_ADDR) (*(volatile unsigned char*) (_ADDR))
#define Write8(_ADDR, _VALUE)                                                  \
    (*(volatile unsigned char*) (_ADDR) = (unsigned char) (_VALUE))

#define Read32O(_ADDR, _OFFSET)                                                \
    (*(volatile unsigned int*) ((unsigned int) (_ADDR) +                       \
                                (unsigned int) (_OFFSET)))
#define Write32O(_ADDR, _OFFSET, _VALUE)                                       \
    (*(volatile unsigned int*) ((unsigned int) (_ADDR) +                       \
                                (unsigned int) (_OFFSET)) =                    \
         (unsigned int) (_VALUE))
#define Read16O(_ADDR, _OFFSET)                                                \
    (*(volatile unsigned short*) ((unsigned int) (_ADDR) +                     \
                                  (unsigned int) (_OFFSET)))
#define Write16O(_ADDR, _OFFSET, _VALUE)                                       \
    (*(volatile unsigned short*) ((unsigned int) (_ADDR) +                     \
                                  (unsigned int) (_OFFSET)) =                  \
         (unsigned short) (_VALUE))
#define Read8O(_ADDR, _OFFSET)                                                 \
    (*(volatile unsigned char*) ((unsigned int) (_ADDR) +                      \
                                 (unsigned int) (_OFFSET)))
#define Write8O(_ADDR, _OFFSET, _VALUE)                                        \
    (*(volatile unsigned char*) ((unsigned int) (_ADDR) +                      \
                                 (unsigned int) (_OFFSET)) =                   \
         (unsigned char) (_VALUE))

#define Mask32(_ADDR, _CLEAR, _SET)                                            \
    Write32(_ADDR, (Read32(_ADDR) & ~(_CLEAR)) | (_SET))
#define Mask16(_ADDR, _CLEAR, _SET)                                            \
    Write16(_ADDR, (Read16(_ADDR) & ~(_CLEAR)) | (_SET))
#define Mask8(_ADDR, _CLEAR, _SET)                                             \
    Write8(_ADDR, (Read8(_ADDR) & ~(_CLEAR)) | (_SET))

#define Mask32O(_ADDR, _OFFSET, _CLEAR, _SET)                                  \
    Write32O(_ADDR, _OFFSET, (Read32O(_ADDR, _OFFSET) & ~(_CLEAR)) | (_SET))
#define Mask16O(_ADDR, _OFFSET, _CLEAR, _SET)                                  \
    Write16O(_ADDR, _OFFSET, (Read16O(_ADDR, _OFFSET) & ~(_CLEAR)) | (_SET))
#define Mask8O(_ADDR, _OFFSET, _CLEAR, _SET)                                   \
    Write8O(_ADDR, _OFFSET, (Read8O(_ADDR, _OFFSET) & ~(_CLEAR)) | (_SET))

#define Clear32(_ADDR, _CLEAR) Write32(_ADDR, Read32(_ADDR) & ~(_CLEAR))
#define Clear16(_ADDR, _CLEAR) Write16(_ADDR, Read16(_ADDR) & ~(_CLEAR))
#define Clear8(_ADDR, _CLEAR) Write8(_ADDR, Read8(_ADDR) & ~(_CLEAR))

#define Clear32O(_ADDR, _OFFSET, _CLEAR)                                       \
    Write32O(_ADDR, _OFFSET, Read32O(_ADDR, _OFFSET) & ~(_CLEAR))
#define Clear16O(_ADDR, _OFFSET, _CLEAR)                                       \
    Write16O(_ADDR, _OFFSET, Read16O(_ADDR, _OFFSET) & ~(_CLEAR))
#define Clear8O(_ADDR, _OFFSET, _CLEAR)                                        \
    Write8O(_ADDR, _OFFSET, Read8O(_ADDR, _OFFSET) & ~(_CLEAR))

#define Set32(_ADDR, _SET) Write32(_ADDR, Read32(_ADDR) | (_SET))
#define Set16(_ADDR, _SET) Write16(_ADDR, Read16(_ADDR) | (_SET))
#define Set8(_ADDR, _SET) Write8(_ADDR, Read8(_ADDR) | (_SET))

#define Set32O(_ADDR, _OFFSET, _SET)                                           \
    Write32O(_ADDR, _OFFSET, Read32O(_ADDR, _OFFSET) | (_SET))
#define Set16O(_ADDR, _OFFSET, _SET)                                           \
    Write16O(_ADDR, _OFFSET, Read16O(_ADDR, _OFFSET) | (_SET))
#define Set8O(_ADDR, _OFFSET, _SET)                                            \
    Write8O(_ADDR, _OFFSET, Read8O(_ADDR, _OFFSET) | (_SET))

#ifdef NSMBW
#  include "nsmbw/gct-import-decl.hpp"
#endif