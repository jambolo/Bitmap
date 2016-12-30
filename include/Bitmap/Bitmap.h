#if !defined(BITMAP_H_INCLUDED)
#define BITMAP_H_INCLUDED

#pragma once

/*****************************************************************************

                                   Bitmap.h

                        Copyright 2001, John J. Bolton
    ----------------------------------------------------------------------

    $Header: //depot/Libraries/Bitmap/Bitmap.h#2 $

    $NoKeywords: $

*****************************************************************************/

#include "Pixel.h"

#include <Cstring>
#include <new>

#include "../Rect/Rect.h"
#include "Palette.h"

// Forward declarations

template <class _Pixel>
class Bitmap
{
public:

    typedef _Pixel Pixel;

    enum { PIXEL_SIZE = sizeof Pixel };

    // Constructors and destructors

    Bitmap(int w = 0, int h = 0);               // Normal
    Bitmap(int w, int h, int p, Pixel * pMap);      // Reference bitmap
    Bitmap(Bitmap const & rval);                // Copy constructor
    virtual ~Bitmap();                          // Destructor

    // Assignment operator
    Bitmap & operator =(Bitmap const & rval);

    // Load this bitmap from a memory buffer
    void Import(int w, int h, int p, Pixel const * pMap);

    // Reference a memory buffer through this bitmap
    void Reference(int w, int h, int p, Pixel * pMap);

    // Draw a section of this bitmap onto another
    void Draw(Rect const & srcRect, Bitmap * pDst, int pDx, int pDy) const;

    // Returns a pointer to the pixel at (x, y)
    Pixel const * GetMap(int x = 0, int y = 0) const { return _GetMap(x, y); }
    Pixel       * GetMap(int x = 0, int y = 0)        { return const_cast<Pixel *>(_GetMap(x, y)); }

    // Returns the width of the bitmap (in pixels)
    int GetWidth() const { return m_Width; }

    // Returns the height of the bitmap (in pixels)
    int GetHeight() const { return m_Height; }

    // Returns the pitch of the bitmap (in bytes)
    int GetPitch() const { return m_Pitch; }

protected:

    // Clips a rect so that the rect lies entirely within both the source and
    // dest bitmaps. If the source m_X or m_Y changes then so must the destinaion m_X or m_Y.
    void ClipForDrawing(Rect * srcRect, int * pDx, int * pDy, int dw, int dh) const;

    int m_Width;                // m_Width in pixels
    int m_Height;               // m_Height
    int m_Pitch;                // m_Pitch (in bytes)

    Pixel * m_Map;              // Bitmap image data

private:

    // Returns a pointer to the pixel at (x,y)
    Pixel const * _GetMap(int x, int y) const;

    // If true then this bitmap does not "own" the image data and should
    // not delete it.
    bool m_IsAReference;
};

template <class _Pixel>
Bitmap<_Pixel>::Bitmap(int w /*= 0*/, int h /*= 0*/)
    : m_IsAReference(false)
{
    assert(w >= 0);
    assert(h >= 0);

    if (w != 0 && h != 0)
    {
        m_Width  = w;
        m_Height = h;
        m_Pitch  = w * PIXEL_SIZE;
        m_Map    = new Pixel[w * h];
        if (!m_Map)
            throw std::bad_alloc();
    }
    else
    {
        m_Width  = 0;
        m_Height = 0;
        m_Pitch  = 0;
        m_Map    = 0;
    }
}

template <class _Pixel>
Bitmap<_Pixel>::Bitmap(int w, int h, int p, Pixel * pMap)
    : m_Width(w),
    m_Height(h),
    m_Pitch(p),
    m_Map(pMap),
    m_IsAReference(true)
{
    assert(pMap);
    assert(w > 0);
    assert(h > 0);
    assert(p >= w * sizeof(Pixel));
}

template <class _Pixel>
Bitmap<_Pixel>::Bitmap(Bitmap const & rval)
{
    m_Width        = rval.m_Width;
    m_Height       = rval.m_Height;
    m_Pitch        = rval.m_Pitch;
    m_IsAReference = rval.m_IsAReference;

    if (m_IsAReference)
    {
        m_Map = rval.m_Map;
    }
    else
    {
        if (m_Width != 0 && m_Height != 0)
        {
            m_Map = reinterpret_cast<Pixel *>(new char[m_Pitch * m_Height]);
            if (!m_Map)
                throw std::bad_alloc();

            memcpy(m_Map, rval.m_Map, m_Pitch * m_Height);
        }
        else
        {
            m_Map = 0;
        }
    }
}

template <class _Pixel>
Bitmap<_Pixel> & Bitmap<_Pixel>::operator =(Bitmap const & rval)
{
    if (this == &rval)
        return *this;

    if (!m_IsAReference && m_Map)
        delete[] m_Map;

    m_Width        = rval.m_Width;
    m_Height       = rval.m_Height;
    m_Pitch        = rval.m_Pitch;
    m_IsAReference = rval.m_IsAReference;

    if (m_IsAReference)
    {
        m_Map = rval.m_Map;
    }
    else
    {
        if (m_Width != 0 && m_Height != 0)
        {
            m_Map = reinterpret_cast<Pixel *>(new char[m_Pitch * m_Height]);
            if (!m_Map)
                throw std::bad_alloc();

            memcpy(m_Map, rval.m_Map, m_Pitch * m_Height);
        }
        else
        {
            m_Map = 0;
        }
    }

    return *this;
}

template <class _Pixel>
Bitmap<_Pixel>::~Bitmap()
{
    if (!m_IsAReference && m_Map)
        delete[] m_Map;
}

template <class _Pixel>
void Bitmap<_Pixel>::Import(int w, int h, int p, Pixel const * pMap)
{
    assert(pMap);
    assert(w > 0);
    assert(h > 0);
    assert(p >= w * sizeof(Pixel));

    if (!m_IsAReference && m_Map)
        delete[] m_Map;

    m_Width        = w;
    m_Height       = h;
    m_Pitch        = p;
    m_IsAReference = false;

    m_Map = reinterpret_cast<Pixel *>(new char[p * h]);
    if (!m_Map)
        throw std::bad_alloc();

    memcpy(m_Map, pMap, p * h);
}

template <class _Pixel>
void Bitmap<_Pixel>::Reference(int w, int h, int p, Pixel * pMap)
{
    assert(pMap);
    assert(w > 0);
    assert(h > 0);
    assert(p >= w * sizeof(Pixel));

    if (!m_IsAReference && m_Map)
        delete[] m_Map;

    m_Width        = w;
    m_Height       = h;
    m_Pitch        = p;
    m_Map          = pMap;
    m_IsAReference = true;
}

template <class _Pixel>
inline Bitmap<_Pixel>::Pixel const * Bitmap<_Pixel>::_GetMap(int x, int y) const
{
    return reinterpret_cast<Pixel *>(reinterpret_cast<char *>(m_Map) + y * m_Pitch) + x;
}

template <class _Pixel>
void Bitmap<_Pixel>::Draw(Rect const & srcRect, Bitmap * pDst, int pDx, int pDy) const
{
    Rect rect = srcRect;

    ClipForDrawing(&rect, &pDx, &pDy, pDst->GetWidth(), pDst->GetHeight());

    if (rect.m_Width <= 0 || rect.m_Height <= 0)
        return;

    Pixel * dstp       = pDst->GetMap(pDx, pDy);
    Pixel const * srcp = GetMap(rect.m_X, rect.m_Y);
    int const dpitch   = pDst->GetPitch();

    for (int i = 0; i < rect.m_Height; i++)
    {
        memcpy(dstp, srcp, rect.m_Width * PIXEL_SIZE);

        dstp = reinterpret_cast<Pixel *>(reinterpret_cast<char *>(dstp) + dpitch);
        srcp = reinterpret_cast<Pixel const *>(reinterpret_cast<char const *>(srcp) + m_Pitch);
    }
}

template <class _Pixel>
void Bitmap<_Pixel>::ClipForDrawing(Rect * srcRect, int * pDx, int * pDy, int dw, int dh) const
{
    // Clip to source bitmap

    if (srcRect->m_X < 0)
    {
        srcRect->m_Width += srcRect->m_X;
        pDx -= srcRect->m_X;
        srcRect->m_X      = 0;
    }

    if (srcRect->m_Y < 0)
    {
        srcRect->m_Height += srcRect->m_Y;
        pDy -= srcRect->m_Y;
        srcRect->m_Y       = 0;
    }

    if (srcRect->m_Width > m_Width - srcRect->m_X)
    {
        srcRect->m_Width = m_Width - srcRect->m_X;
    }

    if (srcRect->m_Height > m_Height - srcRect->m_Y)
    {
        srcRect->m_Height = m_Height - srcRect->m_Y;
    }

    // Clip to dest bitmap

    int ldx = *pDx;
    int ldy = *pDy;

    if (ldx < 0)
    {
        srcRect->m_Width += ldx;
        srcRect->m_X     -= ldx;
        ldx = 0;
    }

    if (ldy < 0)
    {
        srcRect->m_Height += ldy;
        srcRect->m_X      -= ldy;
        ldy = 0;
    }

    *pDx = ldx;
    *pDy = ldy;

    if (srcRect->m_Width > dw - ldx)
    {
        srcRect->m_Width = dw - ldx;
    }

    if (srcRect->m_Height > dh - ldy)
    {
        srcRect->m_Height = dh - ldy;
    }
}

typedef Bitmap<Pixel8> Bitmap8;
typedef Bitmap<Pixel16> Bitmap16;
typedef Bitmap<Pixel24> Bitmap24;
typedef Bitmap<Pixel32> Bitmap32;

#endif // !defined( BITMAP_H_INCLUDED )
