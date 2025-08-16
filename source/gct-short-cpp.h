#pragma once

#include "gct.h"
#include <types.h>

asm("    .section .c0_start\n"
    "    .global _L_TextAddr\n"
    "_L_TextAddr:\n"
    "    blr\n");