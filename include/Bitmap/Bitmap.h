#if !defined(BITMAP_BITMAP_H)
#define BITMAP_BITMAP_H

#pragma once

#include "Bitmap/Palette.h"
#include "Bitmap/Pixel.h"
#include "Rect/Rect.h"

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
    Bitmap(int w, int h, int p = 0);

    //! Constructor.
    Bitmap(int w, int h, int p, Pixel * data);

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
    void load(int w, int h, int p, Pixel const * data);

    //! References an image
    void reference(int w, int h, int p, Pixel * data);

    //! Returns the pixel at (row, column)
    Pixel pixel(int row, int column) const { return *addressOf(row, column); }

    //! Returns a pointer to the pixel at (row, column).
    Pixel const * data(int row = 0, int column = 0) const { return addressOf(row, column); }

    //! Returns a pointer to the pixel at (row, column).
    Pixel       * data(int row = 0, int column = 0)       { return const_cast<Pixel *>(addressOf(row, column)); }

    //! Returns the width of the bitmap (in pixels).
    int width() const { return width_; }

    //! Returns the height of the bitmap (in pixels).
    int height() const { return height_; }

    //! Returns the pitch of the bitmap (in bytes).
    int pitch() const { return pitch_; }

    //! Creates a bitmap from a region of this bitmap.
    Bitmap region(int row, int column, int width, int height) const;

    //! Copies a section of another bitmap into this bitmap
    void copy(Bitmap const & src, Rect const & srcRect, int dstC, int dstR);

protected:

    int width_   = 0;       //!< Width (in pixels)
    int height_  = 0;       //!< Height (in pixels)
    int pitch_   = 0;       //!< Pitch (in bytes)
    bool referenced_ = false; //!< If true, then this bitmap does not "own" the image data
    Pixel * data_ = nullptr; //!< Bitmap image data

private:

    // Clips a rect so that it lies entirely within both the source and dest bitmaps.
    void clipToFit(Rect * srcRect, int srcW, int srcH, int * dstR, int * dstC) const;

   // Returns a pointer to the pixel at (row,column)
    Pixel const * addressOf(int row, int column) const;

    // Creates a copy of image data
    static Pixel * createCopy(int pitch, int height, Pixel const * data);
};

template <class Pixel>
Bitmap<Pixel>::Bitmap(int w, int h, int p /*= 0*/)
    : width_(w)
    , height_(h)
    , pitch_((p > 0) ? p : w * PIXEL_SIZE)
    , referenced_(true)
    , data_(reinterpret_cast<Pixel *>(new char[pitch_ * height_]))
{
    assert(w > 0);
    assert(h > 0);
    assert(p == 0 || p >= w * PIXEL_SIZE);
    assert(p % PIXEL_SIZE == 0);
}

template <class Pixel>
Bitmap<Pixel>::Bitmap(int w, int h, int p, Pixel * data)
    : width_(w)
    , height_(h)
    , pitch_((p > 0) ? p : w * PIXEL_SIZE)
    , referenced_(true)
    , data_(data)
{
    assert(w > 0);
    assert(h > 0);
    assert(p == 0 || p >= w * PIXEL_SIZE);
    assert(data);
    assert(p % PIXEL_SIZE == 0);
}

template <class Pixel>
Bitmap<Pixel>::Bitmap(Bitmap const & rhs)
    : width_(rhs.width_)
    , height_(rhs.height_)
    , pitch_(rhs.pitch_)
    , referenced_(rhs.referenced_)
{
    if (referenced_)
        data_ = rhs.data_;
    else if (pitch_ > 0 && height_ > 0)
        data_ = createCopy(rhs.data_, pitch_, height_);
    else
        data_ = nullptr;
}

template <class Pixel>
Bitmap<Pixel>::Bitmap(Bitmap && rhs)
    : width_(rhs.width_)
    , height_(rhs.height_)
    , pitch_(rhs.pitch_)
    , referenced_(rhs.referenced_)
    , data_(rhs.data_)
{
    rhs.referenced_ = true;
}

template <class Pixel>
Bitmap<Pixel>::~Bitmap()
{
    if (!referenced_ && data_)
        delete[] data_;
}

template <class Pixel>
Bitmap<Pixel> & Bitmap<Pixel>::operator =(Bitmap const & rhs)
{
    if (this == &rhs)
        return *this;

    if (!referenced_ && data_)
        delete[] data_;

    width_      = rhs.width_;
    height_     = rhs.height_;
    pitch_      = rhs.pitch_;
    referenced_ = rhs.referenced_;
    if (referenced_)
        data_ = rhs.data_;
    else if (pitch_ > 0 && height_ > 0)
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

    if (!referenced_ && data_)
        delete[] data_;

    width_      = rhs.width_;
    height_     = rhs.height_;
    pitch_      = rhs.pitch_;
    referenced_ = rhs.referenced_;
    data_        = rhs.data_;

    if (!rhs.referenced_)
        rhs.data_ = nullptr;

    return *this;
}

template <class Pixel>
void Bitmap<Pixel>::load(int w, int h, int p, Pixel const * data)
{
    assert(w > 0);
    assert(h > 0);
    assert(p == 0 || p >= w * PIXEL_SIZE);
    assert(data);

    if (!referenced_ && data_)
        delete[] data_;

    width_      = w;
    height_     = h;
    pitch_      = (p > 0) ? p : w * PIXEL_SIZE;
    referenced_ = false;
    data_ = createCopy(data, p, h);
}

template <class Pixel>
void Bitmap<Pixel>::reference(int w, int h, int p, Pixel * data)
{
    assert(data);
    assert(w > 0);
    assert(h > 0);
    assert(p == 0 || p >= w * PIXEL_SIZE);

    if (!referenced_ && data_)
        delete[] data_;

    width_ = w;
    height_ = h;
    pitch_ = (p > 0) ? p : w * PIXEL_SIZE;
    referenced_ = true;
    data_ = data;
}

template <class Pixel>
Bitmap<Pixel> Bitmap<Pixel>::region(int row, int column, int width, int height, int pitch = 0) const
{
    Bitmap<Pixel> region(width, height, pitch);
    Rect srcRect(column, row, width, height);
    region.copy(*this, srcRect, 0, 0);
    return region;
}

template <class Pixel>
void Bitmap<Pixel>::copy(Bitmap const & src, Rect const & srcRect, int dstR, int dstC)
{
    Rect rect = srcRect;
    clipToFit(&rect, width_, height_, &dstR, &dstC);

    if (rect.width <= 0 || rect.height <= 0)
        return;

    int srcR = rect.row;
    int srcC = rect.column;
    for (int i = 0; i < rect.height; ++i)
    {
        Pixel * d       = const_cast<Pixel *>(addressOf(dstR, dstC));
        Pixel const * s = src.data(srcR, srcC);
        memcpy(d, s, rect.width * PIXEL_SIZE);
        ++srcR;
        ++dstR;
    }
}

//!
//! @note   If the source x or y changes then so must the destinaion x or y.
template <class Pixel>
void Bitmap<Pixel>::clipToFit(Rect * srcRect, int srcW, int srcH, int * dstR, int * dstC) const
{
    Rect rect = *srcRect;
    int dc = *dstC;
    int dr = *dstR;

    // Clip to source bitmap

    if (rect.x < 0)
    {
        rect.width += rect.x;
        dc -= rect.x;
        rect.x = 0;
    }

    if (rect.y < 0)
    {
        rect.height += rect.y;
        dr -= rect.y;
        rect.y = 0;
    }

    if (rect.width > srcW - rect.x)
        rect.width = srcW - rect.x;

    if (rect.height > srcH - rect.y)
        rect.height = srcH - rect.y;

    // Clip to this bitmap

    if (dc < 0)
    {
        rect.width_ += dc;
        rect.x -= dc;
        dc = 0;
    }

    if (dr < 0)
    {
        rect.height_ += dr;
        rect.y -= dr;
        dr = 0;
    }

    if (rect.width > width_ - dc)
        rect.width = width_ - dc;

    if (rect.height > height_ - dr)
        rect.height = height_ - dr;

    *srcRect = rect;
    *dstC = dc;
    *dstR = dr;
}

template <class Pixel>
Pixel const * Bitmap<Pixel>::addressOf(int row, int column) const
{
    return reinterpret_cast<Pixel const *>(reinterpret_cast<char const *>(data_) + row * pitch_) + column;
}

template <class Pixel>
Pixel * Bitmap<Pixel>::createCopy(Pixel * data, int pitch, int height)
{
    Pixel * copy = reinterpret_cast<Pixel *>(new char[pitch * height]);
    memcpy(copy, data, pitch * height);
}

using Bitmap8 = Bitmap<Pixel8>;
using Bitmap16 = Bitmap<Pixel16>;
using Bitmap24 = Bitmap<Pixel24>;
using Bitmap32 = Bitmap<Pixel32>;

#endif // !defined(BITMAP_BITMAP_H)
