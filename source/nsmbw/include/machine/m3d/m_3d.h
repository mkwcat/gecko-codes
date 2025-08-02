#pragma once

#include <nw4r/g3d/Camera.h>

namespace m3d
{

cGCT_IMPORT( //
    0x80164C60
) nw4r::g3d::CameraData* getCamera(int id);

cGCT_IMPORT( //
    0x80164C80
) int getCurrentCameraID();

} // namespace m3d