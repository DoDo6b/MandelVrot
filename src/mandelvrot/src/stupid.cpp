#include "../include.hpp"
#include <cstdint>

__attribute__((used))
void
stupidMandelvrot (
    PixMap& rPixMap,
    float fScale,
    const std::pair<float, float>& crffScreenShift
)
{
    const float cfMagnCoeff  = 1. / fScale;

    const std::pair<float, float> cffMathPixStep = {
        rPixMap.GetAspectRatio () * cfMagnCoeff * (2. / rPixMap.GetScreen ().first),
        cfMagnCoeff * (2. / rPixMap.GetScreen ().second)
    };

    std::pair<float, float> ffMathPix = {
        0,
        -1 * (crffScreenShift.second + cfMagnCoeff)
    };
 
    for (
        uint16_t uhY = 0;
        uhY < rPixMap.GetScreen ().second; 
        uhY++, ffMathPix.second += cffMathPixStep.second
    )
    {
        ffMathPix.first = crffScreenShift.first - rPixMap.GetAspectRatio () * cfMagnCoeff;

        for (
            uint16_t uhX = 0;
            uhX < rPixMap.GetScreen ().first;
            uhX++, ffMathPix.first  += cffMathPixStep.first
        )
        {
            uint16_t uhIterationsPassed = 0;
            
            std::pair<float, float> ffZ  = {0, 0};
            std::pair<float, float> ffZ2 = {0, 0};

            while (
                ffZ2.first + ffZ2.second < c_fPermRad2 && 
                uhIterationsPassed < c_hDepth
            )
            {
                ffZ.second = 2 * ffZ.first * ffZ.second + ffMathPix.second;
                ffZ.first  = ffZ2.first - ffZ2.second + ffMathPix.first;
                
                ffZ2.first  = ffZ.first  * ffZ.first;
                ffZ2.second = ffZ.second * ffZ.second;

                ++uhIterationsPassed;
            }

            
            uint8_t                                                   bIterClr = (uint8_t)(uhIterationsPassed / 255. * c_hDepth);
            Color   stPixClr = uhIterationsPassed < c_hDepth ? Color {bIterClr, bIterClr, bIterClr, 255} : BLACK;

            int32_t           iPixIdx = (uhY * rPixMap.GetScreen ().first + uhX);
            rPixMap.GetMap ()[iPixIdx] = stPixClr;
        }
    }
}