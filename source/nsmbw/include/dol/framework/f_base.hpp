#pragma once

#include "f_base_id.hpp"
#include <types.h>

template <u32 N>
consteval bool IsVoid(const char (&str)[N])
{
    if (N != 5) {
        return false;
    }

    if (str[0] != 'v' || str[1] != 'o' || str[2] != 'i' || str[3] != 'd') {
        return false;
    }

    return true;
}

#define DECL_VIRTUAL(_OFFSET, _RETURN, _NAME, ...)                             \
    template <class... Args>                                                   \
    _RETURN _NAME(Args... args)                                                \
    {                                                                          \
        auto func = (_RETURN(*)(decltype(this), ##__VA_ARGS__))(               \
            this->vtable[_OFFSET >> 2]                                         \
        );                                                                     \
        if constexpr (IsVoid(#_RETURN)) {                                      \
            func(this, args...);                                               \
        } else {                                                               \
            return func(this, args...);                                        \
        }                                                                      \
    }                                                                          \
                                                                               \
    virtual _RETURN _NAME##__IMPL(__VA_ARGS__)

#define IMPORT_VIRTUAL(_ADDRESS, _OFFSET, _RETURN, _NAME, ...)                 \
    template <class... Args>                                                   \
    _RETURN _NAME(Args... args)                                                \
    {                                                                          \
        auto func = (_RETURN(*)(decltype(this), ##__VA_ARGS__))(               \
            this->vtable[_OFFSET >> 2]                                         \
        );                                                                     \
        if constexpr (IsVoid(#_RETURN)) {                                      \
            func(this, args...);                                               \
        } else {                                                               \
            return func(this, args...);                                        \
        }                                                                      \
    }                                                                          \
                                                                               \
    cGCT_IMPORT_ATTR(_ADDRESS) virtual _RETURN _NAME##__IMPL(__VA_ARGS__)

#define DECL_VIRTUAL_N(_RETURN, _NAME, ...) _RETURN _NAME##__IMPL(__VA_ARGS__)

#define IMPORT_VIRTUAL_N(_ADDRESS, _RETURN, _NAME, ...)                        \
    cGCT_IMPORT_ATTR(_ADDRESS) _RETURN _NAME##__IMPL(__VA_ARGS__)

struct DummyStruct {
};

#define ENABLE_VTABLE_FIX                                                      \
    DummyStruct _vtFix_##__COUNTER__ = [&]() {                                 \
        vtable = *reinterpret_cast<u32**>(this);                               \
        asm volatile("" : "=rm"(*reinterpret_cast<u32*>(this)));               \
        return DummyStruct{};                                                  \
    }();

class fBase_c
{
public:
    /**
     * The possible lifecycle states.
     */
    enum LIFECYCLE_e {
        /**
         * The base's create operation has yet to conclude.
         */
        CREATING,
        /**
         * The base is in the main execution cycle.
         */
        ACTIVE,
        /**
         * The base's delete operation is about to run.
         */
        DELETING,
    };

    /**
     * The possible group types.
     */
    enum GROUP_TYPE_e {
        /**
         * The base is a @ref dBase_c "generic process".
         */
        OTHER,
        /**
         * The base is a @ref dScene_c "scene".
         */
        SCENE,
        /**
         * The base is an @ref dBaseActor_c "actor".
         */
        ACTOR,
    };

    /**
     * The possible operation results.
     */
    enum MAIN_STATE_e {
        /**
         * The operation was canceled early.
         */
        CANCELED,
        /**
         * The operation could not be completed.
         */
        ERROR,
        /**
         * The operation was completed successfully.
         */
        SUCCESS,
        /**
         * The operation is waiting for something and cannot be completed yet.
         */
        WAITING,
    };

    /**
     * The possible operation step results.
     */
    enum PACK_RESULT_e {
        /**
         * The step could not completed at this time.
         */
        NOT_READY,
        /**
         * The step was completed successfully.
         */
        SUCCEEDED,
        /**
         * The step could not be completed.
         */
        FAILED,
    };

    /**
     * Controls if the execute and draw operations should be skipped.
     */
    enum PROC_DISABLE_e {
        /**
         * Execution is disabled, and this is a root base.
         */
        ROOT_DISABLE_EXECUTE = 1,
        /**
         * Execution is disabled.
         */
        DISABLE_EXECUTE = 2,
        /**
         * Drawing is disabled, and this is a root base.
         */
        ROOT_DISABLE_DRAW = 4,
        /**
         * Drawing is disabled.
         */
        DISABLE_DRAW = 8,
    };

    /**
     * Unique ID that is incremented for every created base. This is commented
     * out here due to the temporary vtable taking its place in the GCC
     * structure.
     */
    /* 0x00 */ // fBaseID_e mUniqueID;
    fBaseID_e getUniqueID() const
    {
        return *reinterpret_cast<const fBaseID_e*>(this);
    }

    /**
     * The current unique ID.
     */
    /* 0x80429748 */ static fBaseID_e m_rootUniqueID;

    /**
     * A bitfield that configures the base's behaviour. (Represents nybbles 5 to
     * 12 of Reggie's spritedata).
     */
    /* 0x04 */ u32 mParam;

    /**
     * The base's profile name
     */
    /* 0x08 */ u16 mProfName; // The base's profile name.

    /**
     * The temporary state value for the profile name.
     */
    /* 0x8042A678 */ static u16 m_tmpCtProfName;

protected:
    /**
     * The base's lifecycle state. Value is a ::LIFECYCLE_e.
     */
    /* 0x0A */ u8 mLifecycleState;

    /**
     * If deletion of the base was requested, but the corresponding operation
     * has not been scheduled yet.
     */
    /* 0x0B */ bool mDeleteRequested;

    /**
     * If the create operation was completed, but scheduling the execute
     * and draw operations isn't possible at this time.
     * @details If true, scheduling will be deferred to the next connect
     * operation.
     */
    /* 0x0C */ bool mDeferExecute;

    /**
     * If the create operation has not been completed, and rescheduling it
     * isn't possible at this time.
     * @details If true, rescheduling will be deferred to the next connect
     * operation.
     */
    /* 0x0D */ bool mDeferRetryCreate;

    /**
     * The base's group type. Value is a ::GROUP_TYPE_e.
     */
    /* 0x0E */ u8 mGroupType;

    /**
     * The operations to be skipped. Value is a ::PROC_DISABLE_e.
     */
    /* 0x0F */ u8 mProcControl;

    /**
     * Checks if a flag is set in ::mProcControl.
     */
    bool isProcControlFlag(u8 flag) const
    {
        return (mProcControl & flag) != 0;
    }

    /**
     * Sets a flag in ::mProcControl.
     */
    void setProcControlFlag(u8 flag)
    {
        mProcControl |= flag;
    }

    /**
     * Clears a flag in ::mProcControl.
     */
    void clearProcControlFlag(u8 flag)
    {
        mProcControl &= ~flag;
    }

    // Temporary fill data
    /* 0x10 */ u8 _fill[0x60 - 0x10];

    /**
     * Actual vtable pointer, autogenerated by CW but has to be explicit for
     * GCC.
     */
    /* 0x60 */ u32* vtable;

private:
    // Padding for GCC, as it expects the vtable pointer to be +0x8 from the
    // beginning of the vtable.
    virtual void _0()
    {
    }

    virtual void _4()
    {
    }

public:
    fBase_c();

    /**
     * do method for the create operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x08, int, create);

    /**
     * pre method for the create operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x0C, int, preCreate);

    /**
     * post method for the create operation.
     */
    DECL_VIRTUAL(0x10, void, postCreate, MAIN_STATE_e state);

    /**
     * do method for the delete operation.
     * @details This method was renamed due to conflict with the delete C++
     * keyword.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x14, int, doDelete);

    /**
     * pre method for the delete operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x18, int, preDelete);

    /**
     * post method for the delete operation.
     */
    DECL_VIRTUAL(0x1C, void, postDelete, MAIN_STATE_e state);

    /**
     * do method for the execute operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x20, int, execute);

    /**
     * pre method for the execute operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x24, int, preExecute);

    /**
     * post method for the execute operation.
     */
    DECL_VIRTUAL(0x28, void, postExecute, MAIN_STATE_e state);

    /**
     * do method for the draw operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x2C, int, draw);

    /**
     * pre method for the draw operation.
     * @return A ::PACK_RESULT_e value.
     */
    DECL_VIRTUAL(0x30, int, preDraw);

    /**
     * post method for the draw operation.
     */
    DECL_VIRTUAL(0x34, void, postDraw, MAIN_STATE_e state);

    /**
     * Informs the base that it's about to be deleted.
     */
    DECL_VIRTUAL(0x38, void, deleteReady);

    /**
     * @unused Creates a heap of the given size for the base.
     * @details If the requested heap space is not available, the heap is
     * adjusted to allocate all the available memory. If that also fails, the
     * base is deleted.
     * @param size The heap's size, or -1 to allocate all available space.
     * @param parentHeap The parent heap, or nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    DECL_VIRTUAL(0x3C, bool, entryFrmHeap, u32 size, void* parentHeap);

    /**
     * @unused Creates a heap of the given size for the base.
     * @details Unlike ::entryFrmHeap, the base is immediately deleted if the
     * requested space is not available.
     * @param size The heap's size, or -1 to allocate all available space.
     * @param parentHeap The parent heap, or nullptr to use the current heap.
     * @return If the heap creation was successful.
     */
    DECL_VIRTUAL(0x40, bool, entryFrmHeapNonAdjust, u32 size, void* parentHeap);

    /**
     * @unused Does nothing.
     */
    DECL_VIRTUAL(0x44, bool, createHeap);

    /**
     * Destroys the base.
     */
    DECL_VIRTUAL(0x48, void, __dt, int type);
};

static_assert(sizeof(fBase_c) == 0x64);
