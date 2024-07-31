#pragma once

#include <types.h>

namespace nw4r::lyt
{

namespace res
{

struct SectionHeader {
    /* 0x00 */ u32 magic;
    /* 0x04 */ u32 size;
};

static_assert(sizeof(SectionHeader) == 0x8);

struct TextureList {
    // https://wiki.tockdom.com/wiki/BRLYT_(File_Format)#txl1
    /* 0x00 */ SectionHeader header;
    /* 0x08 */ u16 numEntries;
    /* 0x0A */ u16 pad;
};

static_assert(sizeof(TextureList) == 0xC);

struct MaterialList {
    // https://wiki.tockdom.com/wiki/BRLYT_(File_Format)#mat1
    /* 0x00 */ SectionHeader header;
    /* 0x08 */ u16 numEntries;
    /* 0x0A */ u16 pad;
};

static_assert(sizeof(MaterialList) == 0xC);

struct Material {
    /* 0x00 */ char materialName[0x14];
    /* 0x14 */ s16 foreColor[4]; // RGBA
    /* 0x1C */ s16 backColor[4]; // RGBA
    /* 0x24 */ s16 colorReg3[4]; // RGBA
    /* 0x2C */ u8 tevColor1[4]; // RGBA
    /* 0x30 */ u8 tevColor2[4]; // RGBA
    /* 0x34 */ u8 tevColor3[4]; // RGBA
    /* 0x38 */ u8 tevColor4[4]; // RGBA
    /* 0x3C */ u32 flags;
};

static_assert(sizeof(Material) == 0x40);

struct FontList {
    // https://wiki.tockdom.com/wiki/BRLYT_(File_Format)#fnl1
    /* 0x00 */ SectionHeader header;
    /* 0x08 */ u16 numEntries;
    /* 0x0A */ u16 pad;
};

static_assert(sizeof(FontList) == 0xC);

struct Font {
    /* 0x00 */ u32 fontNameOffset;
    /* 0x04 */ u32 pad;
};

static_assert(sizeof(Font) == 0x8);

struct Pane {
    // https://wiki.tockdom.com/wiki/BRLYT_(File_Format)#pan1
    /* 0x00 */ SectionHeader header;
    /* 0x08 */ u8 flags;
    /* 0x09 */ u8 originType;
    /* 0x0A */ u8 alpha;
    /* 0x0B */ u8 pad_0xB;
    /* 0x0C */ char name[0x10];
    /* 0x1C */ char userData[0x8];
    /* 0x24 */ float translation[3];
    /* 0x30 */ float rotation[3];
    /* 0x3C */ float scale[2];
    /* 0x44 */ float width;
    /* 0x48 */ float height;
};

static_assert(sizeof(Pane) == 0x4C);

struct TextBox {
    // https://wiki.tockdom.com/wiki/BRLYT_(File_Format)#txt1
    /* 0x00 */ Pane pane;
    /* 0x4C */ u16 stringSize;
    /* 0x4E */ u16 maxStringSize;
    /* 0x50 */ u16 matIndex;
    /* 0x52 */ u16 fontIndex;
    /* 0x54 */ u8 stringOrigin;
    /* 0x55 */ u8 lineAlignment;
    /* 0x56 */ u16 pad_0x56;
    /* 0x58 */ u32 textOffset;
    /* 0x5C */ u8 topColor[4]; // RGBA
    /* 0x60 */ u8 bottomColor[4]; // RGBA
    /* 0x64 */ float fontSizeX;
    /* 0x68 */ float fontSizeY;
    /* 0x6C */ float characterSize;
    /* 0x70 */ float lineSize;
};

static_assert(sizeof(TextBox) == 0x74);

} // namespace res

struct ResBlockSet {
    res::TextureList* txl1;
    res::FontList* fnl1;
    res::MaterialList* mat1;
    void* resourceAccessor;
};

} // namespace nw4r::lyt