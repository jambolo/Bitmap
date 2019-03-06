#if !defined(BITMAP_BITMAP_H)
#define BITMAP_BITMAP_H

#pragma once

#include "Bitmap/Palette.h"
#include "Bitmap/Pixel.h"
#include "Rect/Rect.h"
#include <cassert>
#include <cstddef>
#include <cstring>

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

    //! Copy constructor.
    Bitmap(Bitmap const & rhs);

    //! Move constructor.
    Bitmap(Bitmap && rhs);

    //! Destructor.
    virtual ~Bitmap();

    //! Assignment operator
    Bitmap & operator =(Bitmap const & rhs);

    //! Move assignment operator
    Bitmap & operator =(Bitmap && rhs);

    //! Loads an image
    void load(int width, int height, size_t pitch, Pixel const * data);

#if 0
    //! References an image
    void reference(int width, int height, size_t pitch, Pixel * data);
#endif

    //! Returns the pixel at (y, x)
    Pixel pixel(int x, int y) const { return data_ ? *addressOf(x, y) : Pixel(); }

    //! Returns the width of the bitmap (in pixels).
    int width() const { return width_; }

    //! Returns the height of the bitmap (in pixels).
    int height() const { return height_; }

    //! Returns the pitch of the bitmap (in bytes).
    size_t pitch() const { return pitch_; }

#if 0
    //! Returns true if the bitmap references an external image.
    bool referenced() const { return referenced_; }
#endif

    //! Returns a pointer to the pixel at (y, x).
    Pixel const * data(int x = 0, int y = 0) const { return addressOf(x, y); }

    //! Returns a pointer to the pixel at (y, x).
    Pixel * data(int x = 0, int y = 0) { return const_cast<Pixel *>(addressOf(x, y)); }

    //! Creates a bitmap from a region of this bitmap.
    Bitmap region(int x, int y, int width, int height, size_t pitch = 0) const;

    //! Copies a section of another bitmap into this bitmap
    void copy(Bitmap const & src, Rect const & srcRect, int dstX, int dstY);

protected:

    int width_    = 0;          //!< Width (in pixels)
    int height_   = 0;          //!< Height (in pixels)
    size_t pitch_ = 0;          //!< Pitch (in bytes)
#if 0
    bool referenced_ = false;   //!< If true, then this bitmap does not "own" the image data
#endif
    Pixel * data_ = nullptr;    //!< Bitmap image data

private:

    // Clips a rect so that it lies entirely within both the source and destination bitmaps.
    static void clip(Rect * srcRect, int srcW, int srcH, int * dstX, int * dstY, int dstW, int dstH);

    // Returns a pointer to the pixel at (y,x)
    Pixel const * addressOf(int x, int y) const;

    // Creates a copy of image data
    static Pixel * createCopy(Pixel const * data, size_t pitch, int height);
};

//! @param  width       Width
//! @param  height      Height
//! @param  pitch       Pitch of the referenced image data, or 0 if it is determined by the width
//! @param  data        Referenced image data
template <class Pixel>
Bitmap<Pixel>::Bitmap(int width, int height, size_t pitch /*= 0*/, Pixel const * data /*= nullptr*/)
    : width_(width)
    , height_(height)
    , pitch_((pitch > 0) ? pitch : width * PIXEL_SIZE)
#if 0
    , referenced_(true)
#endif
{
    assert(width >= 0);
    assert(height >= 0);
    assert(pitch == 0 || pitch >= width * PIXEL_SIZE);
    assert(pitch % PIXEL_SIZE == 0);
    if (pitch_ > 0 && height_ > 0)
    {
        data_ = reinterpret_cast<Pixel *>(new char[pitch_ * height_]);
        if (data)
            memcpy(data_, data, pitch_ * height_);
    }
    else
    {
        width_  = 0;
        height_ = 0;
        pitch_  = 0;
    }
}

template <class Pixel>
Bitmap<Pixel>::Bitmap(Bitmap const & rhs)
    : width_(rhs.width_)
    , height_(rhs.height_)
    , pitch_(rhs.pitch_)
#if 0
    , referenced_(rhs.referenced_)
#endif
{
#if 0
    if (referenced_)
        data_ = rhs.data_;
    else
#endif
    if (rhs.data_ && pitch_ > 0 && height_ > 0)
        data_ = createCopy(rhs.data_, pitch_, height_);
}

template <class Pixel>
Bitmap<Pixel>::Bitmap(Bitmap && rhs)
    : width_(rhs.width_)
    , height_(rhs.height_)
    , pitch_(rhs.pitch_)
#if 0
    , referenced_(rhs.referenced_)
#endif
    , data_(rhs.data_)
{
#if 0
    if (rhs.data_)
        rhs.referenced_ = true;
#else
    rhs.width_  = 0;
    rhs.height_ = 0;
    rhs.pitch_  = 0;
    rhs.data_   = nullptr;
#endif
}

template <class Pixel>
Bitmap<Pixel>::~Bitmap()
{
#if 0
    if (!referenced_ && data_)
        delete[] data_;
#else
    delete[] data_;
#endif
}

//! @note   The constructor creates a reference bitmap if the source is a reference bitmap.
template <class Pixel>
Bitmap<Pixel> & Bitmap<Pixel>::operator =(Bitmap const & rhs)
{
    if (this == &rhs)
        return *this;

#if 0
    if (!referenced_ && data_)
        delete[] data_;
#else
    delete[] data_;
#endif

    width_  = rhs.width_;
    height_ = rhs.height_;
    pitch_  = rhs.pitch_;
#if 0
    referenced_ = rhs.referenced_;
    if (referenced_)
        data_ = rhs.data_;
    else
#endif
    if (rhs.data_ && pitch_ > 0 && height_ > 0)
        data_ = createCopy(rhs.data_, pitch_, height_);
    else
        data_ = nullptr;

    return *this;
}

template <class Pixel>
Bitmap<Pixel> & Bitmap<Pixel>::operator =(Bitmap && rhs)
{
    if (this == &rhs)
        return *this;

#if 0
    if (!referenced_ && data_)
        delete[] data_;
#else
    delete[] data_;
#endif

    width_  = rhs.width_;
    height_ = rhs.height_;
    pitch_  = rhs.pitch_;
#if 0
    referenced_ = rhs.referenced_;
#endif
    data_ = rhs.data_;

#if 0
    if (!rhs.referenced_)
        rhs.data_ = nullptr;
#else
    rhs.width_  = 0;
    rhs.height_ = 0;
    rhs.pitch_  = 0;
    rhs.data_   = nullptr;
#endif

    return *this;
}

//! This function replaces the current image with the specified data.
//!
//! @param  width       New width
//! @param  height      New height
//! @param  pitch       Pitch of the loaded data, or 0 if it is determined by the width
//! @param  data        Image data to load
template <class Pixel>
void Bitmap<Pixel>::load(int width, int height, size_t pitch, Pixel const * data)
{
    assert(width > 0);
    assert(height > 0);
    assert(pitch == 0 || pitch >= width * PIXEL_SIZE);
    assert(data);

#if 0
    if (!referenced_ && data_)
        delete[] data_;
#else
    delete[] data_;
#endif

    width_  = width;
    height_ = height;
    pitch_  = (pitch > 0) ? pitch : width * PIXEL_SIZE;
#if 0
    referenced_ = false;
#endif
    data_ = createCopy(data, pitch_, height_);
}

#if 0
//! This function replaces the current image with a reference to the specified data.
//!
//! @param  width       New width
//! @param  height      New height
//! @param  pitch       Pitch of the referenced image, or 0 if it is determined by the width
//! @param  data        Referenced image data
//!
//! @return void
template <class Pixel>
void Bitmap<Pixel>::reference(int width, int height, size_t pitch, Pixel * data)
{
    assert(data);
    assert(width > 0);
    assert(height > 0);
    assert(pitch == 0 || pitch >= width * PIXEL_SIZE);

    if (!referenced_ && data_)
        delete[] data_;

    width_      = width;
    height_     = height;
    pitch_      = (pitch > 0) ? pitch : width * PIXEL_SIZE;
    referenced_ = true;
    data_       = data;
}
#endif // if 0

//! This function creates a bitmap that references a region in this bitmap. Use copy() to create a copy of the region.
//!
//! @param  x           Left of the region
//! @param  y           Top of the region
//! @param  width       Width of the region
//! @param  height      Height of the region
//!
//! @return     reference to this bitmap

template <class Pixel>
Bitmap<Pixel> Bitmap<Pixel>::region(int x, int y, int width, int height, size_t pitch /*= 0*/) const
{
    Rect srcRect(0, 0, width_, height_);
    srcRect.clip(Rect(x, y, width, height));
#if 0
    return Bitmap<Pixel>(srcRect.width, srcRect.height, pitch_, addressOf(srcRect.x, srcRect.y), pitch);
#else
    Bitmap<Pixel> r(srcRect.width, srcRect.height, pitch);
    r.copy(*this, srcRect, 0, 0);
    return r;
#endif
}

//! @param  src         Source bitmap
//! @param  srcRect     Region of the source bitmap to copy
//! @param  dstY        Where to place the copy
//! @param  dstX        Where to place the copy
template <class Pixel>
void Bitmap<Pixel>::copy(Bitmap const & src, Rect const & srcRect, int dstY, int dstX)
{
    Rect rect = srcRect;
    clip(&rect, src.width(), src.height(), &dstX, &dstY, width_, height_);

    if (rect.width <= 0 || rect.height <= 0)
        return;

    int srcY = rect.y;
    int srcX = rect.x;
    for (int i = 0; i < rect.height; ++i)
    {
        Pixel *       d = const_cast<Pixel *>(addressOf(dstX, dstY));
        Pixel const * s = src.data(srcX, srcY);
        memcpy(d, s, rect.width * PIXEL_SIZE);
        ++srcY;
        ++dstY;
    }
}

//!
//! @note   If the source x or y changes then so must the destination x or y.
template <class Pixel>
void Bitmap<Pixel>::clip(Rect * srcRect, int srcW, int srcH, int * dstX, int * dstY, int dstW, int dstH)
{
    assert(srcW >= 0);
    assert(srcH >= 0);
    assert(dstW >= 0);
    assert(dstH >= 0);

    Rect rect = *srcRect;
    int  dx   = *dstX;
    int  dy   = *dstY;

    // Clip to the source (0, 0, srcW, srcH)

    if (rect.x < 0)
    {
        rect.width += rect.x;
        dx         -= rect.x;
        rect.x      = 0;
    }

    if (rect.y < 0)
    {
        rect.height += rect.y;
        dy          -= rect.y;
        rect.y       = 0;
    }

    if (rect.width > srcW - rect.x)
        rect.width = srcW - rect.x;

    if (rect.height > srcH - rect.y)
        rect.height = srcH - rect.y;

    // Clip to the destination (0, 0, dstW, dstH)

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

    if (rect.width > dstW - dx)
        rect.width = dstW - dx;

    if (rect.height > dstH - dy)
        rect.height = dstH - dy;

    *srcRect = rect;
    *dstX    = dx;
    *dstY    = dy;
}

template <class Pixel>
Pixel const * Bitmap<Pixel>::addressOf(int x, int y) const
{
    assert(!data_ || (x >= 0 && x < width_));
    assert(!data_ || (y >= 0 && y < height_));

    if (!data_)
        return nullptr;

    return reinterpret_cast<Pixel const *>(reinterpret_cast<char const *>(data_) + y * pitch_) + x;
}

template <class Pixel>
Pixel * Bitmap<Pixel>::createCopy(Pixel const * data, size_t pitch, int height)
{
    Pixel * copy = reinterpret_cast<Pixel *>(new char[pitch * height]);
    memcpy(copy, data, pitch * height);
    return copy;
}

#endif // !defined(BITMAP_BITMAP_H)
