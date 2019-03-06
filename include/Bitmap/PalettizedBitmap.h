#if !defined(BITMAP_PALETTIZEDBITMAP_H)
#define BITMAP_PALETTIZEDBITMAP_H

#pragma once

#include "Bitmap.h"
#include "Palette.h"

template <class Color>
class PalettizedBitmap : public Bitmap<Pixel8>
{
public:

    using ColorType = Color;

    //! Constructor.
    PalettizedBitmap() = default;

    //! Constructor.
    PalettizedBitmap(int w, int h);

    //! Constructor.
    PalettizedBitmap(int w, int h, Palette<Color> const & palette);

    //! Constructor.
    PalettizedBitmap(int w, int h, Pixel * data, Palette<Color> const & palette);

    //! Loads a bitmap
    void load(int w, int h, Pixel const * data, Palette<Color> const & palette);

    //! References a memory buffer
    void reference(int w, int h, Pixel * data, Palette<Color> const & palette);

    //! Returns the palette
    Palette<Color> palette() const { return palette_; }

    //! Returns the palette
    Palette<Color> & palette() { return palette_; }

    //! Sets the palette
    void setPalette(Palette<Color> const & palette) { palette_ = palette; }

    //! Creates a non-palettized bitmap from a region of this bitmap.
    Bitmap<Color> decompressedRegion(int row, int column, int width, int height, int pitch = 0) const;

private:

    Palette<Color> palette_;        // The palette
};

template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h)
    : BaseClass(w, h)
{
}

template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, Palette<Color> const & palette)
    : BaseClass(w, h)
    , palette_(palette)
{
}

template <class Color>
PalettizedBitmap<Color>::PalettizedBitmap(int w, int h, Pixel * data, Palette<Color> const & palette)
    : BaseClass(w, h, 0, data)
    , palette_(palette)
{
}

template <class Color>
void PalettizedBitmap<Color>::load(int w, int h, Pixel const * data, Palette<Color> const & palette)
{
    BaseClass::load(w, h, 0, data);
    palette_ = palette;
}

template <class Color>
void PalettizedBitmap<Color>::reference(int w, int h, Pixel * data, Palette<Color> const & palette)
{
    BaseClass::reference(w, h, 0, data);
    palette_ = palette;
}

template <class Color>
Bitmap<Color> PalettizedBitmap<Color>::region(int row, int column, int width, int height, int pitch) const
{
    Bitmap<Color>  region(width, height, pitch);
    Pixel8 const * src = data(row, column);
    Color *        dst = region.data();
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            dst[j] = palette_[src[j]];
        }
        dst  = reinterpret_cast<Color *>(reinterpret_cast<char *>(dst) + region.pitch());
        src += width_;
    }
}

#endif // !defined(BITMAP_PALETTIZEDBITMAP_H)
