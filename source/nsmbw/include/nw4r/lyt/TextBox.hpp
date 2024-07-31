#pragma once

#include "Pane.hpp"
#include "Resource.hpp"

namespace nw4r::lyt
{

class TextBox : public Pane
{
public:
    cGCT_IMPORT_ATTR( //
        0x802ADFE0
    ) TextBox(res::TextBox* res, ResBlockSet& resBlockSet);

    cGCT_IMPORT_ATTR( //
        0x802AE2D0
    ) TextBox* __dt(int type);

    FILL(0x0D8, 0x104);
};

static_assert(sizeof(TextBox) == 0x104);

} // namespace nw4r::lyt