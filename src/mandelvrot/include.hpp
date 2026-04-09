#pragma once

#include "../PixMap/PixMap.hpp"

const uint16_t c_hDepth    = 255;
const float   c_fPermRad2 = 4.f;


void
stupidMandelvrot (
    PixMap& rPxMap,
    float fScale,
    const std::pair<float, float>& crffScreenShift
);

void
adaptiveMandelvrot (
    PixMap& rPxMap,
    float fScale,
    const std::pair<float, float>& crffScreenShift
);

void
avxMandelvrot (
    PixMap& rPxMap,
    float fScale,
    const std::pair<float, float>& crffScreenShift
);