#if !defined(BITMAP_PALETTE_H)
#define BITMAP_PALETTE_H

#pragma once

#include "Pixel.h"
#include <algorithm>
#include <cassert>
#include <cstddef>

//! A fixed-size color lookup table mapping 8-bit indices to colors.
//!
//! Holds exactly @c PALETTE_SIZE (256) entries, indexable by an 8-bit value. Used to expand an
//! index-based (palettized) bitmap into full colors.
//!
//! @tparam Entry   Color type stored in each entry
//!
//! @sa PalettizedBitmap
template <class Entry>
class Palette
{
public:

    using EntryType = Entry;                                //!< Entry template parameter type

    static size_t constexpr PALETTE_SIZE = 256;             //!< Number of entries in a palette
    static size_t constexpr ENTRY_SIZE   = sizeof(Entry);   //!< Size of a palette entry in bytes

    //! Constructs a palette with default-constructed entries.
    Palette() = default;

    //! Constructs a palette by copying @c PALETTE_SIZE entries from the given array.
    //!
    //! @param  pData   Pointer to @c PALETTE_SIZE contiguous entries to copy
    Palette(Entry const * pData)
    {
        std::copy(pData, pData + PALETTE_SIZE, entries_.begin());
    }

    //! Returns a read-only pointer to the @c PALETTE_SIZE contiguous entries.
    Entry const * entries() const { return entries_.data(); }

    //! Returns a mutable pointer to the @c PALETTE_SIZE contiguous entries.
    Entry * entries() { return entries_.data(); }

    //! Returns a mutable reference to the entry at the given index.
    //!
    //! @param  entry   Index, in [0, PALETTE_SIZE)
    Entry & operator [](int entry)
    {
        assert(entry >= 0 && entry < PALETTE_SIZE);
        return entries_[entry];
    }

    //! Returns a copy of the entry at the given index.
    //!
    //! @param  entry   Index, in [0, PALETTE_SIZE)
    Entry operator [](int entry) const
    {
        assert(entry >= 0 && entry < PALETTE_SIZE);
        return entries_[entry];
    }

private:

    std::array<Entry, PALETTE_SIZE> entries_;      // Palette data
};

// Types defined for convenience

using Palette1555 = Palette<Pixel1555>;     //!< Palette of 16-bit A1R5G5B5 entries
using Palette565  = Palette<Pixel565>;      //!< Palette of 16-bit R5G6B5 entries
using Palette4444 = Palette<Pixel4444>;     //!< Palette of 16-bit A4R4G4B4 entries
using PaletteRGB  = Palette<PixelRGB>;      //!< Palette of 24-bit R8G8B8 entries
using PaletteBGR  = Palette<PixelBGR>;      //!< Palette of 24-bit B8G8R8 entries
using PaletteARGB = Palette<PixelARGB>;     //!< Palette of 32-bit A8R8G8B8 entries
using PaletteRGBA = Palette<PixelRGBA>;     //!< Palette of 32-bit R8G8B8A8 entries
using PaletteBGRA = Palette<PixelBGRA>;     //!< Palette of 32-bit B8G8R8A8 entries
using PaletteABGR = Palette<PixelABGR>;     //!< Palette of 32-bit A8B8G8R8 entries

#endif // !defined(BITMAP_PALETTE_H)
