/*****************************************************************************

                                  Pixel.cpp

                        Copyright 2001, John J. Bolton
    ----------------------------------------------------------------------

    $Header: //depot/Libraries/Bitmap/Pixel.cpp#2 $

    $NoKeywords: $

*****************************************************************************/

#include "Bitmap/Pixel.h"

// Instantiate two common 16-bit pixel formats here

template class Pixel16<0x01, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;
template class Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;

void Pixel24::SetColor(float r, float g, float b)
{
    SetRed(r);
    SetGreen(g);
    SetBlue(b);
}

void Pixel32::SetColor(float a, float r, float g, float b)
{
    SetAlpha(a);
    SetRed(r);
    SetGreen(g);
    SetBlue(b);
}
