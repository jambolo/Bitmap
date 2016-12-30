#if !defined(PIXEL_H_INCLUDED)
#define PIXEL_H_INCLUDED

#pragma once

/*****************************************************************************

                                   Pixel.h

                        Copyright 2001, John J. Bolton
    ----------------------------------------------------------------------

    $Header: //depot/Libraries/Bitmap/Pixel.h#2 $

    $NoKeywords: $

*****************************************************************************/

struct ARGB
{
    unsigned __int8 m_Alpha;
    unsigned __int8 m_Red;
    unsigned __int8 m_Green;
    unsigned __int8 m_Blue;
};

struct RGB
{
    unsigned __int8 m_Red;
    unsigned __int8 m_Green;
    unsigned __int8 m_Blue;
};

struct BGRA
{
    unsigned __int8 m_Blue;
    unsigned __int8 m_Green;
    unsigned __int8 m_Red;
    unsigned __int8 m_Alpha;
};

struct BGR
{
    unsigned __int8 m_Blue;
    unsigned __int8 m_Green;
    unsigned __int8 m_Red;
};

typedef unsigned __int8 Pixel8;

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
class Pixel16
{
public:

    Pixel16(float a, float r, float g, float b);
    Pixel16(unsigned int c);

    unsigned int    GetRawColor()                   const { return m_Value;               }
    void            SetRawColor(unsigned int c)           { m_Value = unsigned int(c);  }

    float   GetAlpha() const;
    void    SetAlpha(float a);

    float   GetRed() const;
    void    SetRed(float r);

    float   GetGreen() const;
    void    SetGreen(float g);

    float   GetBlue() const;
    void    SetBlue(float b);

private:

    unsigned __int16 m_Value;   // The raw value of the pixel
};

//
// The following two template classes are instantiated in Pixel.cpp. Use the
// following lines to prevent automatic instantiation in you cpp file.
//
//
//#pragma warning( disable: 4231 )
//
//extern template class Pixel16< 0x01, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0 >;
//extern template class Pixel16< 0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0 >;
//

typedef Pixel16<0x01, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0> Pixel565;
typedef Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0> Pixel1555;

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
inline Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::Pixel16(unsigned int c)
    : m_Value(c)
{
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::Pixel16(float a, float r, float g, float b)
    : m_Value((int((a * _am + .5f)) << _as) |
              (int((r * _rm + .5f)) << _rs) |
              (int((g * _gm + .5f)) << _gs) |
              (int((b * _bm + .5f)) << _bs))
{
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
float Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::GetAlpha() const
{
    return float((m_Value >> _as) & _am) / float(_am);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
void Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::SetAlpha(float a)
{
    m_Value = (m_Value & ~_am) | (int((a * _am + .5f)) << _as);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
float Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::GetRed() const
{
    return float((m_Value >> _rs) & _rm) / float(_rm);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
void Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::SetRed(float r)
{
    m_Value = (m_Value & ~_rm) | (int((r * _rm + .5f)) << _rs);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
float Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::GetGreen() const
{
    return float((m_Value >> _gs) & _gm) / float(_gm);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
void Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::SetGreen(float g)
{
    m_Value = (m_Value & ~_gm) | (int((g * _gm + .5f)) << _gs);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
float Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::GetBlue() const
{
    return float((m_Value >> _bs) & _bm) / float(_bm);
}

template <int _am, int _rm, int _gm, int _bm, int _as, int _rs, int _gs, int _bs>
void Pixel16<_am, _rm, _gm, _bm, _as, _rs, _gs, _bs>::SetBlue(float b)
{
    m_Value = (m_Value & ~_bm) | (int((b * _bm + .5f)) << _bs);
}

class Pixel24
{
    // Byte ordering (in memory, BGR)
    enum
    {
        _bs = 0,
        _gs = 1,
        _rs = 2
    };
public:

    Pixel24(float r, float g, float b);
    Pixel24(unsigned int c);

    // Set the color of the pixel
    void SetColor(float r, float g, float b);

    // Return the raw value of the pixel
    unsigned __int32 GetRawColor() const;

    void SetRawColor(unsigned __int32 c);

    float GetRed() const;
    void SetRed(float r);

    float GetGreen() const;
    void SetGreen(float g);

    float GetBlue() const;
    void SetBlue(float b);

private:
    unsigned __int8 m_Value[3];         // Component colors
};

inline Pixel24::Pixel24(float r, float g, float b)
{
    SetColor(r, g, b);
}

inline Pixel24::Pixel24(unsigned int c)
{
    SetRawColor(c);
}

// Return the raw value of the pixel
inline unsigned __int32 Pixel24::GetRawColor() const
{
    return (m_Value[0]) |
           (m_Value[1] <<  8) |
           (m_Value[2] << 16);
}

inline void Pixel24::SetRawColor(unsigned __int32 c)
{
    m_Value[0] = unsigned __int8(c);
    m_Value[1] = unsigned __int8(c >>  8);
    m_Value[2] = unsigned __int8(c >> 16);
}

inline float Pixel24::GetRed() const
{
    return float(m_Value[_rs]) / float(0xff);
}

inline void Pixel24::SetRed(float r)
{
    m_Value[_rs] = unsigned __int8(r * 0xff + .5f);
}

inline float Pixel24::GetGreen() const
{
    return float(m_Value[_gs]) / float(0xff);
}

inline void Pixel24::SetGreen(float g)
{
    m_Value[_gs] = unsigned __int8(g * 0xff + .5f);
}

inline float Pixel24::GetBlue() const
{
    return float(m_Value[_bs]) / float(0xff);
}

inline void Pixel24::SetBlue(float b)
{
    m_Value[_bs] = unsigned __int8(b * 0xff + .5f);
}

class Pixel32
{
    // Byte ordering (in memory, BGRA)
    enum
    {
        _bs = 0,
        _gs = 1,
        _rs = 2,
        _as = 3
    };

public:

    Pixel32(float a, float r, float g, float b);
    Pixel32(unsigned __int32 c);

    // Set the color of the pixel
    void SetColor(float a, float r, float g, float b);

    // Return the raw value of the pixel
    unsigned __int32 GetRawColor()          const { return *reinterpret_cast<unsigned __int32 const *>(m_Value);  }
    void SetRawColor(unsigned __int32 c)          { *reinterpret_cast<unsigned __int32 *>(m_Value) = c;       }

    float GetAlpha() const;
    void SetAlpha(float a);

    float GetRed() const;
    void SetRed(float r);

    float GetGreen() const;
    void SetGreen(float g);

    float GetBlue() const;
    void SetBlue(float b);

private:
    unsigned __int8 m_Value[4];
};

inline Pixel32::Pixel32(float a, float r, float g, float b)
{
    SetColor(a, r, g, b);
}

inline Pixel32::Pixel32(unsigned __int32 c)
{
    SetRawColor(c);
}

inline float Pixel32::GetAlpha() const
{
    return float(m_Value[_as]) / float(0xff);
}

inline void Pixel32::SetAlpha(float r)
{
    m_Value[_as] = unsigned __int8(r * 0xff + .5f);
}

inline float Pixel32::GetRed() const
{
    return float(m_Value[_rs]) / float(0xff);
}

inline void Pixel32::SetRed(float r)
{
    m_Value[_rs] = unsigned __int8(r * 0xff + .5f);
}

inline float Pixel32::GetGreen() const
{
    return float(m_Value[_gs]) / float(0xff);
}

inline void Pixel32::SetGreen(float g)
{
    m_Value[_gs] = unsigned __int8(g * 0xff + .5f);
}

inline float Pixel32::GetBlue() const
{
    return float(m_Value[_bs]) / float(0xff);
}

inline void Pixel32::SetBlue(float b)
{
    m_Value[_bs] = unsigned __int8(b * 0xff + .5f);
}

#endif // !defined( PIXEL_H_INCLUDED )
