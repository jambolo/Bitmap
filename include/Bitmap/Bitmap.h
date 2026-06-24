#if !defined(BITMAP_BITMAP_H)
#define BITMAP_BITMAP_H

#pragma once

#include "Bitmap/Palette.h"
#include "Bitmap/Pixel.h"
#include "Rect/Rect.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <vector>

//! An image type.
template <typename Pixel>
class Bitmap
{
public:

    //! Pixel type.
    using PixelType = Pixel;

    //! Size of a pixel in bytes
    static size_t constexpr PIXEL_SIZE = sizeof(Pixel);

    //! Constructor.
    Bitmap() = default;

    //! Constructor.
    Bitmap(int width, int height, size_t pitch = 0, Pixel const * data = nullptr);

    //! Loads an image
    void load(int width, int height, size_t pitch, Pixel const * data);

    //! Erases the image data, resetting the width, height, and pitch to 0.
    void clear();

    //! Returns the pixel at (y, x)
    Pixel pixel(int x, int y) const { Pixel const * p = addressOf(x, y); return p ? *p : Pixel(); }

    //! Returns the width of the bitmap (in pixels).
    int width() const { return width_; }

    //! Returns the height of the bitmap (in pixels).
    int height() const { return height_; }

    //! Returns the pitch of the bitmap (in bytes).
    size_t pitch() const { return pitch_; }

    //! Returns a pointer to the pixel at (y, x).
    Pixel const * data(int x = 0, int y = 0) const { return addressOf(x, y); }

    //! Returns a pointer to the pixel at (y, x).
    Pixel * data(int x = 0, int y = 0) { return const_cast<Pixel *>(addressOf(x, y)); }

    //! Creates a bitmap from a region of this bitmap.
    Bitmap region(int x, int y, int width, int height, size_t pitch = 0) const;

    //! Copies a section of another bitmap into this bitmap
    void copy(Bitmap const & src, Rect const & srcRect, int dstX, int dstY);

protected:

    int width_    = 0;              //!< Width (in pixels)
    int height_   = 0;              //!< Height (in pixels)
    size_t pitch_ = 0;              //!< Pitch (in bytes)
    std::vector<std::byte> data_;   //!< Bitmap image data

private:

    // Clips a rect so that it lies entirely within both the source and destination bitmaps.
    static void clip(Rect * srcRect, int srcW, int srcH, int * dstX, int * dstY, int dstW, int dstH);

    // Returns a pointer to the pixel at (y,x)
    Pixel const * addressOf(int x, int y) const;

};

//! @param  width       Width of the image data (in pixels)
//! @param  height      Height of the image data (in pixels)
//! @param  pitch       Pitch of the image data (in bytes), or 0 if it is determined by the width
//! @param  data        Image data
template <class Pixel>
Bitmap<Pixel>::Bitmap(int width, int height, size_t pitch /*= 0*/, Pixel const * data /*= nullptr*/)
    : width_(width)
    , height_(height)
    , pitch_((pitch > 0) ? pitch : width * PIXEL_SIZE)
{
    assert(width >= 0);
    assert(height >= 0);
    assert(pitch == 0 || pitch >= width * PIXEL_SIZE);
    assert(pitch % PIXEL_SIZE == 0);

    if (pitch_ > 0 && height_ > 0)
    {
        data_.resize(pitch_ * height_);
        if (data)
        {
            std::byte const * src  = reinterpret_cast<std::byte const *>(data);
            size_t            size = pitch_ * height_;
            std::copy(src, src + size, data_.data());
        }
    }
    else
    {
        width_  = 0;
        height_ = 0;
        pitch_  = 0;
    }
}

//! @param  width       Width of the replacement image (in pixels)
//! @param  height      Height of the replacement image (in pixels)
//! @param  pitch       Pitch of the replacement data (in bytes), or 0 if it is determined by the width
//! @param  data        Image data to load
template <class Pixel>
void Bitmap<Pixel>::load(int width, int height, size_t pitch, Pixel const * data)
{
    assert(width > 0);
    assert(height > 0);
    assert(pitch == 0 || pitch >= width * PIXEL_SIZE);
    assert(data);

    width_  = width;
    height_ = height;
    pitch_  = (pitch > 0) ? pitch : width * PIXEL_SIZE;
    data_.resize(pitch_ * height_);
    std::byte const * src = reinterpret_cast<std::byte const *>(data);
    size_t            size = pitch_ * height_;
    std::copy(src, src + size, data_.data());
}

template<class Pixel> void Bitmap<Pixel>::clear()
{
    width_  = 0;
    height_ = 0;
    pitch_  = 0;
    data_.clear();
}

//! @param  x           Left of the region
//! @param  y           Top of the region
//! @param  width       Width of the region
//! @param  height      Height of the region
//! @param  pitch       Pitch of the resulting bitmap, or 0 if determined by width
//!
//! @return     resulting bitmap
template <class Pixel>
Bitmap<Pixel> Bitmap<Pixel>::region(int x, int y, int width, int height, size_t pitch /*= 0*/) const
{
    Rect srcRect{ 0, 0, width_, height_ };
    srcRect.clip(Rect{ x, y, width, height });
    Bitmap<Pixel> r(srcRect.width, srcRect.height, pitch);
    r.copy(*this, srcRect, 0, 0);
    return r;
}

//! @param  src         Source bitmap
//! @param  srcRect     Region of the source bitmap to copy
//! @param  dstY        Where to place the copy
//! @param  dstX        Where to place the copy
template <class Pixel>
void Bitmap<Pixel>::copy(Bitmap const & src, Rect const & srcRect, int dstY, int dstX)
{
    Rect clippedSrcRect = srcRect;
    clip(&clippedSrcRect, src.width(), src.height(), &dstX, &dstY, width_, height_);

    if (clippedSrcRect.width <= 0 || clippedSrcRect.height <= 0)
        return;

    int srcY = clippedSrcRect.y;
    int srcX = clippedSrcRect.x;
    int rowSize = clippedSrcRect.width * PIXEL_SIZE;
    for (int i = 0; i < clippedSrcRect.height; ++i)
    {
        std::byte *       d = reinterpret_cast<std::byte *>(data(dstX, dstY));
        std::byte const * s = reinterpret_cast<std::byte const *>(src.data(srcX, srcY));
        std::copy(s, s + rowSize, d);
        ++srcY;
        ++dstY;
    }
}

// Bitmap<Pixel>::clip adjusts the rects used to copy from an arbitrary region in a source bitmap to a location in a
// destination bitmap.
// 
// Parameters:
//  - a source rectangle(*srcRect) inside a source bitmap of size (srcW, srcH)
//  - a destination placement (*dstX, *dstY) inside a destination bitmap of size (dstW, dstH)
//
// It mutates *srcRect, *dstX, and *dstY so a later copy only operates on valid pixels in the source bitmap.
template<class Pixel>
void Bitmap<Pixel>::clip(Rect * srcRect,
                         int srcW,
                         int srcH,
                         int * dstX,
                         int * dstY,
                         int dstW,
                         int dstH)
{
    Rect rect = *srcRect;
    int  dx   = *dstX;
    int  dy   = *dstY;

    // Clip the source rect to the source bitmap dimensions (0, 0, srcW, srcH)

    if (rect.x < 0)
    {
        rect.width += rect.x;
        dx         -= rect.x; // The origin of the source rect moved so the destination must move too
        rect.x      = 0;
    }

    if (rect.y < 0)
    {
        rect.height += rect.y;
        dy          -= rect.y; // The origin of the source rect moved so the destination must move too
        rect.y       = 0;
    }

    if (rect.x + rect.width > srcW)
        rect.width = srcW - rect.x;

    if (rect.y + rect.height > srcH)
        rect.height = srcH - rect.y;

    // Clip the translated source rect to the destination bitmap dimensions(0, 0, dstW, dstH)
    // Ensure that the destination coordinates are non-negative, and adjust the source rect accordingly.

    if (dx < 0)
    {
        rect.width += dx;
        rect.x     -= dx;
        dx          = 0;
    }

    if (dy < 0)
    {
        rect.height += dy;
        rect.y      -= dy;
        dy           = 0;
    }

    if (dx + rect.width > dstW)
        rect.width = dstW - dx;

    if (dy + rect.height > dstH)
        rect.height = dstH - dy;

    *srcRect = rect;
    *dstX    = dx;
    *dstY    = dy;
}

template <class Pixel>
Pixel const * Bitmap<Pixel>::addressOf(int x, int y) const
{
    if (data_.empty())
        return nullptr;

    assert(x < width_);
    assert(y < height_);

    size_t offset = y * pitch_ + x * PIXEL_SIZE;
    return reinterpret_cast<Pixel const *>(data_.data() + offset);
}

#endif // !defined(BITMAP_BITMAP_H)
