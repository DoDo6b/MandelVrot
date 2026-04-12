#include "../include.hpp"
#include <cstdint>


const uint8_t c_bParallelProcessing = 16;


#define ALLIGN alignas (c_bParallelProcessing * 4)
#define VECFOR for (uint8_t bIdx = 0; bIdx < c_bParallelProcessing; bIdx++)

__attribute__((used))
void
adaptiveMandelvrot (
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
    const float cfMathPixStepx8 = c_bParallelProcessing * cffMathPixStep.first;

    float fMathPixY = -1 * (crffScreenShift.second + cfMagnCoeff);
    const float cfMathPixX = crffScreenShift.first - rPixMap.GetAspectRatio () * cfMagnCoeff;

    for (
        uint16_t uhY = 0;
        uhY < rPixMap.GetScreen ().second;
        uhY++, fMathPixY += cffMathPixStep.second
    )
    {
        ALLIGN float afMathPixX[c_bParallelProcessing];
        VECFOR afMathPixX[bIdx] = cfMathPixX + cffMathPixStep.first * bIdx;

        for (
            uint16_t uhX = 0;
            uhX < rPixMap.GetScreen ().first;
            uhX += c_bParallelProcessing
        )
        {
            ALLIGN int aiIterations[c_bParallelProcessing] = {0};
            
            struct 
            {
                ALLIGN float m_afX[c_bParallelProcessing];
                ALLIGN float m_afY[c_bParallelProcessing];
                ALLIGN float m_afX2[c_bParallelProcessing];
                ALLIGN float m_afY2[c_bParallelProcessing];
                ALLIGN float m_afXY[c_bParallelProcessing];
            } stZ = {0};

            for (
                int iIteration = 0;
                iIteration < c_hDepth;
                iIteration++
            )
            {
                ALLIGN float afRad2[c_bParallelProcessing] = {0};
                VECFOR afRad2[bIdx] = stZ.m_afX2[bIdx] + stZ.m_afY2[bIdx];

                ALLIGN bool abMask[c_bParallelProcessing] = {0};
                VECFOR abMask[bIdx] = afRad2[bIdx] < c_fPermRad2;


                bool bInvalid = true;
                VECFOR
                {
                    if (abMask[bIdx])
                    {
                        bInvalid = false;
                        break;
                    }
                }
                if (bInvalid)
                {
                    break;
                }

                VECFOR stZ.m_afX[bIdx] = stZ.m_afX2[bIdx] - stZ.m_afY2[bIdx] + afMathPixX[bIdx];
                VECFOR stZ.m_afY[bIdx] = 2 * stZ.m_afXY[bIdx] + fMathPixY;

                VECFOR aiIterations[bIdx] += abMask[bIdx];

                VECFOR stZ.m_afX2[bIdx] = stZ.m_afX[bIdx] * stZ.m_afX[bIdx];
                VECFOR stZ.m_afY2[bIdx] = stZ.m_afY[bIdx] * stZ.m_afY[bIdx];
                VECFOR stZ.m_afXY[bIdx] = stZ.m_afX[bIdx] * stZ.m_afY[bIdx];
            }

            for (uint8_t bPackIdx = 0; bPackIdx < c_bParallelProcessing; bPackIdx++)
            {
                uint8_t bIterClr = (uint8_t)(aiIterations[bPackIdx] / 255. * c_hDepth);
                Color   stPixClr = aiIterations[bPackIdx] < c_hDepth ? Color {bIterClr, bIterClr, bIterClr, 255} : Color {0, 0, 0, 255};

                int32_t           iPixIdx = (uhY * rPixMap.GetScreen ().first + uhX + bPackIdx);
                rPixMap.GetMap ()[iPixIdx] = stPixClr;
            }

            VECFOR afMathPixX[bIdx] += cffMathPixStep.first * c_bParallelProcessing;
        }
    }
}