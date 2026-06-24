#if !defined(BITMAP_PALETTE_H)
#define BITMAP_PALETTE_H

#pragma once

#include "Pixel.h"
#include <algorithm>
#include <cassert>
#include <cstddef>

//! A palette type.
template <class Entry>
class Palette
{
public:

    using EntryType = Entry;                                //!< Entry template parameter type

    static size_t constexpr PALETTE_SIZE = 256;             //!< Number of entries in a palette
    static size_t constexpr ENTRY_SIZE   = sizeof(Entry);   //!< Size of a palette entry

    //! Constructor.
    Palette() = default;

    //! Constructor.
    //!
    //! @param  pData   Palette data
    Palette(Entry const * pData)
    {
        std::copy(pData, pData + PALETTE_SIZE, entries_.begin());
    }

    //! Returns a pointer to the palette data
    Entry const * entries() const { return entries_.data(); }

    //! Returns a pointer to the palette data
    Entry * entries() { return entries_.data(); }

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

    std::array<Entry, PALETTE_SIZE> entries_;      // Palette data
};

// Types defined for convenience

using Palette1555 = Palette<Pixel1555>;
using Palette565  = Palette<Pixel565>;
using Palette4444 = Palette<Pixel4444>;
using PaletteRGB  = Palette<PixelRGB>;
using PaletteBGR  = Palette<PixelBGR>;
using PaletteARGB = Palette<PixelARGB>;
using PaletteRGBA = Palette<PixelRGBA>;
using PaletteBGRA = Palette<PixelBGRA>;
using PaletteABGR = Palette<PixelABGR>;

#endif // !defined(BITMAP_PALETTE_H)
