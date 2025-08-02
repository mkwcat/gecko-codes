#pragma once

cGCT_IMPORT_DECL(0x80253910);
cGCT_IMPORT_DECL(0x802541F0);

#include <nw4r/math/mtx.h>

namespace nw4r::g3d
{

struct CameraData;

class Camera
{
public:
    cGCT_IMPORT_ATTR( //
        0x80253910
    ) Camera(CameraData* pCameraData);

    cGCT_IMPORT_ATTR( //
        0x802541F0
    ) void GetCameraMtx(nw4r::math::MTX34* pMtx) const;

    CameraData* mCameraData;
};

} // namespace nw4r::g3d