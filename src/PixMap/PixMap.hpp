#pragma once


#include <cstdint>
#include <stdexcept>
#include <utility>
#include "raylib.h"

#define FPSLOCK 240

class PixMap
{
private:
    Color* m_pMap;
    std::pair<uint16_t, uint16_t> m_xyScreen;
    Texture2D m_Texture;

public:
    PixMap (
        uint16_t uhWidth, 
        uint16_t uhHeight
    ) :
    m_xyScreen (uhWidth, uhHeight)
    {
        m_pMap = new Color[m_xyScreen.first * m_xyScreen.second];
        if (m_pMap == nullptr)
        {
            throw std::bad_alloc ();
        }

        InitWindow (uhWidth, uhHeight, "Mandelvrot");
        SetTargetFPS (FPSLOCK);

        Image img = GenImageColor (uhWidth, uhHeight, BLACK);
        m_Texture = LoadTextureFromImage (img);
        UnloadImage (img);
    }

    ~PixMap ()
    {
        if (m_Texture.id)
        {
            UnloadTexture (m_Texture);
        }

        delete[] m_pMap;
    }

    PixMap (const PixMap&)            = delete;
    PixMap& operator= (const PixMap&) = delete;

    PixMap (
        PixMap&& other
    ) :
    m_pMap (nullptr), 
    m_Texture{0}, 
    m_xyScreen (0,0)
    {
        std::swap (m_pMap, other.m_pMap);
        std::swap (m_xyScreen, other.m_xyScreen);
        std::swap (m_Texture, other.m_Texture);
    }

    Color*
    GetMap () const
    {
        return m_pMap;
    }

    const std::pair<uint16_t, uint16_t>& 
    GetScreen () const
    {
        return m_xyScreen;
    }

    uint32_t 
    GetSize () const
    {
        return m_xyScreen.first * m_xyScreen.second;
    }

    float 
    GetAspectRatio () const
    {
        return (float)m_xyScreen.first / m_xyScreen.second;
    }

    void 
    UpdateWindow (
        int32_t x,
        int32_t y,
        Color color = WHITE
    )
    {
        UpdateTexture (m_Texture, m_pMap);
        DrawTexture (m_Texture, x, y, color);
    }
};