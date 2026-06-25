#include "Bitmap/PalettizedBitmap.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <numeric>
#include <type_traits>
#include <vector>

namespace
{
Palette<PixelRGB> makePalette()
{
    Palette<PixelRGB> palette;
    for (int i = 0; i < static_cast<int>(Palette<PixelRGB>::PALETTE_SIZE); ++i)
        palette[i] = PixelRGB(static_cast<unsigned>(i) << 16 | static_cast<unsigned>(i) << 8 | static_cast<unsigned>(i));
    return palette;
}
}

TEST(PalettizedBitmapTest, StaticTraits)
{
    static_assert(std::is_same<PalettizedBitmap<PixelRGB>::ColorType, PixelRGB>::value,
                  "ColorType must match template argument");
    static_assert(std::is_same<PalettizedBitmap<PixelRGB>::PixelType, uint8_t>::value,
                  "Indices are 8-bit");
    EXPECT_EQ(PalettizedBitmap<PixelRGB>::PIXEL_SIZE, 1u);
}

TEST(PalettizedBitmapTest, DefaultConstructor)
{
    PalettizedBitmap<PixelRGB> const bitmap{};

    EXPECT_EQ(bitmap.width(), 0);
    EXPECT_EQ(bitmap.height(), 0);
    EXPECT_EQ(bitmap.pitch(), 0);
    EXPECT_EQ(bitmap.data(), nullptr);
}

TEST(PalettizedBitmapTest, DimensionConstructor)
{
    PalettizedBitmap<PixelRGB> const bitmap(16, 8);

    EXPECT_EQ(bitmap.width(), 16);
    EXPECT_EQ(bitmap.height(), 8);
    EXPECT_EQ(bitmap.pitch(), 16);
    EXPECT_NE(bitmap.data(), nullptr);
}

TEST(PalettizedBitmapTest, PaletteConstructorStoresPalette)
{
    Palette<PixelRGB> const palette = makePalette();
    PalettizedBitmap<PixelRGB> const bitmap(16, 8, palette);

    EXPECT_EQ(bitmap.width(), 16);
    EXPECT_EQ(bitmap.height(), 8);
    for (int i = 0; i < 256; ++i)
        EXPECT_EQ(bitmap.palette()[i].raw(), palette[i].raw());
}

TEST(PalettizedBitmapTest, DataConstructorCopiesIndicesAndPalette)
{
    int const WIDTH  = 8;
    int const HEIGHT = 4;
    std::vector<uint8_t> indices(WIDTH * HEIGHT);
    std::iota(indices.begin(), indices.end(), uint8_t(0));
    Palette<PixelRGB> const palette = makePalette();

    PalettizedBitmap<PixelRGB> const bitmap(WIDTH, HEIGHT, indices.data(), palette);

    EXPECT_EQ(bitmap.width(), WIDTH);
    EXPECT_EQ(bitmap.height(), HEIGHT);
    EXPECT_NE(bitmap.data(), nullptr);
    EXPECT_NE(reinterpret_cast<void const *>(bitmap.data()), reinterpret_cast<void const *>(indices.data()));
    EXPECT_EQ(memcmp(bitmap.data(), indices.data(), WIDTH * HEIGHT), 0);
    EXPECT_EQ(bitmap.pixel(3, 1), indices[1 * WIDTH + 3]);
    for (int i = 0; i < 256; ++i)
        EXPECT_EQ(bitmap.palette()[i].raw(), palette[i].raw());
}

TEST(PalettizedBitmapTest, Load)
{
    int const WIDTH  = 8;
    int const HEIGHT = 4;
    std::vector<uint8_t> indices(WIDTH * HEIGHT);
    std::iota(indices.begin(), indices.end(), uint8_t(10));
    Palette<PixelRGB> const palette = makePalette();

    PalettizedBitmap<PixelRGB> bitmap;
    bitmap.load(WIDTH, HEIGHT, indices.data(), palette);

    EXPECT_EQ(bitmap.width(), WIDTH);
    EXPECT_EQ(bitmap.height(), HEIGHT);
    EXPECT_EQ(bitmap.pitch(), WIDTH);
    EXPECT_EQ(memcmp(bitmap.data(), indices.data(), WIDTH * HEIGHT), 0);
    for (int i = 0; i < 256; ++i)
        EXPECT_EQ(bitmap.palette()[i].raw(), palette[i].raw());
}

TEST(PalettizedBitmapTest, SetPaletteAndMutableAccess)
{
    PalettizedBitmap<PixelRGB> bitmap(4, 4);

    Palette<PixelRGB> palette = makePalette();
    bitmap.setPalette(palette);
    EXPECT_EQ(bitmap.palette()[100].raw(), palette[100].raw());

    // Non-const palette() returns a reference that mutates the stored palette.
    bitmap.palette()[5] = PixelRGB(0xDEAD12u);
    EXPECT_EQ(bitmap.palette()[5].raw(), 0xDEAD12u);
}

TEST(PalettizedBitmapTest, DecompressedRegion)
{
    int const WIDTH  = 8;
    int const HEIGHT = 8;
    std::vector<uint8_t> indices(WIDTH * HEIGHT);
    std::iota(indices.begin(), indices.end(), uint8_t(0));
    Palette<PixelRGB> const palette = makePalette();
    PalettizedBitmap<PixelRGB> bitmap(WIDTH, HEIGHT, indices.data(), palette);

    Bitmap<PixelRGB> region = bitmap.decompressedRegion(0, 0, WIDTH, HEIGHT);

    EXPECT_EQ(region.width(), WIDTH);
    EXPECT_EQ(region.height(), HEIGHT);
    for (int r = 0; r < HEIGHT; ++r)
        for (int c = 0; c < WIDTH; ++c)
            EXPECT_EQ(region.pixel(c, r).raw(), palette[indices[r * WIDTH + c]].raw());
}
