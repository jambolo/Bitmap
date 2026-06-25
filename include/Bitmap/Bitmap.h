#if !defined(BITMAP_BITMAP_H)
#define BITMAP_BITMAP_H

#pragma once

#include "Bitmap/detail/Clip.h"
#include "Bitmap/Palette.h"
#include "Bitmap/Pixel.h"
#include "Rect/Rect.h"
#include <cassert>
#include <cstddef>
#include <cstring>
#include <vector>

template <typename Pixel> class BitmapView;
template <typename Pixel> class BitmapConstView;

//! An owning, contiguous 2D image.
//!
//! Stores pixels row-major in a heap buffer. Rows are separated by the pitch (the byte stride from one row to the next), which may
//! exceed @c width*PIXEL_SIZE so rows can be padded for alignment. A default-constructed bitmap is empty (width, height, and pitch
//! are all 0).
//!
//! @tparam Pixel   Pixel type. Must be trivially copyable; @c sizeof(Pixel) is the pixel stride.
//!
//! @sa BitmapView, BitmapConstView
template <typename Pixel>
class Bitmap
{
public:

    //! Pixel type.
    using PixelType = Pixel;

    //! Size of a pixel in bytes.
    static size_t constexpr PIXEL_SIZE = sizeof(Pixel);

    //! Constructs an empty bitmap (width, height, and pitch are 0).
    Bitmap() = default;

    //! Constructs a bitmap, optionally copying initial pixel data.
    Bitmap(int width, int height, size_t pitch = 0, Pixel const * data = nullptr);

    //! Constructs an owning copy of a view.
    explicit Bitmap(BitmapConstView<Pixel> const & src);

    //! Replaces the contents with a copy of the given pixel data.
    void load(int width, int height, size_t pitch, Pixel const * data);

    //! Erases the image data, resetting the width, height, and pitch to 0.
    void clear();

    //! Returns the pixel at (x, y), or a default-constructed pixel if the bitmap is empty.
    //!
    //! @param  x   Column (0-based). Must be less than width() for a non-empty bitmap.
    //! @param  y   Row (0-based). Must be less than height() for a non-empty bitmap.
    //!
    //! @return     Copy of the pixel at (x, y), or @c Pixel() if the bitmap holds no data.
    Pixel pixel(int x, int y) const { Pixel const * p = addressOf(x, y); return p ? *p : Pixel(); }

    //! Returns the width of the bitmap (in pixels).
    int width() const { return width_; }

    //! Returns the height of the bitmap (in pixels).
    int height() const { return height_; }

    //! Returns the pitch of the bitmap, i.e. the byte stride between consecutive rows.
    size_t pitch() const { return pitch_; }

    //! Returns a read-only pointer to the pixel at (x, y), or @c nullptr if the bitmap is empty.
    //!
    //! @param  x   Column (0-based). Defaults to 0.
    //! @param  y   Row (0-based). Defaults to 0.
    Pixel const * data(int x = 0, int y = 0) const { return addressOf(x, y); }

    //! Returns a mutable pointer to the pixel at (x, y), or @c nullptr if the bitmap is empty.
    //!
    //! @param  x   Column (0-based). Defaults to 0.
    //! @param  y   Row (0-based). Defaults to 0.
    Pixel * data(int x = 0, int y = 0) { return const_cast<Pixel *>(addressOf(x, y)); }

    //! Returns an owning copy of a region of this bitmap as a new Bitmap.
    Bitmap region(int x, int y, int width, int height, size_t pitch = 0) const;

    //! Copies a region of another bitmap into this bitmap.
    void copy(Bitmap const & src, Rect const & srcRect, int dstX, int dstY);

    //! Replaces the contents with an owning copy of a view.
    void load(BitmapConstView<Pixel> const & src);

    //! Returns a mutable view of the entire bitmap.
    BitmapView<Pixel> view();

    //! Returns a read-only view of the entire bitmap.
    BitmapConstView<Pixel> view() const;

    //! Returns a mutable view of a region of the bitmap.
    BitmapView<Pixel> view(int x, int y, int w, int h);

    //! Returns a read-only view of a region of the bitmap.
    BitmapConstView<Pixel> view(int x, int y, int w, int h) const;

    //! Returns a read-only view of the entire bitmap.
    BitmapConstView<Pixel> cview() const;

protected:

    int width_    = 0;              //!< Width (in pixels)
    int height_   = 0;              //!< Height (in pixels)
    size_t pitch_ = 0;              //!< Pitch, i.e. byte stride between rows
    std::vector<std::byte> data_;   //!< Bitmap image data (row-major, pitch-strided)

private:

    // Returns a pointer to the pixel at (y,x)
    Pixel const * addressOf(int x, int y) const;

};

//! Allocates a @c pitch*height byte buffer. If @p data is non-null, its bytes are copied into the buffer; otherwise the buffer is
//! left default-initialized. If the resulting buffer would be empty (zero pitch or zero height), the bitmap is reset to empty.
//!
//! @param  width       Width of the image data (in pixels)
//! @param  height      Height of the image data (in pixels)
//! @param  pitch       Pitch of the image data (in bytes), or 0 to derive it from the width
//! @param  data        Source pixels to copy, or @c nullptr to leave the buffer uninitialized
//!
//! @pre    @p width >= 0, @p height >= 0
//! @pre    @p pitch is 0 or >= @p width * PIXEL_SIZE, and is a multiple of PIXEL_SIZE
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

//! Reallocates the buffer to @c pitch*height bytes and copies the supplied pixels into it, discarding any previous contents.
//!
//! @param  width       Width of the replacement image (in pixels)
//! @param  height      Height of the replacement image (in pixels)
//! @param  pitch       Pitch of the replacement data (in bytes), or 0 to derive it from the width
//! @param  data        Image data to load
//!
//! @pre    @p width > 0, @p height > 0, @p data is non-null
//! @pre    @p pitch is 0 or >= @p width * PIXEL_SIZE
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

//! Releases the image buffer and resets the width, height, and pitch to 0, leaving the bitmap empty.
template<class Pixel> void Bitmap<Pixel>::clear()
{
    width_  = 0;
    height_ = 0;
    pitch_  = 0;
    data_.clear();
}

//! The requested rectangle is clipped to this bitmap's bounds, so the returned bitmap may be smaller than @p width by @p height
//! (and empty if the rectangle lies entirely outside).
//!
//! @param  x           Left of the region
//! @param  y           Top of the region
//! @param  width       Width of the region
//! @param  height      Height of the region
//! @param  pitch       Pitch of the resulting bitmap (in bytes), or 0 to derive it from the width
//!
//! @return     A new bitmap holding a copy of the clipped region
template <class Pixel>
Bitmap<Pixel> Bitmap<Pixel>::region(int x, int y, int width, int height, size_t pitch /*= 0*/) const
{
    Rect srcRect{ 0, 0, width_, height_ };
    srcRect.clip(Rect{ x, y, width, height });
    Bitmap<Pixel> r(srcRect.width, srcRect.height, pitch);
    r.copy(*this, srcRect, 0, 0);
    return r;
}

//! The source rectangle and destination origin are clipped to both bitmaps' bounds, so portions that would fall outside either
//! bitmap are skipped. Does nothing if the clipped region is empty.
//!
//! @param  src         Source bitmap
//! @param  srcRect     Region of the source bitmap to copy
//! @param  dstX        Destination column at which to place the copy
//! @param  dstY        Destination row at which to place the copy
//!
//! @warning Source and destination buffers must not overlap.
template <class Pixel>
void Bitmap<Pixel>::copy(Bitmap const & src, Rect const & srcRect, int dstX, int dstY)
{
    Rect clippedSrcRect = srcRect;
    detail::clip(&clippedSrcRect, src.width(), src.height(), &dstX, &dstY, width_, height_);

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

// Returns a pointer to the pixel at (x, y), or @c nullptr if the bitmap is empty. The caller must ensure that (x, y) is within
// bounds.
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
