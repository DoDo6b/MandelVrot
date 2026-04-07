#include "../include.hpp"
#include <cstdint>
#include <immintrin.h>


static const __m256 sce_8fPackIdx  = _mm256_set_ps (7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f);
static const __m256 sce_8fPermRad2 = _mm256_set1_ps (c_fPermRad2);

void
avxMandelvrot (
    PixMap& rPixMap,
    float fScale,
    std::pair<float, float> ffScreenShift
)
{
    const float cfMagnCoeff  = 1. / fScale;

    const std::pair<float, float> cffMathPixStep = {
        rPixMap.GetAspectRatio () * cfMagnCoeff * (2. / rPixMap.GetScreen ().first),
        cfMagnCoeff * (2. / rPixMap.GetScreen ().second)
    };
    const std::pair<__m256, __m256> c8f8fMathPixStep = {
        _mm256_mul_ps (_mm256_set1_ps (cffMathPixStep.first), sce_8fPackIdx),
        _mm256_set1_ps (cffMathPixStep.second)
    };
    const __m256 c8fMathPixStepx8 = _mm256_set1_ps (8 * cffMathPixStep.first);

    std::pair<float, float> ffMathPix = {
        0,
        -1 * (ffScreenShift.second + cfMagnCoeff)
    };
    std::pair<__m256, __m256>_8f8fMathPix;
                             _8f8fMathPix.second = _mm256_set1_ps (ffMathPix.second);

    for (
        uint16_t uhY = 0;
        uhY < rPixMap.GetScreen ().second;
        uhY++, ffMathPix.second += cffMathPixStep.second
    )
    {
        ffMathPix.first = ffScreenShift.first - rPixMap.GetAspectRatio () * cfMagnCoeff;
       _8f8fMathPix.first = _mm256_set1_ps (ffMathPix.first);

       _8f8fMathPix.first = _mm256_add_ps (_8f8fMathPix.first, c8f8fMathPixStep.first);

        for (
            uint16_t uhX = 0;
            uhX < rPixMap.GetScreen ().first;
            uhX += 8
        )
        {
            __m256i _8iIterations = _mm256_setzero_si256 ();
            
            struct 
            {
                __m256 m_8fX;
                __m256 m_8fY;
                __m256 m_8fX2;
                __m256 m_8fY2;
                __m256 m_8fXY;
            } stZ = {_mm256_setzero_ps ()};

            for (
                int iIteration = 0;
                iIteration < c_hDepth;
                iIteration++
            )
            {
                __m256 _8fRad2 = _mm256_add_ps (stZ.m_8fX2, stZ.m_8fY2);

                __m256 _8fMask = _mm256_cmp_ps (_8fRad2, sce_8fPermRad2, _CMP_LT_OQ);


                if (!_mm256_movemask_ps (_8fMask))
                {
                    break;
                }

                stZ.m_8fY = _mm256_add_ps (
                    _8f8fMathPix.second,
                    _mm256_mul_ps (_mm256_set1_ps (2.), stZ.m_8fXY)
                );
                stZ.m_8fX = _mm256_add_ps (
                    _8f8fMathPix.first, 
                    _mm256_sub_ps (stZ.m_8fX2, stZ.m_8fY2)
                );

                _8iIterations = _mm256_sub_epi32 (
                    _8iIterations,
                    _mm256_castps_si256 (_8fMask)
                );

                stZ.m_8fX2 = _mm256_mul_ps (stZ.m_8fX, stZ.m_8fX);
                stZ.m_8fY2 = _mm256_mul_ps (stZ.m_8fY, stZ.m_8fY);
                stZ.m_8fXY = _mm256_mul_ps (stZ.m_8fX, stZ.m_8fY);
            }

            int32_t aiIterations[8] = {};
            _mm256_storeu_si256 ((__m256i*)aiIterations, _8iIterations);

            for (uint8_t bPackIdx = 0; bPackIdx < 8; bPackIdx++)
            {
                uint8_t bIterClr = (uint8_t)(aiIterations[bPackIdx] / 255. * c_hDepth);
                Color   stPixClr = aiIterations[bPackIdx] < c_hDepth ? Color {bIterClr, bIterClr, bIterClr, 255} : BLACK;

                int32_t           iPixIdx = (uhY * rPixMap.GetScreen ().first + uhX + bPackIdx);
                rPixMap.GetMap ()[iPixIdx] = stPixClr;
            }

            _8f8fMathPix.first = _mm256_add_ps (_8f8fMathPix.first, c8fMathPixStepx8);
        }
        _8f8fMathPix.second = _mm256_add_ps (_8f8fMathPix.second, c8f8fMathPixStep.second);
    }
}