#if !defined(BITMAP_PALETTE_H)
#define BITMAP_PALETTE_H

#pragma once

#include "Pixel.h"
#include <cassert>
#include <cstddef>

template <class Entry>
class Palette
{
public:

    using EntryType = Entry;    //!< Entry template parameter type

    static size_t constexpr PALETTE_SIZE = 256;     //!< Number of entries in a palette
    static size_t constexpr ENTRY_SIZE   = sizeof(Entry); //!< Size of a palette entry

    //! Constructor.
    Palette() = default;

    //! Constructor.
    //!
    //! @param  pData   Palette data
    Palette(Entry const * pData)
    {
        memcpy(entries_, pData, sizeof entries_);
    }

    //! Returns a pointer to the palette data
    Entry const * entries() const { return entries_; }

    //! Returns a pointer to the palette data
    Entry * entries() { return entries_; }

    //! Operator []
    Entry & operator [](int entry)
    {
        assert(entry >= 0 && entry < PALETTE_SIZE);
        return entries_[entry];
    }

    //! Operator []
    Entry operator [](int entry) const
    {
        assert(entry >= 0 && entry < PALETTE_SIZE);
        return entries_[entry];
    }

private:

    Entry entries_[PALETTE_SIZE];      // Palette data
};

// Types defined for convenience

using Palette1555 = Palette<Pixel1555>;
using Palette565  = Palette<Pixel1555>;
using PaletteRGB  = Palette<PixelRGB>;
using PaletteBGR  = Palette<PixelBGR>;
using PaletteARGB = Palette<PixelARGB>;
using PaletteRGBA = Palette<PixelRGBA>;
using PaletteBGRA = Palette<PixelBGRA>;
using PaletteABGR = Palette<PixelABGR>;

#endif // !defined(BITMAP_PALETTE_H)
