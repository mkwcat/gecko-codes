#pragma once

#include "gx/GXEnum.h"
#include "gx/GXStruct.h"

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C1EF0)
) void GXInit(void* data, u32 dataSize);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C3900)
) void GXSetVtxDesc(u32 param_1, u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C41B0)
) void GXClearVtxDesc();

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C41F0)
) void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C4B60)
) void GXSetNumTexGens(u32 num);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C4900)
) void GXInvalidateVtxCache();

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C4910)
) void GXSetTexCoordGen2(GXTexCoordID dst_coord, GXTexGenType func, GXTexGenSrc src_param, u32 mtx, GXBool normalize, u32 postmtx);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C4FE0)
) void GXDrawDone();

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C56B0)
) void GXBegin(u32 param_1, u32 formatIndex, u32 param_3);

#define GXEnd() (void) 0

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C58E0)
) void GXSetLineWidth(u32 param_1, GXTextureOffset param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C5920)
) void GXSetPointSize(u32 param_1, u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C59A0)
) void GXSetCullMode(u32 param_1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C5D00)
) float GXGetYScaleFactor(u16 efbHeight, u16 xfbHeight);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C5A60)
) void GXSetDispCopySrc(u32 param_1, u32 param_2, u16 fbWidth, u16 efbHeight);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C5AE0)
) void GXSetDispCopyDst(u16 fbWidth, u16 xfbLines);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C5F30)
) u16 GXSetDispCopyYScale(float factor);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6000)
) void GXSetCopyClear(GXColor color, u32 z);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6080)
) void GXSetCopyFilter(u8 aa, u8* sample, u32 param_3, u8* vertFilter);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6290)
) void GXCopyDisp(void* xfb, u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6C00)
) void GXSetNumChans(u32 num);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6A40)
) void GXSetChanAmbColor(GXChannelID chan, GXColor color);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6B20)
) void GXSetChanMatColor(GXChannelID chan, GXColor color);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6C30)
) void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src, GXLightID light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C6ED0)
) void GXInitTexObj(GXTexObj* obj, void* image, u16 width, u16 height, int format, int wrapS, int wrapT, bool mipmap);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C7600)
) void GXLoadTexObj(GXTexObj* obj, int texMapID);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C7800)
) void GXInvalidateTexAll();

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8250)
) void GXSetNumIndStages(u32 num);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8270)
) void GXSetTevDirect(GXTevStageID stage);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8390)
) void GXSetTevOp(GXTevStageID id, GXTevMode mode);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8430)
) void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c, GXTevColorArg d);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8470)
) void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c, GXTevAlphaArg d);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C84B0)
) void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out_reg);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8510)
) void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale, GXBool clamp, GXTevRegID out_reg);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8570)
) void GXSetTevColor(GXTevRegID reg, GXColor color);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8780)
) void GXSetTevSwapModeTable(GXTevSwapSel swapid, GXTevColorChan r, GXTevColorChan g, GXTevColorChan b, GXTevColorChan a);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8800)
) void GXSetAlphaCompare(u8 comp0, u8 ref0, u8 aop, u8 comp1, u8 ref1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C88D0)
) void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8A30)
) void GXSetNumTevStages(u32 num);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8A60)
) void GXSetFog(u8 type, f32 startz, f32 endz, f32 nearz, f32 farz, GXColor color);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8DF0)
) void GXSetFogRangeAdj(u8 enable, u16 center, void* table);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8F00)
) void GXSetBlendMode(u8 type, u8 src_fact, u8 dst_fact, u8 op);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8F50)
) void GXSetColorUpdate(u32 param_1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8F80)
) void GXSetAlphaUpdate(u32 param_1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8FB0)
) void GXSetZMode(u32 param_1, u32 param_2, u32 param_3);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C8FF0)
) void GXSetZCompLoc(GXBool param_1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9020)
) void GXSetPixelFmt(u32 param_1, u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C90D0)
) void GXSetDither(GXBool enable);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9980)
) void GXSetProjection(float mtx[4][4], u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9A80)
) void GXLoadPosMtxImm(float mtx[3][4], u32 param_2);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9BA0)
) void GXSetCurrentMtx(u32 param_1);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9D50)
) void GXSetViewport(float param_1, float param_2, float param_3, float param_4, float param_5, float param_6);

cGCT_IMPORT_MULTI( //
    NSMBW(0x801C9DA0)
) void GXSetScissor(u32 left, u32 top, u32 right, u32 bottom);

extern volatile union _WGPIPE {
    char c;
    short s;
    int i;
    void* p;
    float f;
} WGPIPE asm("0xCC008000");

static void GXPosition2f32(f32 x, f32 y)
{
    WGPIPE.f = x;
    WGPIPE.f = y;
}

static void GXPosition3f32(f32 x, f32 y, f32 z)
{
    WGPIPE.f = x;
    WGPIPE.f = y;
    WGPIPE.f = z;
}

static void GXColor1u32(u32 c)
{
    WGPIPE.i = c;
}

static void GXColor4u8(u8 r, u8 g, u8 b, u8 a)
{
    WGPIPE.c = r;
    WGPIPE.c = g;
    WGPIPE.c = b;
    WGPIPE.c = a;
}

static void GXTexCoord2u16(u16 c1, u16 c2)
{
    WGPIPE.s = c1;
    WGPIPE.s = c2;
}

static void GXTexCoord2f32(f32 c1, f32 c2)
{
    WGPIPE.f = c1;
    WGPIPE.f = c2;
}

static void GXCmd1u8(u8 uc)
{
    WGPIPE.c = uc;
}

static void GXCmd1u16(u16 us)
{
    WGPIPE.s = us;
}

static void GXCmd1u32(u32 ul)
{
    WGPIPE.i = ul;
}
