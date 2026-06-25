#if !defined(BITMAP_PALETTIZEDBITMAP_H)
#define BITMAP_PALETTIZEDBITMAP_H

#pragma once

#include "Bitmap/Bitmap.h"
#include "Bitmap/Palette.h"
#include <algorithm>
#include <cstdint>

//! An index-based bitmap paired with a palette.
//!
//! Each pixel is an 8-bit index into the owned palette (the base @c Bitmap<uint8_t> stores the indices).
//!
//! @tparam Color   Color type of the palette entries
//!
//! @sa Palette, Bitmap
template <class Color>
class PalettizedBitmap : public Bitmap<uint8_t>
{
public:

    using ColorType = Color;    //!< Color template parameter type

    //! Constructs an empty palettized bitmap with a default palette.
    PalettizedBitmap() = default;

    //! Constructs a palettized bitmap of the given size with a default palette.
    PalettizedBitmap(int w, int h);

    //! Constructs a palettized bitmap of the given size with the given palette.
    PalettizedBitmap(int w, int h, Palette<Color> const & palette);

    //! Constructs a palettized bitmap from index data and a palette.
    PalettizedBitmap(int w, int h, uint8_t const * data, Palette<Color> const & palette);

    //! Replaces the contents with the given index data and palette.
    void load(int w, int h, uint8_t const * data, Palette<Color> const & palette);

    //! Returns a read-only reference to the palette.
    Palette<Color> const & palette() const { return palette_; }

    //! Returns a mutable reference to the palette.
    Palette<Color> & palette() { return palette_; }

    //! Replaces the palette.
    //!
    //! @param  palette     New palette
    void setPalette(Palette<Color> const & palette) { palette_ = palette; }

    //! Expands a region of this bitmap into a full-color bitmap.
    Bitmap<Color> decompressedRegion(int x, int y, int width, int height, int pitch = 0) const;

protected:
    using BaseClass = Bitmap<uint8_t>; //!< Base class

private:

    Palette<Color> palette_;        // The palette
};

//! The pixel indices are left default-initialized.
//!
//! @param  w   Width (in pixels)
//! @param  h   Height (in pixels)
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h)
    : BaseClass(w, h)
{
}

//! The pixel indices are left default-initialized.
//!
//! @param  w       Width (in pixels)
//! @param  h       Height (in pixels)
//! @param  palette Palette to copy
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, Palette<Color> const & palette)
    : BaseClass(w, h)
    , palette_(palette)
{
}

//! Copies @c w*h index bytes from @p data and stores the given palette.
//!
//! @param  w       Width (in pixels)
//! @param  h       Height (in pixels)
//! @param  data    Source index data (@c w*h bytes, tightly packed)
//! @param  palette Palette to copy
template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, uint8_t const * data, Palette<Color> const & palette)
    : BaseClass(w, h, 0, data)
    , palette_(palette)
{
}

//! Replaces both the index data and the palette, discarding any previous contents.
//!
//! @param  w       Width (in pixels)
//! @param  h       Height (in pixels)
//! @param  data    Source index data (@c w*h bytes, tightly packed)
//! @param  palette Palette to copy
template <class Color>
void PalettizedBitmap<Color>::load(int w, int h, uint8_t const * data, Palette<Color> const & palette)
{
    BaseClass::load(w, h, 0, data);
    palette_ = palette;
}

//! Each index in the clipped region is looked up in the palette to produce a full-color pixel. The requested rectangle is clipped
//! to this bitmap's bounds, so the result may be smaller than requested (and empty if it lies entirely outside).
//!
//! @param  x       Left of the region
//! @param  y       Top of the region
//! @param  width   Width of the region
//! @param  height  Height of the region
//! @param  pitch   Pitch of the resulting bitmap (in bytes), or 0 to derive it from the width
//!
//! @return     A new owning, full-color bitmap holding the decompressed region
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
