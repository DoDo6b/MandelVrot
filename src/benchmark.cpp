#include <utility>
#include <chrono>
#include <iostream>

#ifndef NOGUI
#define NOGUI
#endif
#include "PixMap/PixMap.hpp"
#include "mandelvrot/include.hpp"

const uint64_t c_u64RenderedFramesTarget = 1000;

const uint16_t c_hScreenWidth  = 960;
const uint16_t c_hScreenHeight = 720;

int main ()
{
    PixMap stPixMap (c_hScreenWidth, c_hScreenHeight);

    float fScale = 10;
    std::pair<float, float> ffScreenShifts = {-.5, 0.};

    uint64_t u64FrameCounter = 0;

    std::chrono::duration<double, std::milli> tRuntime (0);
    for (
        uint64_t u64FrameCounter = 0;
        u64FrameCounter < c_u64RenderedFramesTarget;
        u64FrameCounter++
    )
    {
        auto tEntryTimePoint = std::chrono::high_resolution_clock::now ();
        adaptiveMandelvrot (stPixMap, fScale, ffScreenShifts);
        auto tExitTimePoint = std::chrono::high_resolution_clock::now ();
        
        tRuntime += tExitTimePoint - tEntryTimePoint;
    }

    std::cout << "execution time: " << tRuntime.count () << "ms" << std::endl;
}