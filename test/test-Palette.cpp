#include "Bitmap/Palette.h"

#include "gtest/gtest.h"

#include <array>
#include <type_traits>

TEST(PaletteTest, StaticTraits)
{
	EXPECT_EQ(PaletteRGB::PALETTE_SIZE, 256u);
	EXPECT_EQ(PaletteRGB::ENTRY_SIZE, sizeof(PixelRGB));
	static_assert(std::is_same<PaletteRGB::EntryType, PixelRGB>::value, "EntryType must match template argument");
}

TEST(PaletteTest, ConstSubscriptReturnsByValue)
{
	PaletteRGB palette;
	palette[7] = PixelRGB(0x010203u);

	PaletteRGB const & constPalette = palette;
	PixelRGB entry = constPalette[7];

	EXPECT_EQ(entry.raw(), 0x010203u);
}

TEST(PaletteTest, DefaultConstructionExposesMutableEntries)
{
	PaletteRGB palette;

	palette[0] = PixelRGB(0x010203u);
	palette[255] = PixelRGB(0xA0B0C0u);

	EXPECT_EQ(palette[0].raw(), 0x010203u);
	EXPECT_EQ(palette[255].raw(), 0xA0B0C0u);
}

TEST(PaletteTest, ConstructFromEntryBufferCopiesAllEntries)
{
	std::array<PixelRGB, PaletteRGB::PALETTE_SIZE> source{};

	for (std::size_t i = 0; i < source.size(); ++i)
	{
		source[i] = PixelRGB(static_cast<unsigned>(i) << 16 | static_cast<unsigned>(i) << 8 | static_cast<unsigned>(i));
	}

	PaletteRGB palette(source.data());

	for (std::size_t i = 0; i < source.size(); ++i)
	{
		EXPECT_EQ(palette[static_cast<int>(i)].raw(), source[i].raw());
	}
}

TEST(PaletteTest, EntriesPointerMatchesIndexedAccess)
{
	PaletteBGR palette;

	auto * entries = palette.entries();
	entries[42] = PixelBGR(0xABCDEFu);

	EXPECT_EQ(palette[42].raw(), 0xABCDEFu);

	PaletteBGR const & constPalette = palette;
	EXPECT_EQ(constPalette.entries()[42].raw(), 0xABCDEFu);
}
