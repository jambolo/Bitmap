#if !defined(BITMAP_PALETTE_H)
#define BITMAP_PALETTE_H

#pragma once

#include "Pixel.h"

template <class Entry>
class Palette
{
public:

    //! Type of Entry template parameter
    using EntryType = Entry;

    static size_t constexpr PALETTE_SIZE = 256;     //!< Number of entries in a palette
    static size_t constexpr ENTRY_SIZE = sizeof(Entry); //!< Size of a palette entry

    //! Constructor.
    Palette() = default;

    //! Constructor.
    Palette(Entry const * pData)
    {
        memcpy(entries_, pData, sizeof entries_);
    }

    //! Returns a pointer to the palette data
    Entry const * entries() const { return entries_; }

    //! Returns a pointer to the palette data
    Entry       * entries()       { return entries_; }

    // Operator []
    Entry & operator [](int entry)
    {
        assert_limits(0, entry, PALETTE_SIZE - 1);
        return entries_[entry];
    }

    Entry operator [](int entry) const
    {
        assert_limits(0, entry, PALETTE_SIZE - 1);
        return entries_[entry];
    }

private:

    Entry entries_[PALETTE_SIZE];      // Palette data
};

// Types defined for convenience

using Palette1555 = Palette<Pixel1555>;
using Palette565 = Palette<Pixel1555>;
using PaletteRGB = Palette<PixelRGB>;
using PaletteBGR = Palette<PixelBGR>;
using PaletteARGB = Palette<PixelARGB>;
using PaletteRGBA = Palette<PixelRGBA>;
using PaletteBGRA = Palette<PixelBGRA>;
using PaletteABGR = Palette<PixelABGR>;

#endif // !defined(BITMAP_PALETTE_H)
