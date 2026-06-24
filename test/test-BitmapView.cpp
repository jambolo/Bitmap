#include "Bitmap/BitmapView.h"
#include "Rect/Rect.h"

#include "gtest/gtest.h"

#include <cstdint>
#include <numeric>

// 1. WholeView
TEST(BitmapConstViewTest, WholeView)
{
	Bitmap<uint16_t> b(8, 6);
	std::iota(b.data(), b.data() + b.width() * b.height(), uint16_t(0));

	BitmapConstView<uint16_t> v = b.cview();

	EXPECT_EQ(v.width(), 8);
	EXPECT_EQ(v.height(), 6);
	EXPECT_EQ(v.pitch(), b.pitch());
	EXPECT_FALSE(v.empty());
	EXPECT_EQ(v.pixel(0, 0), b.pixel(0, 0));
	EXPECT_EQ(v.pixel(5, 3), b.pixel(5, 3));
}

// 2. RegionInheritsPitch
TEST(BitmapConstViewTest, RegionInheritsPitch)
{
	Bitmap<uint16_t> b(8, 6);
	std::iota(b.data(), b.data() + b.width() * b.height(), uint16_t(0));

	auto v = b.cview().region(2, 1, 4, 3);

	EXPECT_EQ(v.width(), 4);
	EXPECT_EQ(v.height(), 3);
	EXPECT_EQ(v.pitch(), b.pitch());
	EXPECT_EQ(v.pixel(0, 0), b.pixel(2, 1));
	EXPECT_EQ(v.pixel(3, 2), b.pixel(5, 3));
}

// 3. NestedRegion
TEST(BitmapConstViewTest, NestedRegion)
{
	Bitmap<uint16_t> b(10, 10);
	std::iota(b.data(), b.data() + b.width() * b.height(), uint16_t(0));

	auto v1 = b.cview().region(1, 1, 8, 8);
	auto v2 = v1.region(1, 1, 6, 6);

	EXPECT_EQ(v2.width(), 6);
	EXPECT_EQ(v2.height(), 6);
	EXPECT_EQ(v2.pitch(), b.pitch());
	EXPECT_EQ(v2.pixel(0, 0), b.pixel(2, 2));
	EXPECT_EQ(v2.pixel(5, 5), b.pixel(7, 7));
}

// 4. MutateThroughView
TEST(BitmapViewTest, MutateThroughView)
{
	Bitmap<uint8_t> b(8, 8);
	b.view().fill(0);

	auto v = b.view();
	v.setPixel(3, 2, 42);
	EXPECT_EQ(b.pixel(3, 2), 42);

	b.view(1, 1, 2, 2).fill(7);
	EXPECT_EQ(b.pixel(1, 1), 7);
	EXPECT_EQ(b.pixel(2, 1), 7);
	EXPECT_EQ(b.pixel(1, 2), 7);
	EXPECT_EQ(b.pixel(2, 2), 7);
	EXPECT_EQ(b.pixel(0, 0), 0);
	EXPECT_EQ(b.pixel(3, 3), 0);
}

// 5. ViewToConstViewConversion
TEST(BitmapConstViewTest, ViewToConstViewConversion)
{
	Bitmap<uint8_t> b(4, 4);
	std::iota(b.data(), b.data() + b.width() * b.height(), uint8_t(0));

	BitmapView<uint8_t> mutableView = b.view();
	BitmapConstView<uint8_t> constView = mutableView; // Implicit conversion

	EXPECT_EQ(mutableView.width(), constView.width());
	EXPECT_EQ(mutableView.height(), constView.height());
	EXPECT_EQ(mutableView.pixel(1, 1), constView.pixel(1, 1));
	EXPECT_EQ(b.pixel(1, 1), constView.pixel(1, 1));
}

// 6. CopyWhole_uint8
TEST(BitmapViewTest, CopyWhole_uint8)
{
	Bitmap<uint8_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint8_t(0));
	Bitmap<uint8_t> dst(8, 8);
	dst.view().fill(0);

	dst.view().copy(src.cview(), Rect{0, 0, 8, 8}, 0, 0);

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			EXPECT_EQ(dst.pixel(x, y), src.pixel(x, y));
		}
	}
}

// 7. CopySubRegionToOrigin_uint8
TEST(BitmapViewTest, CopySubRegionToOrigin_uint8)
{
	Bitmap<uint8_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint8_t(0));
	Bitmap<uint8_t> dst(4, 4);
	dst.view().fill(0);

	dst.view().copy(src.cview(), Rect{2, 3, 3, 2}, 0, 0);

	for (int r = 0; r < 2; ++r) {
		for (int c = 0; c < 3; ++c) {
			EXPECT_EQ(dst.pixel(c, r), src.pixel(2 + c, 3 + r));
		}
	}
	// Check a pixel outside the copied region
	EXPECT_EQ(dst.pixel(3, 3), 0);
}

// 8. CopyPartialOutOfBounds_uint8
TEST(BitmapViewTest, CopyPartialOutOfBounds_uint8)
{
	Bitmap<uint8_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint8_t(0));
	Bitmap<uint8_t> dst(8, 8);
	dst.view().fill(255);

	// Copy an 8x8 region from (6,6) in src to (0,0) in dst.
	// Only the 2x2 top-left part of the source rect is valid.
	dst.view().copy(src.cview(), Rect{6, 6, 8, 8}, 0, 0);

	// Top-left 2x2 should be copied
	EXPECT_EQ(dst.pixel(0, 0), src.pixel(6, 6));
	EXPECT_EQ(dst.pixel(1, 0), src.pixel(7, 6));
	EXPECT_EQ(dst.pixel(0, 1), src.pixel(6, 7));
	EXPECT_EQ(dst.pixel(1, 1), src.pixel(7, 7));

	// A pixel outside the copied area should be unchanged
	EXPECT_EQ(dst.pixel(2, 0), 255);
	EXPECT_EQ(dst.pixel(0, 2), 255);
	EXPECT_EQ(dst.pixel(2, 2), 255);
}

// 9. CopyFullyOutOfBounds_uint8
TEST(BitmapViewTest, CopyFullyOutOfBounds_uint8)
{
	Bitmap<uint8_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint8_t(0));
	Bitmap<uint8_t> dst(8, 8);
	dst.view().fill(255);

	Bitmap<uint8_t> originalDst = dst; // Deep copy

	dst.view().copy(src.cview(), Rect{100, 100, 4, 4}, 0, 0);

	for (int y = 0; y < dst.height(); ++y) {
		for (int x = 0; x < dst.width(); ++x) {
			EXPECT_EQ(dst.pixel(x, y), originalDst.pixel(x, y));
		}
	}
}

// 10. CopyMultiByte_uint16
TEST(BitmapViewTest, CopyMultiByte_uint16)
{
	Bitmap<uint16_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint16_t(0));
	Bitmap<uint16_t> dst(8, 8);
	dst.view().fill(0);

	dst.view().copy(src.cview(), Rect{0, 0, 8, 8}, 0, 0);

	for (int y = 0; y < 8; ++y) {
		for (int x = 0; x < 8; ++x) {
			EXPECT_EQ(dst.pixel(x, y), src.pixel(x, y));
		}
	}
}

// 11. MaterializeFromView
TEST(BitmapViewTest, MaterializeFromView)
{
	const int width = 8;
	const int height = 6;
	const size_t pitch = 16 * sizeof(uint16_t);
	Bitmap<uint16_t> src(width, height, pitch);
	std::iota(src.data(), src.data() + (pitch / sizeof(uint16_t)) * height, uint16_t(0));

	Bitmap<uint16_t> owned(src.cview().region(1, 1, 4, 4));

	EXPECT_EQ(owned.width(), 4);
	EXPECT_EQ(owned.height(), 4);
	EXPECT_EQ(owned.pitch(), 4 * sizeof(uint16_t)); // Compact
	EXPECT_NE(owned.data(), src.data(1, 1));

	for (int y = 0; y < owned.height(); ++y) {
		for (int x = 0; x < owned.width(); ++x) {
			EXPECT_EQ(owned.pixel(x, y), src.pixel(x + 1, y + 1));
		}
	}

	// Mutating owned does not change src
	owned.view().setPixel(0, 0, 9999);
	EXPECT_NE(src.pixel(1, 1), 9999);
}

// 12. LoadFromView
TEST(BitmapViewTest, LoadFromView)
{
	Bitmap<uint8_t> src(8, 8);
	std::iota(src.data(), src.data() + src.width() * src.height(), uint8_t(100));
	Bitmap<uint8_t> target(1, 1);

	target.load(src.cview());

	EXPECT_EQ(target.width(), src.width());
	EXPECT_EQ(target.height(), src.height());
	EXPECT_NE(target.data(), src.data());
	for (int y = 0; y < src.height(); ++y) {
		for (int x = 0; x < src.width(); ++x) {
			EXPECT_EQ(target.pixel(x, y), src.pixel(x, y));
		}
	}
}

// 13. EmptyView
TEST(BitmapConstViewTest, EmptyView)
{
	BitmapConstView<uint8_t> e;
	EXPECT_TRUE(e.empty());
	EXPECT_EQ(e.width(), 0);
	EXPECT_EQ(e.height(), 0);
	EXPECT_EQ(e.data(), nullptr);

	Bitmap<uint8_t> emptyBmp;
	EXPECT_TRUE(emptyBmp.view().empty());
	EXPECT_TRUE(emptyBmp.cview().empty());

	Bitmap<uint8_t> b(10, 10);
	EXPECT_TRUE(b.cview().region(0, 0, 0, 0).empty());
	EXPECT_TRUE(b.cview().region(5, 5, 10, 0).empty());
	EXPECT_TRUE(b.cview().region(5, 5, 0, 10).empty());
}
