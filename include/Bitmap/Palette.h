#if !defined(PALETTE_H_INCLUDED)
#define PALETTE_H_INCLUDED

#pragma once

/*****************************************************************************

                                  Palette.h

                        Copyright 2001, John J. Bolton
    ----------------------------------------------------------------------

    $Header: //depot/Libraries/Bitmap/Palette.h#2 $

    $NoKeywords: $

*****************************************************************************/

#include "Pixel.h"

#include "..\Misc\Etc.h"
#include <Cstring>

template <class _Entry>
class Palette
{
public:

    typedef _Entry Entry;

    enum
    {
        NUM_PALETTE_ENTRIES = 256,                  // Number of entries in a palette
        ENTRY_SIZE          = sizeof(Entry)         // Number of bytes in each palette entry
    };

    // Constructors and destructors

    Palette() {}
    Palette(Entry const * pData)
    {
        memcpy(m_Data, pData, sizeof m_Data);
    }

    // Returns a pointer to the palette data
    Entry const * GetData() const { return m_Data; }
    Entry       * GetData()       { return m_Data; }

    // Overrides [] as a convenience so that the class can be accessed
    // as an array instead of referencing 'm_Data'.
    Entry & operator [](int entry)
    {
        assert_limits(0, entry, NUM_PALETTE_ENTRIES - 1);
        return m_Data[entry];
    }

    Entry const & operator [](int entry) const
    {
        assert_limits(0, entry, NUM_PALETTE_ENTRIES - 1);
        return m_Data[entry];
    }

private:

    Entry m_Data[NUM_PALETTE_ENTRIES];      // Palette data
};

// Types defined for convenience

typedef Palette<Pixel1555>    Palette1555;
typedef Palette<Pixel1555>    Palette565;
typedef Palette<Pixel24>      Palette24;
typedef Palette<Pixel32>      Palette32;

#endif // !defined( PALETTE_H_INCLUDED )
