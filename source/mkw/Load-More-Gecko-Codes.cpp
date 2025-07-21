// [Gecko]
// $Load More Gecko Codes
// *Load a gecko code file from the save data directory and process it using the
// *current codehandler. The purpose is to allow loading more lines of code than
// *the regular codehandler allows.
// *
// *The file it loads is named "RMC?01.gct", where ? is the region code. If on
// *Dolphin, add this in the Gecko Codes tab, and then right click on the game
// *in the game list, select "Open Wii Save Folder", and then create the GCT
// *file in there with the other codes you want.

#include <gct-use-cpp.h>
#include <gct.h>

#include <revolution/nand.h>
#include <revolution/os.h>

GCT_ASM(
    // clang-format off

    // Dummy instructions to replace later
G_LocalInstrs:
    .long 0x68200001, 0x00000000;
    .long 0x00000000, 0x00000000;

    // clang-format on
) // GCT_ASM

extern u32 G_LocalInstrs[4];

#if 0
cGCT_IMPORT( //
    0x8000A188
) s32 ModuleLinker_linkModule(void* moduleLinker, EGG::Heap* heap, u32 param5);

cGCT_INSERT_BL( //
    0x800074C4
) s32 loadGeckoCodesHook(void* moduleLinker, EGG::Heap* heap, u32 param5)
{
    s32 linkResult = ModuleLinker_linkModule(moduleLinker, heap, param5);

    char* name = (char*) "RMCE01.gct";
    name[3] = *(u8*) 0x80000003;

    NANDFileInfo file;
    s32 result = NANDOpen(name, &file, NAND_MODE_READ);
    if (result != NAND_CODE_OK) {
        return linkResult;
    }

    u32 length;
    result = NANDGetLength(&file, &length);
    if (result != NAND_CODE_OK) {
        NANDClose(&file);
        return linkResult;
    }

    if (length < 0x10 || (length % 8) != 0) {
        NANDClose(&file);
        return linkResult;
    }

    u8* buffer = (u8*) heap->alloc(length + 0x8, 32);
    if (buffer == nullptr) {
        NANDClose(&file);
        return linkResult;
    }

    result = NANDRead(&file, buffer, length);
    if (result != length) {
        NANDClose(&file);
        heap->free(buffer);
        return linkResult;
    }

    NANDClose(&file);

    u32* codes = (u32*) buffer;

    if (codes[0] != 0x00D0C0DE || codes[1] != codes[0]) {
        heap->free(buffer);
        return linkResult;
    }

    u32* end = (u32*) (buffer + length);
    if (end[-2] != 0xF0000000) {
        heap->free(buffer);
        return linkResult;
    }

    end[-2] = 0x04001848;
    end[-1] = u32(&G_LocalInstrs[4]);
    end[0] = 0x64200000;
    end[1] = 0x00000000;

    G_LocalInstrs[1] = u32(codes) + 0x8;
    G_LocalInstrs[2] = 0x64200000;
    G_LocalInstrs[3] = 0x00000000;
    G_LocalInstrs[0] = 0x04001848;

    return linkResult;
}
#endif

cGCT_IMPORT( //
    0x80008EF0
) int RKSystem_main(int argc, char** argv);

static constexpr u32 ROUND_DOWN(u32 value, u32 align)
{
    return value & ~(align - 1);
}

// From mkw-sp
static inline u8* OSAllocFromMEM1ArenaHi(u32 size, u32 align)
{
    u32 result = ROUND_DOWN((u32) OSGetMEM1ArenaHi(), align);

    OSSetMEM1ArenaHi((void*) ROUND_DOWN(result - size, align));
    return (u8*) ROUND_DOWN(result - size, align);
}

static inline void OSFreeToMEM1ArenaHi(u8* ptr, u32 size, u32 align)
{
    OSSetMEM1ArenaHi((void*) ROUND_DOWN((u32) ptr + size, align));
}

cGCT_INSERT_B( //
    0x8000B6B0
) int loadGeckoCodesHook(int argc, char** argv)
{
    char* name = (char*) "RMC?01.gct";
    name[3] = *(u8*) 0x80000003;

    NANDFileInfo file;
    s32 result = NANDOpen(name, &file, NAND_MODE_READ);
    if (result != NAND_CODE_OK) {
        return RKSystem_main(argc, argv);
    }

    u32 length;
    result = NANDGetLength(&file, &length);
    if (result != NAND_CODE_OK) {
        NANDClose(&file);
        return RKSystem_main(argc, argv);
    }

    if (length < 0x10 || (length % 8) != 0) {
        NANDClose(&file);
        return RKSystem_main(argc, argv);
    }

    u8* buffer = OSAllocFromMEM1ArenaHi(length + 0x8, 32);
    if (buffer == nullptr) {
        NANDClose(&file);
        return RKSystem_main(argc, argv);
    }

    result = NANDRead(&file, buffer, length);
    if (result != length) {
        NANDClose(&file);
        OSFreeToMEM1ArenaHi(buffer, length + 0x8, 32);
        return RKSystem_main(argc, argv);
    }

    NANDClose(&file);

    u32* codes = (u32*) buffer;

    if (codes[0] != 0x00D0C0DE || codes[1] != codes[0]) {
        OSFreeToMEM1ArenaHi(buffer, length + 0x8, 32);
        return RKSystem_main(argc, argv);
    }

    u32* end = (u32*) (buffer + length);
    if (end[-2] != 0xF0000000) {
        OSFreeToMEM1ArenaHi(buffer, length + 0x8, 32);
        return RKSystem_main(argc, argv);
    }

    end[-2] = 0x04001848;
    end[-1] = u32(&G_LocalInstrs[4]);
    end[0] = 0x64200000;
    end[1] = 0x00000000;

    G_LocalInstrs[1] = u32(codes) + 0x8;
    G_LocalInstrs[2] = 0x64200000;
    G_LocalInstrs[3] = 0x00000000;
    G_LocalInstrs[0] = 0x04001848;

    return RKSystem_main(argc, argv);
}