// [Gecko]
// $Load More Gecko Codes
// *Load a gecko code file from the save data directory and process it using the
// *current codehandler. The purpose is to allow loading more lines of code than
// *the regular codehandler allows.
// *
// *The file it loads is named "SMN?01.gct", where ? is the region code. If on
// *Dolphin, add this in the Gecko Codes tab, and then right click on the game
// *in the game list, select "Open Wii Save Folder", and then create the GCT
// *file in there with the other codes you want.

#include <gct-use-cpp.h>
#include <gct.h>

#include <egg/core/eggHeap.hpp>
#include <revolution/nand.h>

GCT_ASM(
    // clang-format off

    // Dummy instructions to replace later
G_LocalInstrs:
    .long 0x68200001, 0x00000000;
    .long 0x00000000, 0x00000000;

    // clang-format on
) // GCT_ASM

extern EGG::Heap* sDylinkHeap asm("PORT(0x8042A664)");

extern u32 G_LocalInstrs[4];

cGCT_INSERT_B( //
    0x8015BC9C
) bool loadGeckoCodesHook(bool r3)
{
    char* name = (char*) "SMNE01.gct";
    name[3] = *(u8*) 0x80000003;

    NANDFileInfo file;
    s32 result = NANDOpen(name, &file, NAND_MODE_READ);
    if (result != NAND_CODE_OK) {
        return r3;
    }

    u32 length;
    result = NANDGetLength(&file, &length);
    if (result != NAND_CODE_OK) {
        NANDClose(&file);
        return r3;
    }

    if (length < 0x10 || (length % 8) != 0) {
        NANDClose(&file);
        return r3;
    }

    u8* buffer = (u8*) sDylinkHeap->alloc(length + 0x8, 32);
    if (buffer == nullptr) {
        NANDClose(&file);
        return r3;
    }

    result = NANDRead(&file, buffer, length);
    if (result != length) {
        NANDClose(&file);
        sDylinkHeap->free(buffer);
        return r3;
    }

    NANDClose(&file);

    u32* codes = (u32*) buffer;

    if (codes[0] != 0x00D0C0DE || codes[1] != codes[0]) {
        sDylinkHeap->free(buffer);
        return r3;
    }

    u32* end = (u32*) (buffer + length);
    if (end[-2] != 0xF0000000) {
        sDylinkHeap->free(buffer);
        return r3;
    }

    end[-2] = 0x04001848;
    end[-1] = u32(&G_LocalInstrs[4]);
    end[0] = 0x64200000;
    end[1] = 0x00000000;

    G_LocalInstrs[1] = u32(codes) + 0x8;
    G_LocalInstrs[2] = 0x64200000;
    G_LocalInstrs[3] = 0x00000000;
    G_LocalInstrs[0] = 0x04001848;

    return r3;
}