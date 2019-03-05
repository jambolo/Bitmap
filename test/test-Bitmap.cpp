#include "Bitmap/Bitmap.h"

#include "gtest/gtest.h"

#include <vector>
#include <numeric>

TEST(BitmapTest, DefaultConstructor)
{
    Bitmap<uint32_t> const empty;
    EXPECT_EQ(empty.width(), 0);
    EXPECT_EQ(empty.height(), 0);
    EXPECT_EQ(empty.pitch(), 0);
    EXPECT_EQ(empty.referenced(), false);
    EXPECT_EQ(empty.data(), nullptr);
    EXPECT_EQ(empty.data(1, 2), nullptr);
    EXPECT_EQ(empty.pixel(0, 0), 0);
    EXPECT_EQ(empty.pixel(1, 2), 0);
}

TEST(BitmapTest, Constructor)
{
    Bitmap<uint16_t> const pitch(255, 127, 256*sizeof(uint16_t));

    EXPECT_EQ(pitch.width(), 255);
    EXPECT_EQ(pitch.height(), 127);
    EXPECT_EQ(pitch.pitch(), 256*sizeof(uint16_t));
    EXPECT_FALSE(pitch.referenced());
    EXPECT_NE(pitch.data(), nullptr);
    EXPECT_EQ(pitch.data(1, 2), pitch.data() + 258);
    EXPECT_EQ(pitch.pixel(0, 0), 0);
    EXPECT_EQ(pitch.pixel(1, 2), 0);

    Bitmap<uint16_t> nopitch(256, 127);

    EXPECT_EQ(nopitch.width(), 256);
    EXPECT_EQ(nopitch.height(), 127);
    EXPECT_EQ(nopitch.pitch(), 512);
    EXPECT_FALSE(nopitch.referenced());
    EXPECT_NE(nopitch.data(), nullptr);
    EXPECT_EQ(nopitch.data(1, 2), nopitch.data() + 258);
    EXPECT_EQ(nopitch.pixel(0, 0), 0);
    EXPECT_EQ(nopitch.pixel(1, 2), 0);
}

TEST(BitmapTest, ReferencedConstructor)
{
    std::vector<uint16_t> data(128*200);
    std::iota(data.begin(), data.end(), 0);

    Bitmap<uint16_t> const pitch(99, 200, 128*sizeof(uint16_t), &data[0]);

    EXPECT_EQ(pitch.width(), 99);
    EXPECT_EQ(pitch.height(), 200);
    EXPECT_EQ(pitch.pitch(), 128*sizeof(uint16_t));
    EXPECT_TRUE(pitch.referenced());
    EXPECT_EQ(pitch.data(), &data[0]);
    EXPECT_EQ(pitch.data(1, 2), &data[130]);
    EXPECT_EQ(pitch.pixel(0, 0), data[0]);
    EXPECT_EQ(pitch.pixel(1, 2), data[130]);

    Bitmap<uint16_t> zeropitch(128, 200, 0, &data[0]);

    EXPECT_EQ(zeropitch.width(), 128);
    EXPECT_EQ(zeropitch.height(), 200);
    EXPECT_EQ(zeropitch.pitch(), 128*sizeof(uint16_t));
    EXPECT_TRUE(zeropitch.referenced());
    EXPECT_EQ(zeropitch.data(), &data[0]);
    EXPECT_EQ(zeropitch.data(1, 2), &data[130]);
    EXPECT_EQ(zeropitch.pixel(0, 0), data[0]);
    EXPECT_EQ(zeropitch.pixel(1, 2), data[130]);
}

TEST(BitmapTest, CopyConstructor)
{
    Bitmap<uint16_t> empty;
    Bitmap<uint16_t> const empty_copy(empty);

    EXPECT_EQ(empty_copy.width(), empty.width());
    EXPECT_EQ(empty_copy.height(), empty.height());
    EXPECT_EQ(empty_copy.pitch(), empty.pitch());
    EXPECT_EQ(empty_copy.referenced(), empty.referenced());
    EXPECT_EQ(empty_copy.data(), nullptr);
    EXPECT_EQ(empty_copy.pixel(0, 0), 0);

    Bitmap<uint16_t> pitch(255, 127, 256*sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data()+127*256, 0);
    Bitmap<uint16_t> const pitch_copy(pitch);

    EXPECT_EQ(pitch_copy.width(), pitch.width());
    EXPECT_EQ(pitch_copy.height(), pitch.height());
    EXPECT_EQ(pitch_copy.pitch(), pitch.pitch());
    EXPECT_EQ(pitch_copy.referenced(), pitch.referenced());
    EXPECT_NE(pitch_copy.data(), pitch.data());
    EXPECT_EQ(memcmp(pitch_copy.data(), pitch.data(), pitch_copy.pitch()*pitch_copy.height()), 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data()+127*256, 0);
    Bitmap<uint16_t> const nopitch_copy(nopitch);

    EXPECT_EQ(nopitch_copy.width(), nopitch.width());
    EXPECT_EQ(nopitch_copy.height(), nopitch.height());
    EXPECT_EQ(nopitch_copy.pitch(), nopitch.pitch());
    EXPECT_EQ(nopitch_copy.referenced(), nopitch.referenced());
    EXPECT_NE(nopitch_copy.data(), nopitch.data());
    EXPECT_EQ(memcmp(nopitch_copy.data(), nopitch.data(), nopitch_copy.pitch()*nopitch_copy.height()), 0);

    std::vector<uint16_t> data(128*200);
    std::iota(data.begin(), data.end(), 0);
    Bitmap<uint16_t> const referenced(99, 200, 128*sizeof(uint16_t), &data[0]);
    Bitmap<uint16_t> const referenced_copy(referenced);

    EXPECT_EQ(referenced_copy.width(), referenced.width());
    EXPECT_EQ(referenced_copy.height(), referenced.height());
    EXPECT_EQ(referenced_copy.pitch(), referenced.pitch());
    EXPECT_EQ(referenced_copy.referenced(), referenced.referenced());
    EXPECT_EQ(referenced_copy.data(), referenced.data());
}

TEST(BitmapTest, MoveConstructor)
{
    Bitmap<uint16_t> const empty;
    Bitmap<uint16_t> empty_copy(empty);
    Bitmap<uint16_t> const empty_moved(std::move(empty_copy));

    EXPECT_EQ(empty_moved.width(), empty.width());
    EXPECT_EQ(empty_moved.height(), empty.height());
    EXPECT_EQ(empty_moved.pitch(), empty.pitch());
    EXPECT_EQ(empty_moved.referenced(), empty.referenced());
    EXPECT_EQ(empty_moved.data(), nullptr);
    EXPECT_EQ(empty_moved.pixel(0, 0), 0);

    Bitmap<uint16_t> pitch(255, 127, 256*sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data()+127*256, 0);
    Bitmap<uint16_t> pitch_copy(pitch);
    Bitmap<uint16_t> const pitch_moved(std::move(pitch_copy));

    EXPECT_EQ(pitch_moved.width(), pitch.width());
    EXPECT_EQ(pitch_moved.height(), pitch.height());
    EXPECT_EQ(pitch_moved.pitch(), pitch.pitch());
    EXPECT_EQ(pitch_moved.referenced(), pitch.referenced());
    EXPECT_NE(pitch_moved.data(), pitch.data());
    EXPECT_EQ(memcmp(pitch_moved.data(), pitch.data(), pitch_moved.pitch()*pitch_moved.height()), 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data()+127*256, 0);
    Bitmap<uint16_t> nopitch_copy(nopitch);
    Bitmap<uint16_t> const nopitch_moved(std::move(nopitch_copy));

    EXPECT_EQ(nopitch_moved.width(), nopitch.width());
    EXPECT_EQ(nopitch_moved.height(), nopitch.height());
    EXPECT_EQ(nopitch_moved.pitch(), nopitch.pitch());
    EXPECT_EQ(nopitch_moved.referenced(), nopitch.referenced());
    EXPECT_NE(nopitch_moved.data(), nopitch.data());
    EXPECT_EQ(memcmp(nopitch_moved.data(), nopitch.data(), nopitch_moved.pitch()*nopitch_moved.height()), 0);

    std::vector<uint16_t> data(128*200);
    std::iota(data.begin(), data.end(), 0);
    Bitmap<uint16_t> const referenced(99, 200, 128*sizeof(uint16_t), &data[0]);
    Bitmap<uint16_t> referenced_copy(referenced);
    Bitmap<uint16_t> const referenced_moved(std::move(referenced_copy));

    EXPECT_EQ(referenced_moved.width(), referenced.width());
    EXPECT_EQ(referenced_moved.height(), referenced.height());
    EXPECT_EQ(referenced_moved.pitch(), referenced.pitch());
    EXPECT_EQ(referenced_moved.referenced(), referenced.referenced());
    EXPECT_EQ(referenced_moved.data(), referenced.data());
}

TEST(BitmapTest, AssignmentOperator)
{
    Bitmap<uint16_t> const empty;

    Bitmap<uint16_t> pitch(255, 127, 256*sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data()+127*256, 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data()+127*256, 0x8000);

    std::vector<uint16_t> data(128*200);
    std::iota(data.begin(), data.end(), 0);
    Bitmap<uint16_t> const referenced(99, 200, 128*sizeof(uint16_t), &data[0]);

    // Assignment to empty bitmap

    Bitmap<uint16_t> empty_to_empty_copy;
    empty_to_empty_copy = empty;

    EXPECT_EQ(empty_to_empty_copy.width(), empty.width());
    EXPECT_EQ(empty_to_empty_copy.height(), empty.height());
    EXPECT_EQ(empty_to_empty_copy.pitch(), empty.pitch());
    EXPECT_EQ(empty_to_empty_copy.referenced(), empty.referenced());
    EXPECT_EQ(empty_to_empty_copy.data(), nullptr);
    EXPECT_EQ(empty_to_empty_copy.pixel(0, 0), 0);

    Bitmap<uint16_t> empty_to_pitch_copy;
    empty_to_pitch_copy = pitch;

    EXPECT_EQ(empty_to_pitch_copy.width(), pitch.width());
    EXPECT_EQ(empty_to_pitch_copy.height(), pitch.height());
    EXPECT_EQ(empty_to_pitch_copy.pitch(), pitch.pitch());
    EXPECT_EQ(empty_to_pitch_copy.referenced(), pitch.referenced());
    EXPECT_NE(empty_to_pitch_copy.data(), pitch.data());
    EXPECT_EQ(memcmp(empty_to_pitch_copy.data(), pitch.data(), empty_to_pitch_copy.pitch()*empty_to_pitch_copy.height()), 0);

    Bitmap<uint16_t> empty_to_nopitch_copy;
    empty_to_nopitch_copy = nopitch;

    EXPECT_EQ(empty_to_nopitch_copy.width(), nopitch.width());
    EXPECT_EQ(empty_to_nopitch_copy.height(), nopitch.height());
    EXPECT_EQ(empty_to_nopitch_copy.pitch(), nopitch.pitch());
    EXPECT_EQ(empty_to_nopitch_copy.referenced(), nopitch.referenced());
    EXPECT_NE(empty_to_nopitch_copy.data(), nopitch.data());
    EXPECT_EQ(memcmp(empty_to_nopitch_copy.data(), nopitch.data(), empty_to_nopitch_copy.pitch()*empty_to_nopitch_copy.height()), 0);

    Bitmap<uint16_t> empty_to_referenced_copy;
    empty_to_referenced_copy = referenced;

    EXPECT_EQ(empty_to_referenced_copy.width(), referenced.width());
    EXPECT_EQ(empty_to_referenced_copy.height(), referenced.height());
    EXPECT_EQ(empty_to_referenced_copy.pitch(), referenced.pitch());
    EXPECT_EQ(empty_to_referenced_copy.referenced(), referenced.referenced());
    EXPECT_EQ(empty_to_referenced_copy.data(), referenced.data());

    // Assignment to pitch bitmap

    Bitmap<uint16_t> pitch_to_empty_copy(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 1000);
    pitch_to_empty_copy = empty;

    EXPECT_EQ(pitch_to_empty_copy.width(), empty.width());
    EXPECT_EQ(pitch_to_empty_copy.height(), empty.height());
    EXPECT_EQ(pitch_to_empty_copy.pitch(), empty.pitch());
    EXPECT_EQ(pitch_to_empty_copy.referenced(), empty.referenced());
    EXPECT_EQ(pitch_to_empty_copy.data(), nullptr);
    EXPECT_EQ(pitch_to_empty_copy.pixel(0, 0), 0);

    Bitmap<uint16_t> pitch_to_pitch_copy(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 2000);
    pitch_to_pitch_copy = pitch;

    EXPECT_EQ(pitch_to_pitch_copy.width(), pitch.width());
    EXPECT_EQ(pitch_to_pitch_copy.height(), pitch.height());
    EXPECT_EQ(pitch_to_pitch_copy.pitch(), pitch.pitch());
    EXPECT_EQ(pitch_to_pitch_copy.referenced(), pitch.referenced());
    EXPECT_NE(pitch_to_pitch_copy.data(), pitch.data());
    EXPECT_EQ(memcmp(pitch_to_pitch_copy.data(), pitch.data(), pitch_to_pitch_copy.pitch()*pitch_to_pitch_copy.height()), 0);

    Bitmap<uint16_t> pitch_to_nopitch_copy(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 3000);
    pitch_to_nopitch_copy = nopitch;

    EXPECT_EQ(pitch_to_nopitch_copy.width(), nopitch.width());
    EXPECT_EQ(pitch_to_nopitch_copy.height(), nopitch.height());
    EXPECT_EQ(pitch_to_nopitch_copy.pitch(), nopitch.pitch());
    EXPECT_EQ(pitch_to_nopitch_copy.referenced(), nopitch.referenced());
    EXPECT_NE(pitch_to_nopitch_copy.data(), nopitch.data());
    EXPECT_EQ(memcmp(pitch_to_nopitch_copy.data(), nopitch.data(), pitch_to_nopitch_copy.pitch()*pitch_to_nopitch_copy.height()), 0);

    Bitmap<uint16_t> pitch_to_referenced_copy(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 4000);
    pitch_to_referenced_copy = referenced;

    EXPECT_EQ(pitch_to_referenced_copy.width(), referenced.width());
    EXPECT_EQ(pitch_to_referenced_copy.height(), referenced.height());
    EXPECT_EQ(pitch_to_referenced_copy.pitch(), referenced.pitch());
    EXPECT_EQ(pitch_to_referenced_copy.referenced(), referenced.referenced());
    EXPECT_EQ(pitch_to_referenced_copy.data(), referenced.data());

    // Assignment to nopitch bitmap

    Bitmap<uint16_t> nopitch_to_empty_copy(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 1000);
    nopitch_to_empty_copy = empty;

    EXPECT_EQ(nopitch_to_empty_copy.width(), empty.width());
    EXPECT_EQ(nopitch_to_empty_copy.height(), empty.height());
    EXPECT_EQ(nopitch_to_empty_copy.pitch(), empty.pitch());
    EXPECT_EQ(nopitch_to_empty_copy.referenced(), empty.referenced());
    EXPECT_EQ(nopitch_to_empty_copy.data(), nullptr);
    EXPECT_EQ(nopitch_to_empty_copy.pixel(0, 0), 0);

    Bitmap<uint16_t> nopitch_to_pitch_copy(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 2000);
    nopitch_to_pitch_copy = pitch;

    EXPECT_EQ(nopitch_to_pitch_copy.width(), pitch.width());
    EXPECT_EQ(nopitch_to_pitch_copy.height(), pitch.height());
    EXPECT_EQ(nopitch_to_pitch_copy.pitch(), pitch.pitch());
    EXPECT_EQ(nopitch_to_pitch_copy.referenced(), pitch.referenced());
    EXPECT_NE(nopitch_to_pitch_copy.data(), pitch.data());
    EXPECT_EQ(memcmp(nopitch_to_pitch_copy.data(), pitch.data(), nopitch_to_pitch_copy.pitch()*nopitch_to_pitch_copy.height()), 0);

    Bitmap<uint16_t> nopitch_to_nopitch_copy(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 3000);
    nopitch_to_nopitch_copy = nopitch;

    EXPECT_EQ(nopitch_to_nopitch_copy.width(), nopitch.width());
    EXPECT_EQ(nopitch_to_nopitch_copy.height(), nopitch.height());
    EXPECT_EQ(nopitch_to_nopitch_copy.pitch(), nopitch.pitch());
    EXPECT_EQ(nopitch_to_nopitch_copy.referenced(), nopitch.referenced());
    EXPECT_NE(nopitch_to_nopitch_copy.data(), nopitch.data());
    EXPECT_EQ(memcmp(nopitch_to_nopitch_copy.data(), nopitch.data(), nopitch_to_nopitch_copy.pitch()*nopitch_to_nopitch_copy.height()), 0);

    Bitmap<uint16_t> nopitch_to_referenced_copy(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 4000);
    nopitch_to_referenced_copy = referenced;

    EXPECT_EQ(nopitch_to_referenced_copy.width(), referenced.width());
    EXPECT_EQ(nopitch_to_referenced_copy.height(), referenced.height());
    EXPECT_EQ(nopitch_to_referenced_copy.pitch(), referenced.pitch());
    EXPECT_EQ(nopitch_to_referenced_copy.referenced(), referenced.referenced());
    EXPECT_EQ(nopitch_to_referenced_copy.data(), referenced.data());

    // Assignment to referenced bitmap

    std::vector<uint16_t> overwritten(256*256);
    std::iota(data.begin(), data.end(), 0);

    Bitmap<uint16_t> referenced_to_empty_copy(128, 128, 0, &overwritten[0x0f0f]);
    referenced_to_empty_copy = empty;

    EXPECT_EQ(referenced_to_empty_copy.width(), empty.width());
    EXPECT_EQ(referenced_to_empty_copy.height(), empty.height());
    EXPECT_EQ(referenced_to_empty_copy.pitch(), empty.pitch());
    EXPECT_EQ(referenced_to_empty_copy.referenced(), empty.referenced());
    EXPECT_EQ(referenced_to_empty_copy.data(), nullptr);
    EXPECT_EQ(referenced_to_empty_copy.pixel(0, 0), 0);

    Bitmap<uint16_t> referenced_to_pitch_copy(128, 128, 0, &overwritten[0x1f0f]);
    referenced_to_pitch_copy = pitch;

    EXPECT_EQ(referenced_to_pitch_copy.width(), pitch.width());
    EXPECT_EQ(referenced_to_pitch_copy.height(), pitch.height());
    EXPECT_EQ(referenced_to_pitch_copy.pitch(), pitch.pitch());
    EXPECT_EQ(referenced_to_pitch_copy.referenced(), pitch.referenced());
    EXPECT_NE(referenced_to_pitch_copy.data(), pitch.data());
    EXPECT_EQ(memcmp(referenced_to_pitch_copy.data(), pitch.data(), referenced_to_pitch_copy.pitch()*referenced_to_pitch_copy.height()), 0);

    Bitmap<uint16_t> referenced_to_nopitch_copy(128, 128, 0, &overwritten[0x2f0f]);
    referenced_to_nopitch_copy = nopitch;

    EXPECT_EQ(referenced_to_nopitch_copy.width(), nopitch.width());
    EXPECT_EQ(referenced_to_nopitch_copy.height(), nopitch.height());
    EXPECT_EQ(referenced_to_nopitch_copy.pitch(), nopitch.pitch());
    EXPECT_EQ(referenced_to_nopitch_copy.referenced(), nopitch.referenced());
    EXPECT_NE(referenced_to_nopitch_copy.data(), nopitch.data());
    EXPECT_EQ(memcmp(referenced_to_nopitch_copy.data(), nopitch.data(), referenced_to_nopitch_copy.pitch()*referenced_to_nopitch_copy.height()), 0);

    Bitmap<uint16_t> referenced_to_referenced_copy(128, 128, 0, &overwritten[0x3f0f]);
    referenced_to_referenced_copy = referenced;

    EXPECT_EQ(referenced_to_referenced_copy.width(), referenced.width());
    EXPECT_EQ(referenced_to_referenced_copy.height(), referenced.height());
    EXPECT_EQ(referenced_to_referenced_copy.pitch(), referenced.pitch());
    EXPECT_EQ(referenced_to_referenced_copy.referenced(), referenced.referenced());
    EXPECT_EQ(referenced_to_referenced_copy.data(), referenced.data());
}

#if 0
TEST(BitmapTest, MoveAssignmentOperator)
{
    Bitmap<uint16_t> const empty;

    Bitmap<uint16_t> pitch(255, 127, 256*sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data()+127*256, 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data()+127*256, 0x8000);

    std::vector<uint16_t> data(128*200);
    std::iota(data.begin(), data.end(), 0);
    Bitmap<uint16_t> const referenced(99, 200, 128*sizeof(uint16_t), &data[0]);

    // Assignment to empty bitmap

    Bitmap<uint16_t> empty_to_empty_moved;
    empty_to_empty_moved = std::moved(copy_of_empty;

    EXPECT_EQ(empty_to_empty_moved.width(), empty.width());
    EXPECT_EQ(empty_to_empty_moved.height(), empty.height());
    EXPECT_EQ(empty_to_empty_moved.pitch(), empty.pitch());
    EXPECT_EQ(empty_to_empty_moved.referenced(), empty.referenced());
    EXPECT_EQ(empty_to_empty_moved.data(), nullptr);
    EXPECT_EQ(empty_to_empty_moved.pixel(0, 0), 0);

    Bitmap<uint16_t> empty_to_pitch_moved;
    empty_to_pitch_moved = std::moved(copy_of_pitch;

    EXPECT_EQ(empty_to_pitch_moved.width(), pitch.width());
    EXPECT_EQ(empty_to_pitch_moved.height(), pitch.height());
    EXPECT_EQ(empty_to_pitch_moved.pitch(), pitch.pitch());
    EXPECT_EQ(empty_to_pitch_moved.referenced(), pitch.referenced());
    EXPECT_NE(empty_to_pitch_moved.data(), pitch.data());
    EXPECT_EQ(memcmp(empty_to_pitch_moved.data(), pitch.data(), empty_to_pitch_moved.pitch()*empty_to_pitch_moved.height()), 0);

    Bitmap<uint16_t> empty_to_nopitch_moved;
    empty_to_nopitch_moved = std::moved(copy_of_nopitch;

    EXPECT_EQ(empty_to_nopitch_moved.width(), nopitch.width());
    EXPECT_EQ(empty_to_nopitch_moved.height(), nopitch.height());
    EXPECT_EQ(empty_to_nopitch_moved.pitch(), nopitch.pitch());
    EXPECT_EQ(empty_to_nopitch_moved.referenced(), nopitch.referenced());
    EXPECT_NE(empty_to_nopitch_moved.data(), nopitch.data());
    EXPECT_EQ(memcmp(empty_to_nopitch_moved.data(), nopitch.data(), empty_to_nopitch_moved.pitch()*empty_to_nopitch_moved.height()), 0);

    Bitmap<uint16_t> empty_to_referenced_moved;
    empty_to_referenced_moved = std::moved(copy_of_referenced;

    EXPECT_EQ(empty_to_referenced_moved.width(), referenced.width());
    EXPECT_EQ(empty_to_referenced_moved.height(), referenced.height());
    EXPECT_EQ(empty_to_referenced_moved.pitch(), referenced.pitch());
    EXPECT_EQ(empty_to_referenced_moved.referenced(), referenced.referenced());
    EXPECT_EQ(empty_to_referenced_moved.data(), referenced.data());

    // Assignment to pitch bitmap

    Bitmap<uint16_t> pitch_to_empty_moved(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 1000);
    pitch_to_empty_moved = std::moved(copy_of_empty;

    EXPECT_EQ(pitch_to_empty_moved.width(), empty.width());
    EXPECT_EQ(pitch_to_empty_moved.height(), empty.height());
    EXPECT_EQ(pitch_to_empty_moved.pitch(), empty.pitch());
    EXPECT_EQ(pitch_to_empty_moved.referenced(), empty.referenced());
    EXPECT_EQ(pitch_to_empty_moved.data(), nullptr);
    EXPECT_EQ(pitch_to_empty_moved.pixel(0, 0), 0);

    Bitmap<uint16_t> pitch_to_pitch_moved(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 2000);
    pitch_to_pitch_moved = std::moved(copy_of_pitch;

    EXPECT_EQ(pitch_to_pitch_moved.width(), pitch.width());
    EXPECT_EQ(pitch_to_pitch_moved.height(), pitch.height());
    EXPECT_EQ(pitch_to_pitch_moved.pitch(), pitch.pitch());
    EXPECT_EQ(pitch_to_pitch_moved.referenced(), pitch.referenced());
    EXPECT_NE(pitch_to_pitch_moved.data(), pitch.data());
    EXPECT_EQ(memcmp(pitch_to_pitch_moved.data(), pitch.data(), pitch_to_pitch_moved.pitch()*pitch_to_pitch_moved.height()), 0);

    Bitmap<uint16_t> pitch_to_nopitch_moved(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 3000);
    pitch_to_nopitch_moved = std::moved(copy_of_nopitch;

    EXPECT_EQ(pitch_to_nopitch_moved.width(), nopitch.width());
    EXPECT_EQ(pitch_to_nopitch_moved.height(), nopitch.height());
    EXPECT_EQ(pitch_to_nopitch_moved.pitch(), nopitch.pitch());
    EXPECT_EQ(pitch_to_nopitch_moved.referenced(), nopitch.referenced());
    EXPECT_NE(pitch_to_nopitch_moved.data(), nopitch.data());
    EXPECT_EQ(memcmp(pitch_to_nopitch_moved.data(), nopitch.data(), pitch_to_nopitch_moved.pitch()*pitch_to_nopitch_moved.height()), 0);

    Bitmap<uint16_t> pitch_to_referenced_moved(10, 10, 100);
    std::iota(pitch.data(), pitch.data()+10*100, 4000);
    pitch_to_referenced_moved = std::moved(copy_of_referenced;

    EXPECT_EQ(pitch_to_referenced_moved.width(), referenced.width());
    EXPECT_EQ(pitch_to_referenced_moved.height(), referenced.height());
    EXPECT_EQ(pitch_to_referenced_moved.pitch(), referenced.pitch());
    EXPECT_EQ(pitch_to_referenced_moved.referenced(), referenced.referenced());
    EXPECT_EQ(pitch_to_referenced_moved.data(), referenced.data());

    // Assignment to nopitch bitmap

    Bitmap<uint16_t> nopitch_to_empty_moved(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 1000);
    nopitch_to_empty_moved = std::moved(copy_of_empty;

    EXPECT_EQ(nopitch_to_empty_moved.width(), empty.width());
    EXPECT_EQ(nopitch_to_empty_moved.height(), empty.height());
    EXPECT_EQ(nopitch_to_empty_moved.pitch(), empty.pitch());
    EXPECT_EQ(nopitch_to_empty_moved.referenced(), empty.referenced());
    EXPECT_EQ(nopitch_to_empty_moved.data(), nullptr);
    EXPECT_EQ(nopitch_to_empty_moved.pixel(0, 0), 0);

    Bitmap<uint16_t> nopitch_to_pitch_moved(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 2000);
    nopitch_to_pitch_moved = std::moved(copy_of_pitch;

    EXPECT_EQ(nopitch_to_pitch_moved.width(), pitch.width());
    EXPECT_EQ(nopitch_to_pitch_moved.height(), pitch.height());
    EXPECT_EQ(nopitch_to_pitch_moved.pitch(), pitch.pitch());
    EXPECT_EQ(nopitch_to_pitch_moved.referenced(), pitch.referenced());
    EXPECT_NE(nopitch_to_pitch_moved.data(), pitch.data());
    EXPECT_EQ(memcmp(nopitch_to_pitch_moved.data(), pitch.data(), nopitch_to_pitch_moved.pitch()*nopitch_to_pitch_moved.height()), 0);

    Bitmap<uint16_t> nopitch_to_nopitch_moved(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 3000);
    nopitch_to_nopitch_moved = std::moved(copy_of_nopitch;

    EXPECT_EQ(nopitch_to_nopitch_moved.width(), nopitch.width());
    EXPECT_EQ(nopitch_to_nopitch_moved.height(), nopitch.height());
    EXPECT_EQ(nopitch_to_nopitch_moved.pitch(), nopitch.pitch());
    EXPECT_EQ(nopitch_to_nopitch_moved.referenced(), nopitch.referenced());
    EXPECT_NE(nopitch_to_nopitch_moved.data(), nopitch.data());
    EXPECT_EQ(memcmp(nopitch_to_nopitch_moved.data(), nopitch.data(), nopitch_to_nopitch_moved.pitch()*nopitch_to_nopitch_moved.height()), 0);

    Bitmap<uint16_t> nopitch_to_referenced_moved(10, 10);
    std::iota(pitch.data(), pitch.data()+10*10, 4000);
    nopitch_to_referenced_moved = std::moved(copy_of_referenced;

    EXPECT_EQ(nopitch_to_referenced_moved.width(), referenced.width());
    EXPECT_EQ(nopitch_to_referenced_moved.height(), referenced.height());
    EXPECT_EQ(nopitch_to_referenced_moved.pitch(), referenced.pitch());
    EXPECT_EQ(nopitch_to_referenced_moved.referenced(), referenced.referenced());
    EXPECT_EQ(nopitch_to_referenced_moved.data(), referenced.data());


    // Assignment to referenced bitmap

    std::vector<uint16_t> overwritten(256*256);
    std::iota(data.begin(), data.end(), 0);

    Bitmap<uint16_t> referenced_to_empty_moved(128, 128, 0, &overwritten[0x0f0f]);
    referenced_to_empty_moved = std::moved(copy_of_empty;

    EXPECT_EQ(referenced_to_empty_moved.width(), empty.width());
    EXPECT_EQ(referenced_to_empty_moved.height(), empty.height());
    EXPECT_EQ(referenced_to_empty_moved.pitch(), empty.pitch());
    EXPECT_EQ(referenced_to_empty_moved.referenced(), empty.referenced());
    EXPECT_EQ(referenced_to_empty_moved.data(), nullptr);
    EXPECT_EQ(referenced_to_empty_moved.pixel(0, 0), 0);

    Bitmap<uint16_t> referenced_to_pitch_moved(128, 128, 0, &overwritten[0x1f0f]);
    referenced_to_pitch_moved = std::moved(copy_of_pitch;

    EXPECT_EQ(referenced_to_pitch_moved.width(), pitch.width());
    EXPECT_EQ(referenced_to_pitch_moved.height(), pitch.height());
    EXPECT_EQ(referenced_to_pitch_moved.pitch(), pitch.pitch());
    EXPECT_EQ(referenced_to_pitch_moved.referenced(), pitch.referenced());
    EXPECT_NE(referenced_to_pitch_moved.data(), pitch.data());
    EXPECT_EQ(memcmp(referenced_to_pitch_moved.data(), pitch.data(), referenced_to_pitch_moved.pitch()*referenced_to_pitch_moved.height()), 0);

    Bitmap<uint16_t> referenced_to_nopitch_moved(128, 128, 0, &overwritten[0x2f0f]);
    referenced_to_nopitch_moved = std::moved(copy_of_nopitch;

    EXPECT_EQ(referenced_to_nopitch_moved.width(), nopitch.width());
    EXPECT_EQ(referenced_to_nopitch_moved.height(), nopitch.height());
    EXPECT_EQ(referenced_to_nopitch_moved.pitch(), nopitch.pitch());
    EXPECT_EQ(referenced_to_nopitch_moved.referenced(), nopitch.referenced());
    EXPECT_NE(referenced_to_nopitch_moved.data(), nopitch.data());
    EXPECT_EQ(memcmp(referenced_to_nopitch_moved.data(), nopitch.data(), referenced_to_nopitch_moved.pitch()*referenced_to_nopitch_moved.height()), 0);

    Bitmap<uint16_t> referenced_to_referenced_moved(128, 128, 0, &overwritten[0x3f0f]);
    referenced_to_referenced_moved = std::moved(copy_of_referenced;

    EXPECT_EQ(referenced_to_referenced_moved.width(), referenced.width());
    EXPECT_EQ(referenced_to_referenced_moved.height(), referenced.height());
    EXPECT_EQ(referenced_to_referenced_moved.pitch(), referenced.pitch());
    EXPECT_EQ(referenced_to_referenced_moved.referenced(), referenced.referenced());
    EXPECT_EQ(referenced_to_referenced_moved.data(), referenced.data());
}
#endif

#if 0
//! Loads an image
void load(int w, int h, int p, Pixel const * data);

//! References an image
void reference(int w, int h, int p, Pixel * data);

//! Returns the pixel at (row, column)
Pixel pixel(int row, int column) const { return *addressOf(row, column); }

//! Returns a pointer to the pixel at (row, column).
Pixel const * data(int row = 0, int column = 0) const { return addressOf(row, column); }

//! Returns a pointer to the pixel at (row, column).
Pixel * data(int row = 0, int column = 0) { return const_cast<Pixel *>(addressOf(row, column)); }

//! Returns the width of the bitmap (in pixels).
int width() const { return width_; }

//! Returns the height of the bitmap (in pixels).
int height() const { return height_; }

//! Returns the pitch of the bitmap (in bytes).
int pitch() const { return pitch_; }

//! Creates a bitmap from a region of this bitmap.
Bitmap region(int row, int column, int width, int height, int pitch = 0) const;

//! Copies a section of another bitmap into this bitmap
void copy(Bitmap const & src, Rect const & srcRect, int dstC, int dstR);
#endif

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
