#include <stdexcept>
#include <utility>
#include <memory>
#include <iostream>
#include <chrono>


#include "PixMap/PixMap.hpp"

#ifndef NOGUI
#include "raylib.h"
#else
#include "PixMap/mock.hpp"
#endif

#include "mandelvrot/include.hpp"


#define WSCREEN 960
#define HSCREEN 720
#define SENSETIVITY 2

static const struct Position
{
    std::pair<float, float> m_ffShifts;
    float m_fScale;
} sc_astPresettedPositions[] = {
    {
        .m_ffShifts = {.349375, .512087},
        .m_fScale   = 16833.265625
    },
    {
        .m_ffShifts = {-.549938, .625749},
        .m_fScale   = 12395.225586
    },
    {
        .m_ffShifts = {-.989273, .299529},
        .m_fScale   = 4522.115234
    },
    {
        .m_ffShifts = {-1.243752, .116213},
        .m_fScale   = 21406.693359
    }
};

inline void
CameraMoveTo (
    bool bStartAnimation, 
    std::pair<float, float>& rffScreenShifts, 
    float& rfScale, 
    const std::pair<float, float>& crffTarget = {-.8, 0.}, 
    const float cfTargetScale = .8
)
{
    static bool s_bPlaying = false;
    static std::pair<float, float> s_ffTarget = {-.8, 0.};
    static float s_fTargetZoom = .8;

    if (s_bPlaying == false && bStartAnimation) 
    {
        s_bPlaying = true;
        s_ffTarget = crffTarget;
        s_fTargetZoom = cfTargetScale;
    }

    if (s_bPlaying == false)
    {
        return;
    }

    if (
        fabs (s_ffTarget.first  - rffScreenShifts.first) < .01f &&
        fabs (s_ffTarget.second - rffScreenShifts.second) < .01f &&
        fabs (s_fTargetZoom     - rfScale) < .01f
    )
    {
        s_bPlaying = false;
    }

    if (s_bPlaying)
    {
        float fFrameTime = GetFrameTime ();
        rffScreenShifts.first  += (s_ffTarget.first  - rffScreenShifts.first)  * fFrameTime * 3;
        rffScreenShifts.second += (s_ffTarget.second - rffScreenShifts.second) * fFrameTime * 3;

        rfScale += (s_fTargetZoom - rfScale) * fFrameTime * 3;
    }
}


int main ()
{
    try
    {
        PixMap pixMap (WSCREEN, HSCREEN);

        float fScale  = .8;
        std::pair<float, float> ffScreenShifts = {-.8, 0.};

        while (!WindowShouldClose ())
        {
            float fFrameTime = GetFrameTime();

            float fMWheelMove = GetMouseWheelMove ();
            if (fMWheelMove) fScale *= (fMWheelMove > 0) ? 1.1 : 0.9;

            float fShiftStep = SENSETIVITY / fScale * fFrameTime;

            if (IsKeyDown (KEY_RIGHT) || IsKeyDown (KEY_D)) ffScreenShifts.first  += fShiftStep;
            if (IsKeyDown (KEY_UP)    || IsKeyDown (KEY_W)) ffScreenShifts.second += fShiftStep;
            if (IsKeyDown (KEY_LEFT)  || IsKeyDown (KEY_A)) ffScreenShifts.first  -= fShiftStep;
            if (IsKeyDown (KEY_DOWN)  || IsKeyDown (KEY_S)) ffScreenShifts.second -= fShiftStep;

            int iPressedKey = GetKeyPressed ();

            if (iPressedKey == KEY_SPACE)
            {
                CameraMoveTo (
                    true,
                    ffScreenShifts,
                    fScale,
                    {-.8, 0},
                    .8
                );
            }
            else
            {
                int iPresetedPositionId = iPressedKey - KEY_ONE;
                if (iPresetedPositionId >= 0 && iPresetedPositionId < sizeof (sc_astPresettedPositions) / sizeof (Position))
                {
                    CameraMoveTo (
                        true,
                        ffScreenShifts,
                        fScale,
                        sc_astPresettedPositions[iPresetedPositionId].m_ffShifts,
                        sc_astPresettedPositions[iPresetedPositionId].m_fScale
                    );
                }
                else
                {
                    CameraMoveTo (false, ffScreenShifts, fScale);
                }
            }
            

            adaptiveMandelvrot (pixMap, fScale, ffScreenShifts);

            BeginDrawing ();
            ClearBackground (BLACK);

            pixMap.UpdateWindow (0, 0);

            DrawText (TextFormat ("Scale: %f", fScale), 10, 10, 20, WHITE);
            DrawText (TextFormat ("X: %f Y: %f", ffScreenShifts.first, ffScreenShifts.second), 10, 35, 20, WHITE);
            DrawText (TextFormat ("FPS: %d", GetFPS ()), 10, 60, 20, LIME);
            DrawText (TextFormat ("Frametime: %.2f", GetFrameTime ()*1000.), 10, 85, 20, LIME);

            EndDrawing ();
        }

        CloseWindow ();
    }
    catch (const std::exception& e)
    {
        std::cerr << "exception occured: \"" << e.what () << "\"" << std::endl;
        return 1;
    }

    return 0;
}