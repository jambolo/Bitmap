#if !defined(PIXEL8_H_INCLUDED)
#define PIXEL8_H_INCLUDED

#pragma once

/*****************************************************************************

                                   Pixel8.h

                        Copyright 2001, John J. Bolton
    ----------------------------------------------------------------------

    $Header: //depot/Libraries/Bitmap/Pixel8.h#2 $

    $NoKeywords: $

*****************************************************************************/

#include "Palette.h"

template <class _Palette>
class Pixel8 : public Pixel
{
public:

    typedef _Palette PaletteType;

    Pixel8(int i);

    // *** Pixel overrides

    // Return the color of the pixel in RGBA format
    RGBA                GetColor() const;

    // Return the raw color of the pixel
    unsigned __int32    GetRawColor() const = 0;

    // Return the individual component colors
    int                 GetRed() const;
    int                 GetGreen() const;
    int                 GetBlue() const;

    // *** End Pixel overrides

    // Pixel8 is an 8-bit index into a 256-color palette
    unsigned __int8 m_Value;

private:

    static PaletteType * m_Palette;
};

#endif // !defined( PIXEL8_H_INCLUDED )
