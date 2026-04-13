#include <utility>
#include <time.h>
#include <iostream>

#ifndef NOGUI
#define NOGUI
#endif
#include "PixMap/PixMap.hpp"
#include "mandelvrot/include.hpp"

const uint64_t c_u64RenderedFramesTarget = 1000;

const uint16_t c_hScreenWidth  = 960;
const uint16_t c_hScreenHeight = 720;

void PrintProgress (double dProgressPercent) {
    static const uint8_t sc_bScale = 100;

    static uint8_t s_bSpinnerPosition = 0;
    char             cSpinner         = '-';
    switch (s_bSpinnerPosition % 8)
    {
        case 0: cSpinner = '-';  break;
        case 1: cSpinner = '\\'; break;
        case 2: cSpinner = '|';  break;
        case 3: cSpinner = '/';  break;
        case 4: cSpinner = '-';  break;
        case 5: cSpinner = '\\'; break;
        case 6: cSpinner = '|';  break;
        case 7: cSpinner = '/';  break;
    }
    s_bSpinnerPosition++;
    std::cout << "" << cSpinner << " [";

    uint8_t bScaledProgress = static_cast<uint8_t>(sc_bScale * dProgressPercent);

    for 
    (
        uint8_t bFillingPositionIdx = 0; 
        bFillingPositionIdx < sc_bScale;
        bFillingPositionIdx++
    )
    {
        if (bFillingPositionIdx < bScaledProgress) std::cout << "=";
        else                                       std::cout << ".";
    }

    std::cout << "] " << dProgressPercent * 100. << "%\r";
    std::cout.flush ();
}

inline double ConvertTimespec2Double (const timespec& tSource)
{
    return tSource.tv_sec * 1e9 + tSource.tv_nsec;
}

int main ()
{
    PixMap stPixMap (c_hScreenWidth, c_hScreenHeight);

    float fScale = .8;
    std::pair<float, float> ffScreenShifts = {-.8, 0.};

    uint64_t u64FrameCounter = 0;

    double dRuntime = 0;
    timespec tEntryTimePoint {}, tExitTimePoint {};
    for (
        uint64_t u64FrameCounter = 0;
        u64FrameCounter < c_u64RenderedFramesTarget;
        u64FrameCounter++
    )
    {
        clock_gettime (CLOCK_THREAD_CPUTIME_ID, &tEntryTimePoint);
        adaptiveMandelvrot (stPixMap, fScale, ffScreenShifts);
        clock_gettime (CLOCK_THREAD_CPUTIME_ID, &tExitTimePoint);
        
        dRuntime += ConvertTimespec2Double (tExitTimePoint) - ConvertTimespec2Double (tEntryTimePoint);
        PrintProgress (static_cast<double> (u64FrameCounter) / c_u64RenderedFramesTarget);
    }

    std::cout << "\nexecution time: " << dRuntime / 1e6 << "ms\naverage frametime: " << dRuntime / 1e6 / c_u64RenderedFramesTarget << "ms" << std::endl;
}