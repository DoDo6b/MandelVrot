#pragma once

#include "../PixMap.hpp"

const uint16_t c_hDepth    = 255;
const float   c_fPermRad2 = 4.f;


void
stupidMandelvrot (
    PixMap& rPxMap,
    float fScale,
    std::pair<float, float> ffScreenShift
);

void
avxMandelvrot (
    PixMap& rPxMap,
    float fScale,
    std::pair<float, float> ffScreenShift
);