#if !defined(BITMAP_PALETTIZEDBITMAP_H)
#define BITMAP_PALETTIZEDBITMAP_H

#pragma once

#include "Bitmap/Bitmap.h"
#include "Bitmap/Palette.h"
#include <algorithm>
#include <cstdint>

//! A bitmap with a palette.
template <class Color>
class PalettizedBitmap : public Bitmap<uint8_t>
{
public:

    using ColorType = Color;    //!< Color template parameter type

    //! Constructor.
    PalettizedBitmap() = default;

    //! Constructor.
    PalettizedBitmap(int w, int h);

    //! Constructor.
    PalettizedBitmap(int w, int h, Palette<Color> const & palette);

    //! Constructor.
    PalettizedBitmap(int w, int h, uint8_t const * data, Palette<Color> const & palette);

    //! Loads a bitmap
    void load(int w, int h, uint8_t const * data, Palette<Color> const & palette);

    //! Returns the palette
    Palette<Color> const & palette() const { return palette_; }

    //! Returns the palette
    Palette<Color> & palette() { return palette_; }

    //! Sets the palette
    void setPalette(Palette<Color> const & palette) { palette_ = palette; }

    //! Creates a non-palettized bitmap from a region of this bitmap.
    Bitmap<Color> decompressedRegion(int x, int y, int width, int height, int pitch = 0) const;

protected:
    using BaseClass = Bitmap<uint8_t>; //!< Base class

private:

    Palette<Color> palette_;        // The palette
};

//! @param 	w
//! @param 	h
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h)
    : BaseClass(w, h)
{
}

//! @param 	w
//! @param 	h
//! @param 	palette
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, Palette<Color> const & palette)
    : BaseClass(w, h)
    , palette_(palette)
{
}

//! @param 	w
//! @param 	h
//! @param 	data
//! @param 	palette
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, uint8_t const * data, Palette<Color> const & palette)
    : BaseClass(w, h, 0, data)
    , palette_(palette)
{
}

//! @param 	w
//! @param 	h
//! @param 	data
//! @param 	palette
template <class Color>
void PalettizedBitmap<Color>::load(int w, int h, uint8_t const * data, Palette<Color> const & palette)
{
    BaseClass::load(w, h, 0, data);
    palette_ = palette;
}

//! @param 	x       Location of the region
//! @param 	y       Location of the region
//! @param 	width   Width of the region
//! @param 	height  Height of the region
//! @param 	pitch   Pitch of the resulting bitmap
//!
//! @return un-palettized bitmap
template <class Color>
Bitmap<Color> PalettizedBitmap<Color>::decompressedRegion(int x, int y, int width, int height, int pitch /*= 0*/) const
{
    Rect clipped{0, 0, width_, height_};
    clipped.clip(Rect{x, y, width, height});
    Bitmap<Color>   region(clipped.width, clipped.height, pitch);
    uint8_t const * src = data(clipped.x, clipped.y);
    Color *         dst = region.data();
    for (int i = 0; i < clipped.height; ++i)
    {
        std::transform(src, src + clipped.width, dst, [this](uint8_t index) { return palette_[index]; });
        dst  = reinterpret_cast<Color *>(reinterpret_cast<std::byte *>(dst) + region.pitch());
        src += pitch_;
    }
    return region;
}

#endif // !defined(BITMAP_PALETTIZEDBITMAP_H)
