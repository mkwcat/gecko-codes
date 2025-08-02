#pragma once

#include "d_base.h"
#include "machine/m_angle.h"
#include "machine/m_vec.h"

/**
 * Basic actor class, supporting positioning, rotation and
 * movement/acceleration.
 */
class dBaseActor_c : public dBase_c
{
public:
    /**
     * An identifier that represents the actor's kind. (This does not seem to be
     * used outside of worldmaps).
     */
    enum ACTOR_KIND_e {
        /**
         * Used for behind-the-scenes actors (such as dCsSeqMng_c).
         */
        ACTOR_SYSTEM,

        /**
         * Used for actors that are affected by cutscenes.
         */
        ACTOR_MAP_DEMO,

        /**
         * Used for most map actors.
         */
        ACTOR_MAP,

        /**
         * Used for map enemies.
         */
        ACTOR_MAP_ENEMY,

        /**
         * The worldmap player actor (dWmPlayer_c).
         */
        ACTOR_MAP_PLAYER,

        /**
         * Identifies the daWmStop_c actor.
         */
        ACTOR_MAP_STOP,

        /**
         * Unused.
         */
        ACTOR_UNK_6,

        /**
         * The menu player actor (da2DPlayer_c).
         */
        ACTOR_MENU_PLAYER,

        /**
         * The default value.
         */
        ACTOR_GENERIC,
    };

    dBaseActor_c();

protected:
    DECL_VIRTUAL_N(int, preCreate) override;
    DECL_VIRTUAL_N(void, postCreate, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preDelete) override;
    DECL_VIRTUAL_N(void, postDelete, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preExecute) override;
    DECL_VIRTUAL_N(void, postExecute, MAIN_STATE_e state) override;
    DECL_VIRTUAL_N(int, preDraw) override;
    DECL_VIRTUAL_N(void, postDraw, MAIN_STATE_e state) override;

    DECL_VIRTUAL_N(void, __dt, int type) override;

    DECL_VIRTUAL(0x50, void, draw2D);
    DECL_VIRTUAL(0x54, void, draw2D_lyt2);
    DECL_VIRTUAL(0x58, int, GetActorType);
    DECL_VIRTUAL(0x5C, void, finalUpdate);

private:
    /* 0x070 */ u8 mLinkActor[0xC];

public:
    /**
     * The actor's world matrix.
     */
    /* 0x07C */ float mWorldMatrix[3][4];

    /**
     * The actor's position (top-left corner).
     */
    /* 0x0AC */ mVec3_c mPos;

    /**
     * The actor's position in the previous frame.
     */
    /* 0x0B8 */ mVec3_c mLastPos;

    /**
     * The actor's position delta since the previous frame.
     */
    /* 0x0C4 */ mVec3_c mPosDelta;

    /**
     * The offset from the position to the center of the actor (defaults to 0).
     */
    /* 0x0D0 */ mVec3_c mCenterOffs;

    /**
     * The actor's scale (defaults to 1).
     */
    /* 0x0DC */ mVec3_c mScale;

    /**
     * The actor's speed.
     */
    /* 0x0E8 */ mVec3_c mSpeed;

    /**
     * The actor's maximum speed.
     */
    /* 0x0F4 */ mVec3_c mSpeedMax;

    /**
     * The actor's rotation.
     */
    /* 0x100 */ mAng3_c mAngle;

    /**
     * The actor's rotation in the previous frame.
     */
    /* 0x106 */ mAng3_c mLastAngle;

    /**
     * The actor's horizontal speed.
     */
    /* 0x10C */ float mSpeedF;

    /**
     * The actor's maximum horizontal speed.
     */
    /* 0x110 */ float mMaxSpeedF;

    /**
     * The actor's vertical acceleration.
     */
    /* 0x114 */ float mAccelY;

    /**
     * The actor's maximum fall speed.
     */
    /* 0x118 */ float mMaxFallSpeed;

    /**
     * The actor's horizontal acceleration.
     */
    /* 0x11C */ float mAccelF;

    /**
     * The actor's properties. See fProfile::fActorProfile_c::mActorProperties.
     */
    /* 0x120 */ u32 mActorProperties;

    /**
     * Whether the actor should be visible or not. Defaults to true.
     */
    /* 0x124 */ bool mVisible;
};

static_assert(sizeof(dBaseActor_c) == 0x128);
