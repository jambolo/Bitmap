#if !defined(BITMAP_BITMAPVIEW_H)
#define BITMAP_BITMAPVIEW_H

#pragma once

#include "Bitmap/Bitmap.h"
#include "Bitmap/detail/Clip.h"
#include "Rect/Rect.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>

template <typename Pixel> class BitmapView;
template <typename Pixel> class BitmapConstView;

namespace detail
{
//! Shared, non-owning core for the bitmap view types.
//!
//! Holds the origin pointer, dimensions, and pitch that both BitmapView and BitmapConstView need, and implements the
//! pixel-addressing and clipping logic they share. The @p Ptr template parameter selects const-ness so a single implementation
//! serves both the mutable and read-only views.
//!
//! @tparam Pixel   Pixel type
//! @tparam Ptr     @c Pixel* (mutable view) or @c Pixel const* (const view)
template <typename Pixel, typename Ptr>
class BasicBitmapView
{
public:

	using PixelType = Pixel;									//!< Pixel type
	static std::size_t constexpr PIXEL_SIZE = sizeof(Pixel);	//!< Size of a pixel in bytes

	//! Returns the width of the view (in pixels).
	int width() const { return width_; }

	//! Returns the height of the view (in pixels).
	int height() const { return height_; }

	//! Returns the pitch (byte stride between rows), inherited from the source bitmap.
	std::size_t pitch() const { return pitch_; }

	//! Returns true if the view references no pixels (null origin or zero width/height).
	bool empty() const { return ptr_ == nullptr || width_ == 0 || height_ == 0; }

	//! Returns the pixel at (x, y), or a default-constructed pixel if the view is empty.
	//!
	//! @param  x   Column (0-based), in [0, width())
	//! @param  y   Row (0-based), in [0, height())
	//!
	//! @return     Copy of the pixel at (x, y), or @c Pixel() if the view holds no pixels
	Pixel pixel(int x, int y) const { Ptr p = addressOf(x, y); return p ? *p : Pixel(); }

	//! Returns a pointer to the pixel at (x, y), or @c nullptr if the view is empty.
	//!
	//! @param  x   Column (0-based). Defaults to 0.
	//! @param  y   Row (0-based). Defaults to 0.
	Ptr data(int x = 0, int y = 0) const { return addressOf(x, y); }

protected:

	//! Constructs an empty view (null origin, zero dimensions).
	BasicBitmapView() = default;

	//! Constructs a view over an existing pixel buffer.
	//!
	//! @param  ptr     Pointer to the top-left pixel of the region
	//! @param  w       Width of the region (in pixels)
	//! @param  h       Height of the region (in pixels)
	//! @param  pitch   Byte stride between rows of the underlying buffer
	BasicBitmapView(Ptr ptr, int w, int h, std::size_t pitch)
		: ptr_(ptr)
		, width_(w)
		, height_(h)
		, pitch_(pitch)
	{
	}

	//! Computes the address of the pixel at (x, y) using the view's pitch.
	//!
	//! @param  x   Column (0-based), in [0, width())
	//! @param  y   Row (0-based), in [0, height())
	//!
	//! @return     Pointer to the pixel at (x, y), or @c nullptr if the view has no origin
	Ptr addressOf(int x, int y) const
	{
		if (!ptr_)
			return nullptr;

		assert(x >= 0 && x < width_);
		assert(y >= 0 && y < height_);

		using ByteType = std::conditional_t<std::is_const_v<std::remove_pointer_t<Ptr>>, std::byte const, std::byte>;

		ByteType * base = reinterpret_cast<ByteType *>(ptr_);
        size_t     offset = static_cast<std::size_t>(y) * pitch_ + static_cast<std::size_t>(x) * PIXEL_SIZE;
		return reinterpret_cast<Ptr>(base + offset);
	}

	//! Clips a rectangle to this view and yields the origin and size of the resulting sub-region.
	//!
	//! @param      x       Left of the requested region
	//! @param      y       Top of the requested region
	//! @param      w       Width of the requested region
	//! @param      h       Height of the requested region
	//! @param[out] outPtr  Origin of the clipped sub-region, or @c nullptr if it is empty
	//! @param[out] outW    Width of the clipped sub-region (0 if empty)
	//! @param[out] outH    Height of the clipped sub-region (0 if empty)
	void subRegion(int x, int y, int w, int h, Ptr & outPtr, int & outW, int & outH) const
	{
		Rect r{ 0, 0, width_, height_ };
		r.clip(Rect{ x, y, w, h });
		if (r.empty())
		{
			outPtr = nullptr;
			outW   = 0;
			outH   = 0;
			return;
		}
		outPtr = addressOf(r.x, r.y);
		outW   = r.width;
		outH   = r.height;
	}

	Ptr         ptr_    = nullptr;  //!< Pointer to the top-left pixel, or null when empty
	int         width_  = 0;        //!< Width of the view (in pixels)
	int         height_ = 0;        //!< Height of the view (in pixels)
	std::size_t pitch_  = 0;        //!< Byte stride between rows of the underlying buffer
};
} // namespace detail

//! A read-only, non-owning reference to a region of a Bitmap.
//!
//! The referenced Bitmap must outlive the view. Copying a view is cheap (it copies only the origin,
//! dimensions, and pitch) and does not duplicate pixels.
//!
//! @tparam Pixel   Pixel type
template <typename Pixel>
class BitmapConstView : public detail::BasicBitmapView<Pixel, Pixel const *>
{
	using Base = detail::BasicBitmapView<Pixel, Pixel const *>;

public:

	//! Constructs an empty view.
	BitmapConstView() = default;

	//! Views an entire bitmap.
	//!
	//! @param  b   Bitmap to reference (must outlive the view)
	BitmapConstView(Bitmap<Pixel> const & b)
		: Base(b.data(), b.width(), b.height(), b.pitch())
	{
	}

	//! Views a region of a bitmap, clipped to the bitmap's bounds.
	//!
	//! @param  b   Bitmap to reference (must outlive the view)
	//! @param  x   Left of the region
	//! @param  y   Top of the region
	//! @param  w   Width of the region
	//! @param  h   Height of the region
	BitmapConstView(Bitmap<Pixel> const & b, int x, int y, int w, int h)
		: BitmapConstView(BitmapConstView(b).region(x, y, w, h))
	{
	}

	//! Converts a mutable view to a read-only view.
	BitmapConstView(BitmapView<Pixel> const & v);

	//! Returns a sub-view of this view, clipped to this view's bounds.
	//!
	//! @param  x   Left of the sub-region
	//! @param  y   Top of the sub-region
	//! @param  w   Width of the sub-region
	//! @param  h   Height of the sub-region
	//!
	//! @return     A view of the clipped sub-region (empty if it lies entirely outside this view)
	BitmapConstView region(int x, int y, int w, int h) const
	{
		Pixel const * p = nullptr;
		int           rw = 0;
		int           rh = 0;
		this->subRegion(x, y, w, h, p, rw, rh);
		return BitmapConstView(p, rw, rh, this->pitch());
	}

private:

	BitmapConstView(Pixel const * p, int w, int h, std::size_t pitch)
		: Base(p, w, h, pitch)
	{
	}
};

//! A read-write, non-owning reference to a region of a Bitmap.
//!
//! The referenced Bitmap must outlive the view. Writes through the view mutate the underlying bitmap's pixels in place. Implicitly
//! converts to BitmapConstView.
//!
//! @tparam Pixel   Pixel type
template <typename Pixel>
class BitmapView : public detail::BasicBitmapView<Pixel, Pixel *>
{
	using Base = detail::BasicBitmapView<Pixel, Pixel *>;

public:

	//! Constructs an empty view.
	BitmapView() = default;

	//! Views an entire bitmap.
	//!
	//! @param  b   Bitmap to reference (must outlive the view)
	BitmapView(Bitmap<Pixel> & b)
		: Base(b.data(), b.width(), b.height(), b.pitch())
	{
	}

	//! Views a region of a bitmap, clipped to the bitmap's bounds.
	//!
	//! @param  b   Bitmap to reference (must outlive the view)
	//! @param  x   Left of the region
	//! @param  y   Top of the region
	//! @param  w   Width of the region
	//! @param  h   Height of the region
	BitmapView(Bitmap<Pixel> & b, int x, int y, int w, int h)
		: BitmapView(BitmapView(b).region(x, y, w, h))
	{
	}

	//! Sets the pixel at (x, y). Does nothing if the coordinate lies outside the view.
	//!
	//! @param  x   Column (0-based), in [0, width())
	//! @param  y   Row (0-based), in [0, height())
	//! @param  p   Pixel value to store
	void setPixel(int x, int y, Pixel p)
	{
		Pixel * a = this->addressOf(x, y);
		if (a)
			*a = p;
	}

	//! Fills the whole view with a single pixel value. Does nothing if the view is empty.
	//!
	//! @param  p   Pixel value written to every pixel in the view
	void fill(Pixel p)
	{
		if (this->empty())
		{
			return;
		}
		for (int y = 0; y < this->height(); ++y)
		{
			Pixel * row = this->addressOf(0, y);
			for (int x = 0; x < this->width(); ++x)
				row[x] = p;
		}
	}

	//! Returns a sub-view of this view, clipped to this view's bounds.
	//!
	//! @param  x   Left of the sub-region
	//! @param  y   Top of the sub-region
	//! @param  w   Width of the sub-region
	//! @param  h   Height of the sub-region
	//!
	//! @return     A view of the clipped sub-region (empty if it lies entirely outside this view)
	BitmapView region(int x, int y, int w, int h) const
	{
		Pixel * p  = nullptr;
		int     rw = 0;
		int     rh = 0;
		this->subRegion(x, y, w, h, p, rw, rh);
		return BitmapView(p, rw, rh, this->pitch());
	}

	//! Copies a region of a source view into this view.
	//!
	//! The source rectangle and destination origin are clipped to both views' bounds; portions that would fall outside either view
	//! are skipped. Does nothing if the clipped region is empty.
	//!
	//! @param  src     Source view to read from
	//! @param  srcRect Region of the source view to copy
	//! @param  dstX    Destination column at which to place the copy
	//! @param  dstY    Destination row at which to place the copy
	//!
	//! @warning Source and destination must not overlap; overlapping copies are undefined.
	void copy(BitmapConstView<Pixel> src, Rect const & srcRect, int dstX, int dstY)
	{
		Rect clipped = srcRect;
		detail::clip(&clipped, src.width(), src.height(), &dstX, &dstY, this->width(), this->height());

		if (clipped.width <= 0 || clipped.height <= 0)
			return;

		int sy       = clipped.y;
		int sx       = clipped.x;
		int rowBytes = clipped.width * static_cast<int>(Base::PIXEL_SIZE);
		for (int i = 0; i < clipped.height; ++i)
		{
			std::byte *       d = reinterpret_cast<std::byte *>(this->data(dstX, dstY + i));
			std::byte const * s = reinterpret_cast<std::byte const *>(src.data(sx, sy + i));
			std::copy(s, s + rowBytes, d);
		}
	}

private:

	BitmapView(Pixel * p, int w, int h, std::size_t pitch)
		: Base(p, w, h, pitch)
	{
	}
};

//! Captures the mutable view's origin, dimensions, and pitch as a read-only view. No pixels are copied.
//!
//! @param  v   Mutable view to convert
template <typename Pixel>
BitmapConstView<Pixel>::BitmapConstView(BitmapView<Pixel> const & v)
	: Base(v.data(), v.width(), v.height(), v.pitch())
{
}

//! Allocates a buffer (pitch == @c width*PIXEL_SIZE) and copies the view's pixels into it row by row. Resets the bitmap to empty
//! if the view has zero width or height.
//!
//! @param  src     View whose pixels are copied into the new bitmap
template <typename Pixel>
Bitmap<Pixel>::Bitmap(BitmapConstView<Pixel> const & src)
	: width_(src.width())
	, height_(src.height())
	, pitch_(static_cast<std::size_t>(src.width()) * PIXEL_SIZE)
{
	if (width_ > 0 && height_ > 0)
	{
		data_.resize(pitch_ * static_cast<std::size_t>(height_));
		for (int y = 0; y < height_; ++y)
		{
			std::byte const * s = reinterpret_cast<std::byte const *>(src.data(0, y));
			std::byte *       d = reinterpret_cast<std::byte *>(data(0, y));
			std::copy(s, s + pitch_, d);
		}
	}
	else
	{
		width_  = 0;
		height_ = 0;
		pitch_  = 0;
	}
}

//! Replaces the contents with an owning copy of the view's pixels.
//!
//! @param  src     View whose pixels are copied into this bitmap
template <typename Pixel>
void Bitmap<Pixel>::load(BitmapConstView<Pixel> const & src)
{
	*this = Bitmap<Pixel>(src);
}

//! @return     A mutable view referencing every pixel of this bitmap
template <typename Pixel>
BitmapView<Pixel> Bitmap<Pixel>::view()
{
	return BitmapView<Pixel>(*this);
}

//! Returns a mutable view of a region of the bitmap, clipped to the bitmap's bounds.
//!
//! @param  x   Left of the region
//! @param  y   Top of the region
//! @param  w   Width of the region
//! @param  h   Height of the region
//!
//! @return     A mutable view of the clipped region
template <typename Pixel>
BitmapView<Pixel> Bitmap<Pixel>::view(int x, int y, int w, int h)
{
	return BitmapView<Pixel>(*this, x, y, w, h);
}

//! @return     A read-only view referencing every pixel of this bitmap
template <typename Pixel>
BitmapConstView<Pixel> Bitmap<Pixel>::view() const
{
	return BitmapConstView<Pixel>(*this);
}

//! Returns a read-only view of a region of the bitmap, clipped to the bitmap's bounds.
//!
//! @param  x   Left of the region
//! @param  y   Top of the region
//! @param  w   Width of the region
//! @param  h   Height of the region
//!
//! @return     A read-only view of the clipped region
template <typename Pixel>
BitmapConstView<Pixel> Bitmap<Pixel>::view(int x, int y, int w, int h) const
{
	return BitmapConstView<Pixel>(*this, x, y, w, h);
}

//! Equivalent to the const overload of view(), but callable on a non-const bitmap when a read-only view is wanted explicitly.
//!
//! @return     A read-only view referencing every pixel of this bitmap
template <typename Pixel>
BitmapConstView<Pixel> Bitmap<Pixel>::cview() const
{
	return BitmapConstView<Pixel>(*this);
}

#endif // !defined(BITMAP_BITMAPVIEW_H)
