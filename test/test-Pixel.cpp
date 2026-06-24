#include "Bitmap/Pixel.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <type_traits>

namespace
{
constexpr float kByteTolerance = 1.0f / 255.0f;
}

TEST(Pixel8Test, IsUnsignedByte)
{
	static_assert(std::is_same<Pixel8, uint8_t>::value, "Pixel8 must be uint8_t");
	EXPECT_EQ(sizeof(Pixel8), 1u);
}

TEST(Pixel16Test, Pixel565SetAndReadChannels)
{
	Pixel565 pixel;

	pixel.setRed(1.0f);
	pixel.setGreen(0.5f);
	pixel.setBlue(0.0f);

	EXPECT_NEAR(pixel.red(), 1.0f, 1.0f / 31.0f);
	EXPECT_NEAR(pixel.green(), 0.5f, 1.0f / 63.0f);
	EXPECT_NEAR(pixel.blue(), 0.0f, 1.0f / 31.0f);
	EXPECT_EQ(pixel.alpha(), 1.0f);
}

TEST(Pixel16Test, Pixel565DefaultConstructorIsZero)
{
	Pixel565 pixel;

	EXPECT_EQ(pixel.raw(), 0u);
	EXPECT_NEAR(pixel.red(), 0.0f, 1.0f / 31.0f);
	EXPECT_NEAR(pixel.green(), 0.0f, 1.0f / 63.0f);
	EXPECT_NEAR(pixel.blue(), 0.0f, 1.0f / 31.0f);
}

TEST(Pixel16Test, Pixel565FloatConstructorPacksChannels)
{
	Pixel565 pixel(1.0f, 0.5f, 0.0f);

	EXPECT_NEAR(pixel.red(), 1.0f, 1.0f / 31.0f);
	EXPECT_NEAR(pixel.green(), 0.5f, 1.0f / 63.0f);
	EXPECT_NEAR(pixel.blue(), 0.0f, 1.0f / 31.0f);
	EXPECT_EQ(pixel.alpha(), 1.0f);
}

TEST(Pixel16Test, Pixel565SetRawAndSetFloat)
{
	Pixel565 pixel;

	pixel.set(0xFFFFu);
	EXPECT_EQ(pixel.raw(), 0xFFFFu);

	pixel.set(0.0f, 1.0f, 0.0f);
	EXPECT_NEAR(pixel.red(), 0.0f, 1.0f / 31.0f);
	EXPECT_NEAR(pixel.green(), 1.0f, 1.0f / 63.0f);
	EXPECT_NEAR(pixel.blue(), 0.0f, 1.0f / 31.0f);
}

TEST(Pixel16Test, Pixel1555RawRoundTrip)
{
	Pixel1555 pixel(0xCB75u);

	EXPECT_EQ(pixel.raw(), 0xCB75u);
	EXPECT_NEAR(pixel.alpha(), 1.0f, 1.0f / 1.0f);
}

TEST(Pixel16Test, Pixel1555AlphaIsSingleBit)
{
	Pixel1555 opaque(1.0f, 0.0f, 0.0f, 1.0f);
	EXPECT_NEAR(opaque.alpha(), 1.0f, 0.001f);

	Pixel1555 transparent(1.0f, 0.0f, 0.0f, 0.0f);
	EXPECT_NEAR(transparent.alpha(), 0.0f, 0.001f);

	transparent.setAlpha(1.0f);
	EXPECT_NEAR(transparent.alpha(), 1.0f, 0.001f);
}

TEST(Pixel16Test, Pixel4444AllChannelsRoundTrip)
{
	Pixel4444 pixel(1.0f, 0.0f, 1.0f, 0.0f);

	EXPECT_NEAR(pixel.red(), 1.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.green(), 0.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.blue(), 1.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.alpha(), 0.0f, 1.0f / 15.0f);

	pixel.setAlpha(1.0f);
	pixel.setGreen(1.0f);
	EXPECT_NEAR(pixel.alpha(), 1.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.green(), 1.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.red(), 1.0f, 1.0f / 15.0f);
	EXPECT_NEAR(pixel.blue(), 1.0f, 1.0f / 15.0f);
}

TEST(Pixel16Test, Pixel565IndividualSettersPreserveOtherChannels)
{
	Pixel565 pixel;

	pixel.setRed(1.0f);
	pixel.setBlue(1.0f);
	pixel.setGreen(0.0f);

	EXPECT_NEAR(pixel.red(), 1.0f, 1.0f / 31.0f);
	EXPECT_NEAR(pixel.green(), 0.0f, 1.0f / 63.0f);
	EXPECT_NEAR(pixel.blue(), 1.0f, 1.0f / 31.0f);
}

TEST(Pixel24Test, PixelRGBRawRoundTrip)
{
	PixelRGB pixel(0x123456u);

	EXPECT_EQ(pixel.raw(), 0x123456u);
	EXPECT_NEAR(pixel.red(), 0x12 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0x34 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0x56 / 255.0f, kByteTolerance);
}

TEST(Pixel24Test, PixelBGRChannelSettersPreserveOrdering)
{
	PixelBGR pixel(0u);

	pixel.setRed(0.2f);
	pixel.setGreen(0.4f);
	pixel.setBlue(0.6f);

	EXPECT_NEAR(pixel.red(), 0.2f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0.4f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0.6f, kByteTolerance);
}

TEST(Pixel32Test, PixelARGBRawRoundTrip)
{
	PixelARGB pixel(0xA1B2C3D4u);

	EXPECT_EQ(pixel.raw(), 0xA1B2C3D4u);
	EXPECT_NEAR(pixel.alpha(), 0xA1 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.red(), 0xB2 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0xC3 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0xD4 / 255.0f, kByteTolerance);
}

TEST(Pixel32Test, PixelRGBAFloatConstructorAndSetters)
{
	PixelRGBA pixel(0.1f, 0.2f, 0.3f, 0.4f);

	EXPECT_NEAR(pixel.red(), 0.1f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0.2f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0.3f, kByteTolerance);
	EXPECT_NEAR(pixel.alpha(), 0.4f, kByteTolerance);

	pixel.set(0x8899AABBu);

	EXPECT_EQ(pixel.raw(), 0x8899AABBu);
	EXPECT_NEAR(pixel.red(), 0x88 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0x99 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0xAA / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.alpha(), 0xBB / 255.0f, kByteTolerance);
}

TEST(Pixel24Test, PixelRGBFloatConstructorAndRaw)
{
	PixelRGB pixel(1.0f, 0.0f, 0.0f);

	EXPECT_EQ(pixel.raw(), 0xFF0000u);
	EXPECT_NEAR(pixel.red(), 1.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0.0f, kByteTolerance);
}

TEST(Pixel24Test, PixelRGBSetFromUnsigned)
{
	PixelRGB pixel(0u);

	pixel.set(0x804020u);

	EXPECT_EQ(pixel.raw(), 0x804020u);
	EXPECT_NEAR(pixel.red(), 0x80 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0x40 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0x20 / 255.0f, kByteTolerance);
}

TEST(Pixel24Test, PixelBGRRawHonorsByteOrdering)
{
	PixelBGR pixel(0u);

	// Raw layout is value_[0]<<16 | value_[1]<<8 | value_[2]; BGR maps red->[2], blue->[0].
	pixel.setRed(1.0f);
	pixel.setBlue(0.0f);

	EXPECT_EQ(pixel.raw() & 0xFFu, 0xFFu);
	EXPECT_EQ((pixel.raw() >> 16) & 0xFFu, 0x00u);
}

TEST(Pixel32Test, PixelRGBADefaultAlphaIsOpaque)
{
	PixelRGBA pixel(0.0f, 0.0f, 0.0f);

	EXPECT_NEAR(pixel.alpha(), 1.0f, kByteTolerance);
}

TEST(Pixel32Test, PixelBGRARawRoundTrip)
{
	PixelBGRA pixel(0x11223344u);

	EXPECT_EQ(pixel.raw(), 0x11223344u);
	// BGRA: blue->[0], green->[1], red->[2], alpha->[3].
	EXPECT_NEAR(pixel.blue(), 0x11 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0x22 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.red(), 0x33 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.alpha(), 0x44 / 255.0f, kByteTolerance);
}

TEST(Pixel32Test, PixelABGRRawRoundTrip)
{
	PixelABGR pixel(0x55667788u);

	EXPECT_EQ(pixel.raw(), 0x55667788u);
	// ABGR: alpha->[0], blue->[1], green->[2], red->[3].
	EXPECT_NEAR(pixel.alpha(), 0x55 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0x66 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 0x77 / 255.0f, kByteTolerance);
	EXPECT_NEAR(pixel.red(), 0x88 / 255.0f, kByteTolerance);
}

TEST(Pixel32Test, PixelARGBIndividualSetters)
{
	PixelARGB pixel(0u);

	pixel.setAlpha(1.0f);
	pixel.setRed(0.0f);
	pixel.setGreen(1.0f);
	pixel.setBlue(0.0f);

	EXPECT_NEAR(pixel.alpha(), 1.0f, kByteTolerance);
	EXPECT_NEAR(pixel.red(), 0.0f, kByteTolerance);
	EXPECT_NEAR(pixel.green(), 1.0f, kByteTolerance);
	EXPECT_NEAR(pixel.blue(), 0.0f, kByteTolerance);
}
