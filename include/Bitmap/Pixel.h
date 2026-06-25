#if !defined(BITMAP_PIXEL_H)
#define BITMAP_PIXEL_H

#pragma once

#include <array>
#include <cmath>
#include <cstdint>

//! 8-bit (single-byte) pixel, typically a palette index or grayscale intensity.
using Pixel8 = uint8_t;

//! A packed 16-bit color pixel.
//!
//! Components are stored as bit fields within a single @c uint16_t. Each template parameter pair gives a component's bit mask (its
//! maximum integer value, e.g. @c 0x1f for 5 bits) and its bit offset (its shift within the word). A zero alpha mask denotes a
//! format with no alpha channel.
//!
//! @tparam ALPHA_MASK  Maximum integer value of the alpha component (0 if unsupported)
//! @tparam RED_MASK    Maximum integer value of the red component
//! @tparam GREEN_MASK  Maximum integer value of the green component
//! @tparam BLUE_MASK   Maximum integer value of the blue component
//! @tparam ALPHA_OFF   Bit offset of the alpha component
//! @tparam RED_OFF     Bit offset of the red component
//! @tparam GREEN_OFF   Bit offset of the green component
//! @tparam BLUE_OFF    Bit offset of the blue component
template <int ALPHA_MASK, int RED_MASK, int GREEN_MASK, int BLUE_MASK,
          int ALPHA_OFF, int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel16
{
public:

    //! Constructs a pixel with all components zero.
    Pixel16() : value_(0) {}

    //! Constructs a pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    //! @param  a   Alpha, in [0, 1] (ignored if the format has no alpha channel)
    Pixel16(float r, float g, float b, float a = 1.0f)
    {
        set(r, g, b, a);
    }

    //! Constructs a pixel from a raw packed value.
    //!
    //! @param  c   Packed 16-bit value
    explicit Pixel16(unsigned c)
        : value_(uint16_t(c))
    {
    }

    //! Returns the raw packed pixel value.
    unsigned raw() const { return value_; }

    //! Sets the pixel from a raw packed value.
    //!
    //! @param  c   Packed 16-bit value
    void set(unsigned c) { value_ = uint16_t(c);  }

    //! Sets the pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    //! @param  a   Alpha, in [0, 1] (ignored if the format has no alpha channel)
    void set(float r, float g, float b, float a = 1.0f)
    {
        value_ = static_cast<uint16_t>(((std::lround(r * RED_MASK)) << RED_OFF) |
                                       ((std::lround(g * GREEN_MASK)) << GREEN_OFF) |
                                       ((std::lround(b * BLUE_MASK)) << BLUE_OFF) |
                                       ((std::lround(a * ALPHA_MASK)) << ALPHA_OFF));
    }

    //! Returns the red component, normalized to [0, 1].
    float red() const { return float((value_ >> RED_OFF) & RED_MASK) / float(RED_MASK); }

    //! Sets the red component.
    //!
    //! @param  r   Red, in [0, 1]
    void setRed(float r)
    {
        value_ = uint16_t((value_ & ~(RED_MASK << RED_OFF)) | std::lround(r * RED_MASK) << RED_OFF);
    }

    //! Returns the green component, normalized to [0, 1].
    float green() const { return float((value_ >> GREEN_OFF) & GREEN_MASK) / float(GREEN_MASK); }

    //! Sets the green component.
    //!
    //! @param  g   Green, in [0, 1]
    void setGreen(float g)
    {
        value_ = uint16_t((value_ & ~(GREEN_MASK << GREEN_OFF)) | (std::lround(g * GREEN_MASK) << GREEN_OFF));
    }

    //! Returns the blue component, normalized to [0, 1].
    float blue() const { return float((value_ >> BLUE_OFF) & BLUE_MASK) / float(BLUE_MASK); }

    //! Sets the blue component.
    //!
    //! @param  b   Blue, in [0, 1]
    void setBlue(float b)
    {
        value_ = uint16_t((value_ & ~(BLUE_MASK << BLUE_OFF)) | (std::lround(b * BLUE_MASK) << BLUE_OFF));
    }

    //! Returns the alpha component normalized to [0, 1], or 1 if the format has no alpha channel.
    float alpha() const { return ALPHA_MASK ? float((value_ >> ALPHA_OFF) & ALPHA_MASK) / float(ALPHA_MASK) : 1.0f; }

    //! Sets the alpha component if the format supports one; otherwise does nothing.
    //!
    //! @param  a   Alpha, in [0, 1]
    void setAlpha(float a)
    {
        if (ALPHA_MASK)
            value_ = uint16_t((value_ & ~(ALPHA_MASK << ALPHA_OFF)) | (std::lround(a * ALPHA_MASK) << ALPHA_OFF));
    }

private:

    uint16_t value_;   // The raw value of the pixel
};

extern template class Pixel16<0, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;
extern template class Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;
extern template class Pixel16<0x0f, 0x0f, 0x0f, 0x0f, 12, 8, 4, 0>;

//! 16-bit R5G6B5 pixel format (no alpha).
using Pixel565 = Pixel16<0, 0x1f, 0x3f, 0x1f,  0, 11, 5, 0>;

//! 16-bit A1R5G5B5 pixel format.
using Pixel1555 = Pixel16<0x01, 0x1f, 0x1f, 0x1f, 15, 10, 5, 0>;

//! 16-bit A4R4G4B4 pixel format.
using Pixel4444 = Pixel16<0x0f, 0x0f, 0x0f, 0x0f, 12, 8, 4, 0>;

//! A packed 24-bit RGB color pixel (three bytes, no alpha).
//!
//! Each component occupies one byte of a 3-byte array; the template parameters give the array index of each component, which
//! selects the byte order (e.g. RGB vs BGR).
//!
//! @tparam RED_OFF     Byte index of the red component
//! @tparam GREEN_OFF   Byte index of the green component
//! @tparam BLUE_OFF    Byte index of the blue component
template <int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel24
{
public:

    //! Constructs a pixel with all components zero.
    Pixel24() = default;

    //! Constructs a pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    Pixel24(float r, float g, float b) { set(r, g, b); }

    //! Constructs a pixel from a raw packed value.
    //!
    //! @param  c   Packed value, with red in bits 16-23, green in 8-15, and blue in 0-7
    explicit Pixel24(unsigned c) { set(c); }

    //! Returns the components packed as @c 0x00RRGGBB.
    unsigned raw() const { return (value_[0] << 16) | (value_[1] <<  8) | (value_[2]); }

    //! Sets the pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    void set(float r, float g, float b) { setRed(r); setGreen(g); setBlue(b); }

    //! Sets the pixel from a raw packed value.
    //!
    //! @param  c   Packed value, with red in bits 16-23, green in 8-15, and blue in 0-7
    void set(unsigned c)
    {
        value_[0] = uint8_t((c >> 16) & 0xff);
        value_[1] = uint8_t((c >>  8) & 0xff);
        value_[2] = uint8_t(c & 0xff);
    }

    //! Returns the red component, normalized to [0, 1].
    float red() const { return float(value_[RED_OFF]) / float(0xff); }

    //! Sets the red component.
    //!
    //! @param  r   Red, in [0, 1]
    void setRed(float r) { value_[RED_OFF] = uint8_t(std::lround(r * 0xff)); }

    //! Returns the green component, normalized to [0, 1].
    float green() const { return float(value_[GREEN_OFF]) / float(0xff); }

    //! Sets the green component.
    //!
    //! @param  g   Green, in [0, 1]
    void setGreen(float g) { value_[GREEN_OFF] = uint8_t(std::lround(g * 0xff)); }

    //! Returns the blue component, normalized to [0, 1].
    float blue() const { return float(value_[BLUE_OFF]) / float(0xff); }

    //! Sets the blue component.
    //!
    //! @param  b   Blue, in [0, 1]
    void setBlue(float b) { value_[BLUE_OFF] = uint8_t(std::lround(b * 0xff)); }

private:
    std::array<uint8_t, 3> value_{}; // Component colors
};

// Pre-instantiated common 24-bit formats.

//! 24-bit R8G8B8 pixel format.
using PixelRGB = Pixel24<0, 1, 2>;
extern template class Pixel24<0, 1, 2>;

//! 24-bit B8G8R8 pixel format.
using PixelBGR = Pixel24<2, 1, 0>;
extern template class Pixel24<2, 1, 0>;

//! A packed 32-bit RGBA color pixel (four bytes).
//!
//! Each component occupies one byte of a 4-byte array; the template parameters give the array index of each component, which
//! selects the byte order (e.g. ARGB vs RGBA).
//!
//! @tparam ALPHA_OFF   Byte index of the alpha component
//! @tparam RED_OFF     Byte index of the red component
//! @tparam GREEN_OFF   Byte index of the green component
//! @tparam BLUE_OFF    Byte index of the blue component
template <int ALPHA_OFF, int RED_OFF, int GREEN_OFF, int BLUE_OFF>
class Pixel32
{
public:

    //! Constructs a pixel with all components zero.
    Pixel32() = default;

    //! Constructs a pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    //! @param  a   Alpha, in [0, 1]
    Pixel32(float r, float g, float b, float a = 1.0f) { set(r, g, b, a); }

    //! Constructs a pixel from a raw packed value.
    //!
    //! @param  c   Packed value, with the first byte in bits 24-31 down to the fourth in bits 0-7
    explicit Pixel32(uint32_t c) { set(c); }

    //! Returns the four component bytes packed into a 32-bit value (byte 0 in the high bits).
    uint32_t raw() const { return (value_[0] << 24) | (value_[1] << 16) | (value_[2] << 8) | value_[3]; }

    //! Sets the pixel from normalized color components.
    //!
    //! @param  r   Red, in [0, 1]
    //! @param  g   Green, in [0, 1]
    //! @param  b   Blue, in [0, 1]
    //! @param  a   Alpha, in [0, 1]
    void set(float r, float g, float b, float a = 1.0f) { setRed(r); setGreen(g); setBlue(b); setAlpha(a); }

    //! Sets the pixel from a raw packed value.
    //!
    //! @param  c   Packed value, with the first byte in bits 24-31 down to the fourth in bits 0-7
    void set(uint32_t c)
    {
        value_[0] = uint8_t((c >> 24) & 0xff);
        value_[1] = uint8_t((c >> 16) & 0xff);
        value_[2] = uint8_t((c >>  8) & 0xff);
        value_[3] = uint8_t(c & 0xff);
    }

    //! Returns the alpha component, normalized to [0, 1].
    float alpha() const { return float(value_[ALPHA_OFF]) / float(0xff); }

    //! Sets the alpha component.
    //!
    //! @param  a   Alpha, in [0, 1]
    void setAlpha(float a) { value_[ALPHA_OFF] = uint8_t(std::lround(a * 0xff)); }

    //! Returns the red component, normalized to [0, 1].
    float red() const { return float(value_[RED_OFF]) / float(0xff); }

    //! Sets the red component.
    //!
    //! @param  r   Red, in [0, 1]
    void setRed(float r) { value_[RED_OFF] = uint8_t(std::lround(r * 0xff)); }

    //! Returns the green component, normalized to [0, 1].
    float green() const { return float(value_[GREEN_OFF]) / float(0xff); }

    //! Sets the green component.
    //!
    //! @param  g   Green, in [0, 1]
    void setGreen(float g) { value_[GREEN_OFF] = uint8_t(std::lround(g * 0xff)); }

    //! Returns the blue component, normalized to [0, 1].
    float blue() const { return float(value_[BLUE_OFF]) / float(0xff); }

    //! Sets the blue component.
    //!
    //! @param  b   Blue, in [0, 1]
    void setBlue(float b) { value_[BLUE_OFF] = uint8_t(std::lround(b * 0xff)); }

private:
    std::array<uint8_t, 4> value_{}; // Component colors
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
