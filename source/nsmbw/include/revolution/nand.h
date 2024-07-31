#pragma once

#include <types.h>

extern "C" {

#define NAND_CODE_OK 0
#define NAND_CODE_BUSY -3
#define NAND_CODE_EXISTS -6
#define NAND_CODE_NOT_EXIST -12

#define NAND_PERM_READ 0x10
#define NAND_PERM_WRITE 0x20
#define NAND_PERM_READ_WRITE (NAND_PERM_READ | NAND_PERM_WRITE)

#define NAND_MODE_READ 1
#define NAND_MODE_WRITE 2
#define NAND_MODE_RW (NAND_MODE_READ | NAND_MODE_WRITE)

#define NAND_SEEK_SET 0
#define NAND_SEEK_CUR 1
#define NAND_SEEK_END 2

typedef struct {
    u8 fill[0x8C];
} NANDFileInfo;

typedef struct {
    u8 fill[0xB8];
} NANDCommandBlock;

typedef void(NANDCallback)(s32 result, NANDCommandBlock* block);

cGCT_IMPORT( //
    0x801D96F0
) s32 NANDOpen(const char* path, NANDFileInfo* file, u8 mode);

cGCT_IMPORT( //
    0x801D9990
) s32 NANDClose(NANDFileInfo* file);

cGCT_IMPORT( //
    0x801D8B30
) s32 NANDRead(NANDFileInfo* file, void* data, u32 len);

cGCT_IMPORT( //
    0x801D8D10
) s32 NANDSeek(NANDFileInfo* file, s32 offset, s32 origin);

cGCT_IMPORT( //
    0x801D9180
) s32 NANDGetLength(NANDFileInfo* file, u32* len);

} // extern "C"