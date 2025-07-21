#pragma once

#include "gct-use-cpp.h"
#include <types.h>

extern "C" {

struct OSContext {
    // General-purpose registers
    u32 gpr[32];

    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;

    // Floating-point registers
    f64 fpr[32];

    u32 fpscr_pad;
    u32 fpscr;

    // Exception handling registers
    u32 srr0;
    u32 srr1;

    // Context mode
    u16 mode; // since UIMM is 16 bits in PPC
    u16 state; // OR-ed OS_CONTEXT_STATE_*

    // Place Gekko regs at the end so we have minimal changes to
    // existing code
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
};

cGCT_IMPORT_MULTI( //
    MKW(0x801A0504)
) void OSRegisterVersion(const char*);

cGCT_IMPORT_MULTI( //
    MKW(0x801A25D0), NSMBW(0x8015F870)
) void OSReport(const char* format, ...);

cGCT_IMPORT_MULTI( //
    MKW(0x801A65AC), NSMBW(0x801B1280)
) BOOL OSDisableInterrupts();

cGCT_IMPORT_MULTI( //
    MKW(0x801A65D4), NSMBW(0x801B12C0)
) void OSRestoreInterrupts(BOOL enabled);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801B60C0)
) u64 OSGetTime();

cGCT_IMPORT_MULTI( //
    MKW(0x801A2660)
) void OSPanic(const char* file, int line, const char* format, ...);

cGCT_IMPORT_MULTI( //
    MKW(0x801A10A4)
) void* OSGetMEM1ArenaHi(void);

cGCT_IMPORT_MULTI( //
    MKW(0x801A10D4)
) void OSSetMEM1ArenaHi(void* ptr);

#define OS_REPORT(fmt, ...) OSReport("%s:%d: " fmt, __FILE__, __LINE__ __VA_OPT__(, ) __VA_ARGS__)
#define OS_PANIC(msg) OSPanic(__FILE__, __LINE__, msg)
#define ASSERTMSG(exp, msg) (void) ((exp) || (OSPanic(__FILE__, __LINE__, (msg)), 0))
#define ASSERT(cond) ((cond) || (OSPanic(__FILE__, __LINE__, "Failed assertion " #cond), 0))

} // extern "C"