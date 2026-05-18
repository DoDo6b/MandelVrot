#include <fstream>
#include <string_view>
#include <utility>
#include <time.h>
#include <iostream>

#ifndef NOGUI
#define NOGUI
#endif
#include "PixMap/PixMap.hpp"
#include "mandelvrot/include.hpp"

const uint64_t c_u64RenderedFramesTarget = 1000;
const uint16_t c_hTestIterations = 16;

const uint16_t c_hScreenWidth  = 960;
const uint16_t c_hScreenHeight = 720;

void PrintProgress (double dProgressPercent)
{
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

    uint8_t bScaledProgress = static_cast<uint8_t> (sc_bScale * dProgressPercent);

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

int
main (
    int argc,
    char** argv
)
{
    PixMap stPixMap (c_hScreenWidth, c_hScreenHeight);

    float fScale = .8;
    std::pair<float, float> ffScreenShifts = {-.8, 0.};

    uint64_t u64FrameCounter = 0;

    
    double dRuntimeSqrTotal = 0;
    double dRuntimeTotal    = 0;

    std::fstream logs ("logs.txt", std::ios::out);

    std::string_view mode =
    #ifdef ADAPTIVE
    "ADAPTIVE\n\n";
    #else
    #ifdef AVX
    "AVX\n\n";
    #else
    "STUPID\n\n";
    #endif
    #endif

    std::cout << mode;
    logs << mode;

    timespec tEntryTimePoint {}, tExitTimePoint {};
    for (
        uint16_t hIteration = 0;
        hIteration < c_hTestIterations;
        hIteration++
    )
    {
        std::cout << "[" << hIteration << "/" << c_hTestIterations - 1 << "]:\n";
        logs      << "\n[" << hIteration << "/" << c_hTestIterations - 1 << "]:\n";
        
        double dIterationRuntime = 0;
        for (
            uint64_t u64FrameCounter = 0;
            u64FrameCounter <= c_u64RenderedFramesTarget;
            u64FrameCounter++
        )
        {
            clock_gettime (
                CLOCK_THREAD_CPUTIME_ID,
                &tEntryTimePoint
            );

            #ifdef ADAPTIVE
            adaptiveMandelvrot (
                stPixMap,
                fScale,
                ffScreenShifts
            );
            #else
            #ifdef AVX
            avxMandelvrot (
                stPixMap,
                fScale,
                ffScreenShifts
            );
            #else
            stupidMandelvrot (
                stPixMap,
                fScale,
                ffScreenShifts
            );
            #endif
            #endif

            clock_gettime (
                CLOCK_THREAD_CPUTIME_ID,
                &tExitTimePoint
            );
            
            double dFrametime = (ConvertTimespec2Double (tExitTimePoint) - ConvertTimespec2Double (tEntryTimePoint)) / 1e6;

            dIterationRuntime += dFrametime;
            dRuntimeSqrTotal  += dFrametime * dFrametime;
            
            #ifndef LESSLOG
            PrintProgress (static_cast<double> (u64FrameCounter) / c_u64RenderedFramesTarget);
            #endif
        }
        std::cout << "\nexecution time: " << dIterationRuntime << "ms\naverage frametime: " << dIterationRuntime / c_u64RenderedFramesTarget << "ms\n";
        logs      <<   "execution time: " << dIterationRuntime << "ms\naverage frametime: " << dIterationRuntime / c_u64RenderedFramesTarget << "ms\n";
        

        dRuntimeTotal += dIterationRuntime;
    }

    constexpr uint64_t ce_u64TotalFrames = c_u64RenderedFramesTarget * c_hTestIterations;
    double dAverageFrametime = dRuntimeTotal / ce_u64TotalFrames;
    double dDispercion = dRuntimeSqrTotal / ce_u64TotalFrames - dAverageFrametime * dAverageFrametime;

    std::cout
        << "\ntotal execution time: " << dRuntimeTotal << "ms\n"
        << "average frametime: " << dAverageFrametime 
        << "ms, dispersion: " << dDispercion << "ms\n"
        << "average FPS: " << ce_u64TotalFrames / dRuntimeTotal * 1e3 << "\n";
    logs
        << "\ntotal execution time: " << dRuntimeTotal << "ms\n"
        << "average frametime: " << dAverageFrametime 
        << "ms, dispersion: " << dDispercion << "ms\n"
        << "average FPS: " << ce_u64TotalFrames / dRuntimeTotal * 1e3 << "\n";
    logs.close ();

    return 0;
}