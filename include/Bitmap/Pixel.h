#if !defined(BITMAP_PIXEL_H)
#define BITMAP_PIXEL_H

#pragma once

#include <cstdint>

struct ARGB
{
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct RGB
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

struct BGRA
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
};

struct BGR
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

//! 8-bit pixel.
using Pixel8 = uint8_t;

//! 16-bit pixel.
template <int ALPHA_MASK, int RED_MASK, int GREEN_MASK, int BLUE_MASK,
          int ALPHA_OFF, int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel16
{
public:

    //! Constructor.
    Pixel16() = default;

    //! Constructor.
    Pixel16(float r, float g, float b, float a = 1.0f)
    {
        set(r, g, b, a);
    }

    //! Constructor.
    Pixel16(unsigned c)
        : value_(uint16_t(c))
    {
    }

    //! Returns the raw pixel value.
    unsigned raw() const { return value_; }

    //! Sets the pixel value.
    void set(unsigned c) { value_ = uint16_t(c);  }

    //! Sets the pixel value.
    void set(float r, float g, float b, float a = 1.0f)
    {
        value_ =   (uint16_t(r * RED_MASK + .5f) << RED_OFF)
                 | (uint16_t(r * GREEN_MASK + .5f) << GREEN_OFF)
                 | (uint16_t(r * BLUE_MASK + .5f) << BLUE_OFF)
                 | (uint16_t(r * ALPHA_MASK + .5f) << ALPHA_OFF);
    }

    //! Returns the red value.
    float red() const { return float((value_ >> RED_OFF) & RED_MASK) / float(RED_MASK); }

    //! Sets the red value.
    void setRed(float r) { value_ = (value_ & ~RED_MASK) | (uint16_t(r * RED_MASK + .5f) << RED_OFF); }

    //! Returns the green value.
    float green() const { return float((value_ >> GREEN_OFF) & GREEN_MASK) / float(GREEN_MASK); }

    //! Sets the green value.
    void setGreen(float g) { value_ = (value_ & ~GREEN_MASK) | (uint16_t(g * GREEN_MASK + .5f) << GREEN_OFF); }

    //! Returns the blue value.
    float blue() const { return float((value_ >> BLUE_OFF) & BLUE_MASK) / float(BLUE_MASK); }

    //! Sets the blue value.
    void setBlue(float b) { value_ = (value_ & ~BLUE_MASK) | (uint16_t(b * BLUE_MASK + .5f) << BLUE_OFF); }

    //! Returns the alpha value, or 0 if not supported.
    float alpha() const { return float((value_ >> ALPHA_OFF) & ALPHA_MASK) / float(ALPHA_MASK); }

    //! Sets the alpha value if it is supported.
    void setAlpha(float a) { value_ = (value_ & ~ALPHA_MASK) | (uint16_t(a * ALPHA_MASK + .5f) << ALPHA_OFF); }

private:

    uint16_t value_;   // The raw value of the pixel
};

extern template class Pixel16<0, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;
extern template class Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;

//! 16-bit R5G6B5 pixel format.
using Pixel565 = Pixel16<0, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;

//! 16-bit A1R5G5B5 pixel format.
using Pixel1555 = Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;

//! 24-bit pixel.
template <int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel24
{
public:

    //! Constructor.
    Pixel24() = default;

    //! Constructor.
    Pixel24(float r, float g, float b)
    {
        set(r, g, b);
    }

    //! Constructor.
    Pixel24(unsigned c)
    {
        set(c);
    }

    //! Returns the raw value.
    unsigned raw() const
    {
        return (value_[0] << 16) | (value_[1] <<  8) | (value_[2]);
    }

    //! Sets the value.
    void set(float r, float g, float b)
    {
        setRed(r);
        setGreen(g);
        setBlue(b);
    }

    //! Sets the value.
    void set(unsigned c)
    {
        value_[0] = uint8_t((c >> 16) & 0xff);
        value_[1] = uint8_t((c >>  8) & 0xff);
        value_[2] = uint8_t(c & 0xff);
    }

    //! Returns the red value.
    float red() const
    {
        return float(value_[RED_OFF]) / float(0xff);
    }

    //! Sets the red value.
    void setRed(float r)
    {
        value_[RED_OFF] = uint8_t(r * 0xff + .5f);
    }

    //! Returns the green value.
    float green() const
    {
        return float(value_[GREEN_OFF]) / float(0xff);
    }

    //! Sets the green value.
    void setGreen(float g)
    {
        value_[GREEN_OFF] = uint8_t(g * 0xff + .5f);
    }

    //! Returns the blue value.
    float blue() const
    {
        return float(value_[BLUE_OFF]) / float(0xff);
    }

    //! Sets the blue value.
    void setBlue(float b)
    {
        value_[BLUE_OFF] = uint8_t(b * 0xff + .5f);
    }

private:
    uint8_t value_[3]; // Component colors
};

// Pre-instantiated common 32-bit formats.

//! 32-bit R8G8B8 pixel format.
using PixelRGB = Pixel24<0, 1, 2>;
extern template class Pixel24<0, 1, 2>;

//! 32-bit B8G8R8 pixel format.
using PixelBGR = Pixel24<2, 1, 0>;
extern template class Pixel24<2, 1, 0>;

//! 32-bit pixel.
template <int ALPHA_OFF, int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel32
{
public:

    Pixel32(float r, float g, float b, float a = 1.0f)
    {
        set(r, g, b, a);
    }

    Pixel32(uint32_t c)
    {
        set(c);
    }

    //! Returns the value in raw form.
    uint32_t raw() const
    {
        return (value_[0] << 24) | (value_[1] << 16) | (value_[2] << 8) | value_[3];
    }
    //! Sets the value.
    void set(float r, float g, float b, float a = 1.0f)
    {
        setRed(r);
        setGreen(g);
        setBlue(b);
        setAlpha(a);
    }

    //! Sets the value.
    void set(uint32_t c)
    {
        value_[0] = uint8_t((c >> 24) & 0xff);
        value_[1] = uint8_t((c >> 16) & 0xff);
        value_[2] = uint8_t((c >>  8) & 0xff);
        value_[3] = uint8_t(c & 0xff);
    }

    //! Returns the alpha value.
    float alpha() const
    {
        return float(value_[ALPHA_OFF]) / float(0xff);
    }

    //! Sets the alpha value.
    void setAlpha(float a)
    {
        value_[ALPHA_OFF] = uint8_t(a * 0xff + .5f);
    }

    //! Returns the red value.
    float red() const
    {
        return float(value_[RED_OFF]) / float(0xff);
    }

    //! Sets the red value.
    void setRed(float r)
    {
        value_[RED_OFF] = uint8_t(r * 0xff + .5f);
    }

    //! Returns the green value.
    float green() const
    {
        return float(value_[GREEN_OFF]) / float(0xff);
    }

    //! Sets the green value.
    void setGreen(float g)
    {
        value_[GREEN_OFF] = uint8_t(g * 0xff + .5f);
    }

    //! Returns the blue value.
    float blue() const
    {
        return float(value_[BLUE_OFF]) / float(0xff);
    }

    //! Sets the blue value.
    void setBlue(float b)
    {
        value_[BLUE_OFF] = uint8_t(b * 0xff + .5f);
    }

private:
    uint8_t value_[4]; // Component colors
};

// Pre-instantiated common 32-bit formats.

//! 32-bit A8R8G8B8 pixel format.
using PixelARGB = Pixel32<0, 1, 2, 3>;
extern template class Pixel32<0, 1, 2, 3>;

//! 32-bit R8G8B8A8 pixel format.
using PixelRGBA = Pixel32<3, 0, 1, 2>;
extern template class Pixel32<3, 0, 1, 2>;

//! 32-bit B8G8R8A8 pixel format.
using PixelBGRA = Pixel32<3, 2, 1, 0>;
extern template class Pixel32<3, 2, 1, 0>;

//! 32-bit A8B8G8R8 pixel format.
using PixelABGR = Pixel32<0, 3, 2, 1>;
extern template class Pixel32<0, 3, 2, 1>;

#endif // !defined(BITMAP_PIXEL_H)
