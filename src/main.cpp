#include <stdexcept>
#include <utility>
#include <memory>
#include <iostream>

#include "raylib.h"
#include "PixMap.hpp"
#include "mandelvrot/include.hpp"


#define WSCREEN 800
#define HSCREEN 600
#define SENSETIVITY 2

inline void CameraReturn (std::pair<float, float>& rffScreenShifts, float& rfScale, const std::pair<float, float>& crffTarget, const float cfTargetScale)
{
    static bool s_bPlaying = false;

    if (s_bPlaying == false && IsKeyPressed (KEY_SPACE)) 
    {
        s_bPlaying = true;
    }

    if (s_bPlaying == false)
    {
        return;
    }

    if (
        fabs (crffTarget.first  - rffScreenShifts.first) < .001f &&
        fabs (crffTarget.second - rffScreenShifts.second) < .001f &&
        fabs (cfTargetScale     - rfScale) < .001f
    )
    {
        s_bPlaying = false;
    }

    if (s_bPlaying)
    {
        float fFrameTime = GetFrameTime ();
        rffScreenShifts.first  += (crffTarget.first  - rffScreenShifts.first)  * fFrameTime * 3;
        rffScreenShifts.second += (crffTarget.second - rffScreenShifts.second) * fFrameTime * 3;

        rfScale += (cfTargetScale - rfScale) * fFrameTime * 3;
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

            if (IsKeyDown (KEY_RIGHT) || IsKeyDown (KEY_D)) ffScreenShifts.first  += SENSETIVITY / fScale * fFrameTime;
            if (IsKeyDown (KEY_UP)    || IsKeyDown (KEY_W)) ffScreenShifts.second += SENSETIVITY / fScale * fFrameTime;
            if (IsKeyDown (KEY_LEFT)  || IsKeyDown (KEY_A)) ffScreenShifts.first  -= SENSETIVITY / fScale * fFrameTime;
            if (IsKeyDown (KEY_DOWN)  || IsKeyDown (KEY_S)) ffScreenShifts.second -= SENSETIVITY / fScale * fFrameTime;

            CameraReturn (ffScreenShifts, fScale, {-.8, 0.}, .8);

            avxMandelvrot (pixMap, fScale, ffScreenShifts);

            BeginDrawing ();
            ClearBackground (BLACK);

            pixMap.UpdateWindow (0, 0);

            DrawText (TextFormat ("Scale: %.2f", fScale), 10, 10, 20, WHITE);
            DrawText (TextFormat ("X: %.2f Y: %.2f", ffScreenShifts.first, ffScreenShifts.second), 10, 35, 20, WHITE);
            DrawText (TextFormat ("FPS: %d", GetFPS ()), 10, 60, 20, LIME);

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