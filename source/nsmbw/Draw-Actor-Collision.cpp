// [Gecko]
// Draw Actor Collision [Ninji]
// *Renders collision boxes for actors in the game. This is an unfinished port from Newer.

// https://github.com/Newer-Team/NewerSMBW/blob/cw/Kamek/src/apDebug.cpp
//
// Copyright (c) 2009-2022 Newer Team
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "revolution/os.h"
#include <gct-use-cpp.h>
#include <gct.h>

#include <d_system/d_actor.h>
#include <d_system/d_allocator.h>
#include <d_system/d_bg_ctr.h>
#include <d_system/d_cc.h>
#include <machine/m3d/m_3d.h>
#include <machine/m3d/m_proc.h>
#include <machine/m_heap.h>
#include <machine/m_vec.h>
#include <new>
#include <nw4r/g3d/Camera.h>
#include <revolution/gx.h>

class APDebugDrawer : public m3d::proc_c
{
public:
    dHeapAllocator_c mAllocator;

    APDebugDrawer();

    void drawMe();
    void drawOpa() override;
    void drawXlu() override;
};

cGCT_INSERT_B( //
    0x80830BD8
) int APDebugDraw()
{
    static APDebugDrawer* s_instance = new APDebugDrawer();

    s_instance->drawMe();

    return 1;
}

APDebugDrawer::APDebugDrawer()
{
    mAllocator.attach(mHeap::g_gameHeaps[0], 0x20);
    create(&mAllocator);
}

void APDebugDrawer::drawMe()
{
    entry();
}

void APDebugDrawer::drawOpa()
{
    drawXlu();
}

void APDebugDrawer::drawXlu()
{
    GXClearVtxDesc();

    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

    GXSetNumIndStages(0);
    for (int i = 0; i < 0x10; i++) {
        GXSetTevDirect(GXTevStageID(i));
    }

    GXSetNumChans(1);
    GXSetChanCtrl(
        GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE
    );
    GXSetChanAmbColor(GX_COLOR0A0, (GXColor) {255, 255, 255, 255});
    GXSetChanMatColor(GX_COLOR0A0, (GXColor) {255, 255, 255, 255});
    GXSetNumTexGens(0);

    GXSetNumTevStages(1);
    GXSetNumIndStages(0);

    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);

    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_RASA, GX_CA_ZERO);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    GXSetZCompLoc(GX_FALSE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_SET);
    GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXSetFog(GX_FOG_NONE, 0, 0, 0, 0, (GXColor) {0, 0, 0, 0});
    GXSetFogRangeAdj(GX_FALSE, 0, 0);

    GXSetCullMode(GX_CULL_NONE);

    GXSetDither(GX_TRUE);
    GXSetLineWidth(18, GX_TO_ZERO);

    GXSetTevColor(GX_TEVREG0, (GXColor) {255, 255, 255, 255});
    GXSetTevColor(GX_TEVREG1, (GXColor) {0, 0, 0, 255});

    nw4r::g3d::Camera cam(m3d::getCamera(m3d::getCurrentCameraID()));
    Mtx matrix;
    cam.GetCameraMtx(&matrix);
    GXLoadPosMtxImm(matrix, 0);
    GXSetCurrentMtx(0);

    OSReport(
        "Camera matrix: %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",
        matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3], matrix[1][0], matrix[1][1],
        matrix[1][2], matrix[1][3], matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
    );

    dCc_c* ptr = dCc_c::mEntryN;

    while (ptr != nullptr) {
        dCc_c& cc = *ptr;
        u32 uptr = (u32) ptr;
        u8 r = (uptr >> 16) & 0xFF;
        u8 g = (uptr >> 8) & 0xFF;
        u8 b = uptr & 0xFF;
        u8 a = 0xFF;

        float centerX = cc.mpOwner->mPos.x + cc.mCcData.mOffsetX;
        float centerY = cc.mpOwner->mPos.y + cc.mCcData.mOffsetY;

        float tlX = centerX - cc.mCcData.mWidth;
        float tlY = centerY + cc.mCcData.mHeight;
        float trX = centerX + cc.mCcData.mWidth;
        float trY = tlY;

        float blX = tlX;
        float blY = centerY - cc.mCcData.mHeight;
        float brX = trX;
        float brY = blY;

        switch (cc.mShape) {
        default:
            break;

        case dCc_c::CC_SHAPE_e::CC_SHAPE_DAIKEI_UD: // Vertical trapezoid
            tlY = centerY + cc.mTrpOffsets[0];
            trY = centerY + cc.mTrpOffsets[1];
            blY = centerY + cc.mTrpOffsets[2];
            brY = centerY + cc.mTrpOffsets[3];
            break;
        case dCc_c::CC_SHAPE_e::CC_SHAPE_DAIKEI_LR: // Horizontal trapezoid
            tlX = centerX + cc.mTrpOffsets[0];
            trX = centerX + cc.mTrpOffsets[1];
            blX = centerX + cc.mTrpOffsets[2];
            brX = centerX + cc.mTrpOffsets[3];
            break;
        }

        OSReport(
            "Drawing collider %08X: %f, %f, %f, %f, %f, %f, %f, %f\n", ptr, tlX, tlY, trX, trY, blX,
            blY, brX, brY
        );

        GXBegin(GX_LINES, GX_VTXFMT0, 10);

        // Top
        GXPosition3f32(tlX, tlY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Left
        GXPosition3f32(tlX, tlY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Right
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(brX, brY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Bottom
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(brX, brY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Diagonal
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);

        GXEnd();

        ptr = ptr->mpNext;
    }

    dBg_ctr_c* p = dBg_ctr_c::mEntryN;

    while (p) {
        u32 uptr = (u32) p;
        u8 r = (uptr >> 16) & 0xFF;
        u8 g = (uptr >> 8) & 0xFF;
        u8 b = uptr & 0xFF;
        u8 a = 0xFF;

        GXBegin(GX_LINES, GX_VTXFMT0, 10);

        float tlX = p->mPoint[0].x;
        float tlY = p->mPoint[0].y;
        float trX = p->mPoint[3].x;
        float trY = p->mPoint[3].y;
        float blX = p->mPoint[1].x;
        float blY = p->mPoint[1].y;
        float brX = p->mPoint[2].x;
        float brY = p->mPoint[2].y;

        // Top
        GXPosition3f32(tlX, tlY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Left
        GXPosition3f32(tlX, tlY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Right
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(brX, brY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Bottom
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(brX, brY, 9000.0f);
        GXColor4u8(r, g, b, a);

        // Diagonal
        GXPosition3f32(trX, trY, 9000.0f);
        GXColor4u8(r, g, b, a);
        GXPosition3f32(blX, blY, 9000.0f);
        GXColor4u8(r, g, b, a);

        GXEnd();

        p = p->mpNext;
    }

#if 0
    // Basic Colliders
    BasicCollider* bc = BasicCollider::globalListHead;
    while (bc) {
        u32 uptr = (u32) bc;
        u8 r = (uptr >> 16) & 0xFF;
        u8 g = (uptr >> 8) & 0xFF;
        u8 b = uptr & 0xFF;
        u8 a = 0xFF;

        switch (bc->type) {
        case 0:
        case 2:
            GXBegin(GX_LINES, GX_VTXFMT0, 2);
            GXPosition3f32(bc->leftX, bc->leftY, 9000.0f);
            GXColor4u8(r, g, b, a);
            GXPosition3f32(bc->rightX, bc->rightY, 9000.0f);
            GXColor4u8(r, g, b, a);
            GXEnd();
            break;
        }

        bc = bc->next;
    }

    // Now, the hardest one.. CollisionMgr_c!
    fBase_c* fb = 0;
    while ((fb = fBase_c::searchByBaseType(2, fb))) {
        u8* testMe = ((u8*) fb) + 0x1EC;
        if (*((u32*) testMe) != 0x8030F6D0)
            continue;

        u32 uptr = (u32) fb;
        u8 r = u8((uptr >> 16) & 0xFF) + 0x20;
        u8 g = u8((uptr >> 8) & 0xFF) - 0x30;
        u8 b = u8(uptr & 0xFF) + 0x80;
        u8 a = 0xFF;

        dStageActor_c* ac = (dStageActor_c*) fb;

        sensorBase_s* sensors[4] = {
            ac->collMgr.pBelowInfo, ac->collMgr.pAboveInfo,
            ac->collMgr.pAdjacentInfo, ac->collMgr.pAdjacentInfo};

        for (int i = 0; i < 4; i++) {
            sensorBase_s* s = sensors[i];
            if (s == nullptr) {
                continue;
            }

            float mult = (i == 3) ? -1.0f : 1.0f;

            switch (s->flags & SENSOR_TYPE_MASK) {
            case SENSOR_POINT:
                GXBegin(GX_POINTS, GX_VTXFMT0, 1);
                GXPosition3f32(
                    ac->pos.x + (mult * (s->asPoint()->x / 4096.0f)),
                    ac->pos.y + (s->asPoint()->y / 4096.0f), 8005.0f
                );
                GXColor4u8(r, g, b, a);
                GXEnd();
                break;

            case SENSOR_LINE:
                GXBegin(GX_LINES, GX_VTXFMT0, 2);
                if (i < 2) {
                    GXPosition3f32(
                        ac->pos.x + (s->asLine()->lineA / 4096.0f),
                        ac->pos.y + (s->asLine()->distanceFromCenter / 4096.0f),
                        8005.0f
                    );
                    GXColor4u8(r, g, b, a);
                    GXPosition3f32(
                        ac->pos.x + (s->asLine()->lineB / 4096.0f),
                        ac->pos.y + (s->asLine()->distanceFromCenter / 4096.0f),
                        8005.0f
                    );
                    GXColor4u8(r, g, b, a);
                } else {
                    GXPosition3f32(
                        ac->pos.x + (mult * (s->asLine()->distanceFromCenter /
                                             4096.0f)),
                        ac->pos.y + (s->asLine()->lineA / 4096.0f), 8005.0f
                    );
                    GXColor4u8(r, g, b, a);
                    GXPosition3f32(
                        ac->pos.x + (mult * (s->asLine()->distanceFromCenter /
                                             4096.0f)),
                        ac->pos.y + (s->asLine()->lineB / 4096.0f), 8005.0f
                    );
                    GXColor4u8(r, g, b, a);
                }
                GXEnd();
                break;
            }
        }
    }
#endif
}