#include "Bitmap/Bitmap.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <vector>

static bool imageIsSame(void const * a, void const * b, int width, int height, size_t pitchA, size_t pitchB)
{
    for (int i = 0; i < height; ++i)
    {
        if (memcmp(a, b, width) != 0)
            return false;
        a = reinterpret_cast<char const *>(a) + pitchA;
        b = reinterpret_cast<char const *>(b) + pitchB;
    }
    return true;
}

TEST(BitmapTest, EmptyConstructor)
{
    Bitmap<uint32_t> const empty_constructed;
    EXPECT_EQ(empty_constructed.width(), 0);
    EXPECT_EQ(empty_constructed.height(), 0);
    EXPECT_EQ(empty_constructed.pitch(), 0);
    EXPECT_EQ(empty_constructed.data(), nullptr);
    EXPECT_EQ(empty_constructed.data(2, 1), nullptr);
    EXPECT_EQ(empty_constructed.pixel(0, 0), 0);
    EXPECT_EQ(empty_constructed.pixel(2, 1), 0);
}

TEST(BitmapTest, Constructor)
{
    {
        Bitmap<uint16_t> const pitch_constructed(255, 127, 256 * sizeof(uint16_t));

        EXPECT_EQ(pitch_constructed.width(), 255);
        EXPECT_EQ(pitch_constructed.height(), 127);
        EXPECT_EQ(pitch_constructed.pitch(), 256 * sizeof(uint16_t));
        EXPECT_NE(pitch_constructed.data(), nullptr);
        EXPECT_EQ(pitch_constructed.data(2, 1), pitch_constructed.data() + 258);
    }

    {
        Bitmap<uint16_t> nopitch_constructed(256, 127);

        EXPECT_EQ(nopitch_constructed.width(), 256);
        EXPECT_EQ(nopitch_constructed.height(), 127);
        EXPECT_EQ(nopitch_constructed.pitch(), 512);
        EXPECT_NE(nopitch_constructed.data(), nullptr);
        EXPECT_EQ(nopitch_constructed.data(2, 1), nopitch_constructed.data() + 258);
    }
}

TEST(BitmapTest, ReferencedConstructor)
{
    std::vector<uint16_t> data(128 * 200);
    std::iota(data.begin(), data.end(), 0);

    {
        int const WIDTH  = 99;
        int const HEIGHT = 200;
        size_t    PITCH  = 128 * sizeof(uint16_t);
        uint16_t const * const DATA = &data[0];

        Bitmap<uint16_t> const referenced_pitch_constructed(WIDTH, HEIGHT, PITCH, DATA);

        EXPECT_EQ(referenced_pitch_constructed.width(), WIDTH);
        EXPECT_EQ(referenced_pitch_constructed.height(), HEIGHT);
        EXPECT_EQ(referenced_pitch_constructed.pitch(), PITCH);
        EXPECT_NE(referenced_pitch_constructed.data(), nullptr);
        EXPECT_NE(referenced_pitch_constructed.data(), DATA);
        EXPECT_EQ(memcmp(referenced_pitch_constructed.data(), DATA, WIDTH * sizeof(uint16_t) * HEIGHT), 0);
        EXPECT_EQ(referenced_pitch_constructed.pixel(0, 0), *DATA);
        EXPECT_EQ(referenced_pitch_constructed.pixel(2, 1), *(DATA + 130));
    }

    {
        int const WIDTH  = 128;
        int const HEIGHT = 200;
        size_t    PITCH  = 0;
        uint16_t const * const DATA = &data[0];

        Bitmap<uint16_t> referenced_zeropitch_constructed(128, 200, 0, DATA);

        EXPECT_EQ(referenced_zeropitch_constructed.width(), WIDTH);
        EXPECT_EQ(referenced_zeropitch_constructed.height(), HEIGHT);
        EXPECT_EQ(referenced_zeropitch_constructed.pitch(), WIDTH * sizeof(uint16_t));
        EXPECT_NE(referenced_zeropitch_constructed.data(), nullptr);
        EXPECT_NE(referenced_zeropitch_constructed.data(), DATA);
        EXPECT_EQ(memcmp(referenced_zeropitch_constructed.data(), DATA, WIDTH * sizeof(uint16_t) * HEIGHT), 0);
        EXPECT_EQ(referenced_zeropitch_constructed.pixel(0, 0), *DATA);
        EXPECT_EQ(referenced_zeropitch_constructed.pixel(2, 1), *(DATA + 130));
    }
}

TEST(BitmapTest, CopyConstructor)
{
    Bitmap<uint16_t> empty;
    Bitmap<uint16_t> pitch(255, 127, 256 * sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data() + 127 * 256, 0);
    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data() + 127 * 256, 0);

    {
        Bitmap<uint16_t> const empty_copy(empty);

        EXPECT_EQ(empty_copy.width(), empty.width());
        EXPECT_EQ(empty_copy.height(), empty.height());
        EXPECT_EQ(empty_copy.pitch(), empty.pitch());
        EXPECT_EQ(empty_copy.data(), nullptr);
        EXPECT_EQ(empty_copy.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> const pitch_copy(pitch);

        EXPECT_EQ(pitch_copy.width(), pitch.width());
        EXPECT_EQ(pitch_copy.height(), pitch.height());
        EXPECT_EQ(pitch_copy.pitch(), pitch.pitch());
        EXPECT_NE(pitch_copy.data(), nullptr);
        EXPECT_NE(pitch_copy.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_copy.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> const nopitch_copy(nopitch);

        EXPECT_EQ(nopitch_copy.width(), nopitch.width());
        EXPECT_EQ(nopitch_copy.height(), nopitch.height());
        EXPECT_EQ(nopitch_copy.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_copy.data(), nullptr);
        EXPECT_NE(nopitch_copy.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_copy.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }
}

TEST(BitmapTest, MoveConstructor)
{
    Bitmap<uint16_t> const empty;
    Bitmap<uint16_t>       pitch(255, 127, 256 * sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data() + 127 * 256, 0);
    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data() + 127 * 256, 0);

    {
        Bitmap<uint16_t>       copy(empty);
        Bitmap<uint16_t> const empty_moved(std::move(copy));

        EXPECT_EQ(empty_moved.width(), empty.width());
        EXPECT_EQ(empty_moved.height(), empty.height());
        EXPECT_EQ(empty_moved.pitch(), empty.pitch());
        EXPECT_EQ(empty_moved.data(), nullptr);
        EXPECT_EQ(empty_moved.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t>       copy(pitch);
        Bitmap<uint16_t> const pitch_moved(std::move(copy));

        EXPECT_EQ(pitch_moved.width(), pitch.width());
        EXPECT_EQ(pitch_moved.height(), pitch.height());
        EXPECT_EQ(pitch_moved.pitch(), pitch.pitch());
        EXPECT_NE(pitch_moved.data(), nullptr);
        EXPECT_NE(pitch_moved.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_moved.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t>       copy(nopitch);
        Bitmap<uint16_t> const nopitch_moved(std::move(copy));

        EXPECT_EQ(nopitch_moved.width(), nopitch.width());
        EXPECT_EQ(nopitch_moved.height(), nopitch.height());
        EXPECT_EQ(nopitch_moved.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_moved.data(), nullptr);
        EXPECT_NE(nopitch_moved.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_moved.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }
}

TEST(BitmapTest, AssignmentOperator)
{
    Bitmap<uint16_t> const empty;

    Bitmap<uint16_t> pitch(255, 127, 256 * sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data() + 127 * 256, 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data() + 127 * 256, 0x8000);


    // Assignment to empty bitmap

    {
        Bitmap<uint16_t> empty_assigned_to_empty;
        empty_assigned_to_empty = empty;

        EXPECT_EQ(empty_assigned_to_empty.width(), empty.width());
        EXPECT_EQ(empty_assigned_to_empty.height(), empty.height());
        EXPECT_EQ(empty_assigned_to_empty.pitch(), empty.pitch());
        EXPECT_EQ(empty_assigned_to_empty.data(), nullptr);
        EXPECT_EQ(empty_assigned_to_empty.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_assigned_to_empty;
        pitch_assigned_to_empty = pitch;

        EXPECT_EQ(pitch_assigned_to_empty.width(), pitch.width());
        EXPECT_EQ(pitch_assigned_to_empty.height(), pitch.height());
        EXPECT_EQ(pitch_assigned_to_empty.pitch(), pitch.pitch());
        EXPECT_NE(pitch_assigned_to_empty.data(), nullptr);
        EXPECT_NE(pitch_assigned_to_empty.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_assigned_to_empty.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_assigned_to_empty;
        nopitch_assigned_to_empty = nopitch;

        EXPECT_EQ(nopitch_assigned_to_empty.width(), nopitch.width());
        EXPECT_EQ(nopitch_assigned_to_empty.height(), nopitch.height());
        EXPECT_EQ(nopitch_assigned_to_empty.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_assigned_to_empty.data(), nullptr);
        EXPECT_NE(nopitch_assigned_to_empty.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_assigned_to_empty.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }

    // Assignment to pitch bitmap

    {
        Bitmap<uint16_t> empty_assigned_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(empty_assigned_to_pitch.data(), empty_assigned_to_pitch.data() + 10 * 20, 1000);
        empty_assigned_to_pitch = empty;

        EXPECT_EQ(empty_assigned_to_pitch.width(), empty.width());
        EXPECT_EQ(empty_assigned_to_pitch.height(), empty.height());
        EXPECT_EQ(empty_assigned_to_pitch.pitch(), empty.pitch());
        EXPECT_EQ(empty_assigned_to_pitch.data(), nullptr);
        EXPECT_EQ(empty_assigned_to_pitch.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_assigned_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(pitch_assigned_to_pitch.data(), pitch_assigned_to_pitch.data() + 10 * 20, 2000);
        pitch_assigned_to_pitch = pitch;

        EXPECT_EQ(pitch_assigned_to_pitch.width(), pitch.width());
        EXPECT_EQ(pitch_assigned_to_pitch.height(), pitch.height());
        EXPECT_EQ(pitch_assigned_to_pitch.pitch(), pitch.pitch());
        EXPECT_NE(pitch_assigned_to_pitch.data(), nullptr);
        EXPECT_NE(pitch_assigned_to_pitch.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_assigned_to_pitch.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_assigned_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(nopitch_assigned_to_pitch.data(), nopitch_assigned_to_pitch.data() + 10 * 20, 3000);
        nopitch_assigned_to_pitch = nopitch;

        EXPECT_EQ(nopitch_assigned_to_pitch.width(), nopitch.width());
        EXPECT_EQ(nopitch_assigned_to_pitch.height(), nopitch.height());
        EXPECT_EQ(nopitch_assigned_to_pitch.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_assigned_to_pitch.data(), nullptr);
        EXPECT_NE(nopitch_assigned_to_pitch.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_assigned_to_pitch.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }

    // Assignment to nopitch bitmap

    {
        Bitmap<uint16_t> empty_assigned_to_nopitch(10, 10);
        std::iota(empty_assigned_to_nopitch.data(), empty_assigned_to_nopitch.data() + 10 * 10, 1000);
        empty_assigned_to_nopitch = empty;

        EXPECT_EQ(empty_assigned_to_nopitch.width(), empty.width());
        EXPECT_EQ(empty_assigned_to_nopitch.height(), empty.height());
        EXPECT_EQ(empty_assigned_to_nopitch.pitch(), empty.pitch());
        EXPECT_EQ(empty_assigned_to_nopitch.data(), nullptr);
        EXPECT_EQ(empty_assigned_to_nopitch.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_assigned_to_nopitch(10, 10);
        std::iota(pitch_assigned_to_nopitch.data(), pitch_assigned_to_nopitch.data() + 10 * 10, 2000);
        pitch_assigned_to_nopitch = pitch;

        EXPECT_EQ(pitch_assigned_to_nopitch.width(), pitch.width());
        EXPECT_EQ(pitch_assigned_to_nopitch.height(), pitch.height());
        EXPECT_EQ(pitch_assigned_to_nopitch.pitch(), pitch.pitch());
        EXPECT_NE(pitch_assigned_to_nopitch.data(), nullptr);
        EXPECT_NE(pitch_assigned_to_nopitch.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_assigned_to_nopitch.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_assigned_to_nopitch(10, 10);
        std::iota(nopitch_assigned_to_nopitch.data(), nopitch_assigned_to_nopitch.data() + 10 * 10, 3000);
        nopitch_assigned_to_nopitch = nopitch;

        EXPECT_EQ(nopitch_assigned_to_nopitch.width(), nopitch.width());
        EXPECT_EQ(nopitch_assigned_to_nopitch.height(), nopitch.height());
        EXPECT_EQ(nopitch_assigned_to_nopitch.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_assigned_to_nopitch.data(), nullptr);
        EXPECT_NE(nopitch_assigned_to_nopitch.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_assigned_to_nopitch.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }
}

TEST(BitmapTest, MoveAssignmentOperator)
{
    Bitmap<uint16_t> const empty;

    Bitmap<uint16_t> pitch(255, 127, 256 * sizeof(uint16_t));
    std::iota(pitch.data(), pitch.data() + 127 * 256, 0);

    Bitmap<uint16_t> nopitch(256, 127);
    std::iota(nopitch.data(), nopitch.data() + 127 * 256, 0x8000);


    // Assignment to empty bitmap

    {
        Bitmap<uint16_t> empty_moved_to_empty;
        Bitmap<uint16_t> empty_copy(empty);
        empty_moved_to_empty = std::move(empty_copy);

        EXPECT_EQ(empty_moved_to_empty.width(), empty.width());
        EXPECT_EQ(empty_moved_to_empty.height(), empty.height());
        EXPECT_EQ(empty_moved_to_empty.pitch(), empty.pitch());
        EXPECT_EQ(empty_moved_to_empty.data(), nullptr);
        EXPECT_EQ(empty_moved_to_empty.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_moved_to_empty;
        Bitmap<uint16_t> pitch_copy(pitch);
        pitch_moved_to_empty = std::move(pitch_copy);

        EXPECT_EQ(pitch_moved_to_empty.width(), pitch.width());
        EXPECT_EQ(pitch_moved_to_empty.height(), pitch.height());
        EXPECT_EQ(pitch_moved_to_empty.pitch(), pitch.pitch());
        EXPECT_NE(pitch_moved_to_empty.data(), nullptr);
        EXPECT_NE(pitch_moved_to_empty.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_moved_to_empty.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_moved_to_empty;
        Bitmap<uint16_t> nopitch_copy(nopitch);
        nopitch_moved_to_empty = std::move(nopitch_copy);

        EXPECT_EQ(nopitch_moved_to_empty.width(), nopitch.width());
        EXPECT_EQ(nopitch_moved_to_empty.height(), nopitch.height());
        EXPECT_EQ(nopitch_moved_to_empty.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_moved_to_empty.data(), nullptr);
        EXPECT_NE(nopitch_moved_to_empty.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_moved_to_empty.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }

    // Assignment to pitch bitmap

    {
        Bitmap<uint16_t> empty_moved_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(empty_moved_to_pitch.data(), empty_moved_to_pitch.data() + 10 * 20, 1000);
        Bitmap<uint16_t> empty_copy(empty);
        empty_moved_to_pitch = std::move(empty_copy);

        EXPECT_EQ(empty_moved_to_pitch.width(), empty.width());
        EXPECT_EQ(empty_moved_to_pitch.height(), empty.height());
        EXPECT_EQ(empty_moved_to_pitch.pitch(), empty.pitch());
        EXPECT_EQ(empty_moved_to_pitch.data(), nullptr);
        EXPECT_EQ(empty_moved_to_pitch.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_moved_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(pitch_moved_to_pitch.data(), pitch_moved_to_pitch.data() + 10 * 20, 2000);
        Bitmap<uint16_t> pitch_copy(pitch);
        pitch_moved_to_pitch = std::move(pitch_copy);

        EXPECT_EQ(pitch_moved_to_pitch.width(), pitch.width());
        EXPECT_EQ(pitch_moved_to_pitch.height(), pitch.height());
        EXPECT_EQ(pitch_moved_to_pitch.pitch(), pitch.pitch());
        EXPECT_NE(pitch_moved_to_pitch.data(), nullptr);
        EXPECT_NE(pitch_moved_to_pitch.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_moved_to_pitch.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_moved_to_pitch(10, 10, 20 * sizeof(uint16_t));
        std::iota(nopitch_moved_to_pitch.data(), nopitch_moved_to_pitch.data() + 10 * 20, 3000);
        Bitmap<uint16_t> nopitch_copy(nopitch);
        nopitch_moved_to_pitch = std::move(nopitch_copy);

        EXPECT_EQ(nopitch_moved_to_pitch.width(), nopitch.width());
        EXPECT_EQ(nopitch_moved_to_pitch.height(), nopitch.height());
        EXPECT_EQ(nopitch_moved_to_pitch.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_moved_to_pitch.data(), nullptr);
        EXPECT_NE(nopitch_moved_to_pitch.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_moved_to_pitch.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }

    // Assignment to nopitch bitmap

    {
        Bitmap<uint16_t> empty_moved_to_nopitch(10, 10);
        std::iota(empty_moved_to_nopitch.data(), empty_moved_to_nopitch.data() + 10 * 10, 1000);
        Bitmap<uint16_t> empty_copy(empty);
        empty_moved_to_nopitch = std::move(empty_copy);

        EXPECT_EQ(empty_moved_to_nopitch.width(), empty.width());
        EXPECT_EQ(empty_moved_to_nopitch.height(), empty.height());
        EXPECT_EQ(empty_moved_to_nopitch.pitch(), empty.pitch());
        EXPECT_EQ(empty_moved_to_nopitch.data(), nullptr);
        EXPECT_EQ(empty_moved_to_nopitch.pixel(0, 0), 0);
    }

    {
        Bitmap<uint16_t> pitch_moved_to_nopitch(10, 10);
        std::iota(pitch_moved_to_nopitch.data(), pitch_moved_to_nopitch.data() + 10 * 10, 2000);
        Bitmap<uint16_t> pitch_copy(pitch);
        pitch_moved_to_nopitch = std::move(pitch_copy);

        EXPECT_EQ(pitch_moved_to_nopitch.width(), pitch.width());
        EXPECT_EQ(pitch_moved_to_nopitch.height(), pitch.height());
        EXPECT_EQ(pitch_moved_to_nopitch.pitch(), pitch.pitch());
        EXPECT_NE(pitch_moved_to_nopitch.data(), nullptr);
        EXPECT_NE(pitch_moved_to_nopitch.data(), pitch.data());
        EXPECT_EQ(memcmp(pitch_moved_to_nopitch.data(), pitch.data(), pitch.pitch() * pitch.height()), 0);
    }

    {
        Bitmap<uint16_t> nopitch_moved_to_nopitch(10, 10);
        std::iota(nopitch_moved_to_nopitch.data(), nopitch_moved_to_nopitch.data() + 10 * 10, 3000);
        Bitmap<uint16_t> nopitch_copy(nopitch);
        nopitch_moved_to_nopitch = std::move(nopitch_copy);

        EXPECT_EQ(nopitch_moved_to_nopitch.width(), nopitch.width());
        EXPECT_EQ(nopitch_moved_to_nopitch.height(), nopitch.height());
        EXPECT_EQ(nopitch_moved_to_nopitch.pitch(), nopitch.pitch());
        EXPECT_NE(nopitch_moved_to_nopitch.data(), nullptr);
        EXPECT_NE(nopitch_moved_to_nopitch.data(), nopitch.data());
        EXPECT_EQ(memcmp(nopitch_moved_to_nopitch.data(), nopitch.data(), nopitch.pitch() * nopitch.height()), 0);
    }
}

TEST(BitmapTest, Load)
{
    std::vector<uint16_t> data(256 * 256);
    std::iota(data.begin(), data.end(), 0);

    // Load empty bitmap

    {
        int constexpr    LOAD_WIDTH  = 27;
        int constexpr    LOAD_HEIGHT = 16;
        int constexpr    LOAD_PITCH  = 64;
        uint16_t * const LOAD_DATA   = &data[0];
        Bitmap<uint16_t> pitch_loaded_into_empty;
        pitch_loaded_into_empty.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(pitch_loaded_into_empty.width(), LOAD_WIDTH);
        EXPECT_EQ(pitch_loaded_into_empty.height(), LOAD_HEIGHT);
        EXPECT_EQ(pitch_loaded_into_empty.pitch(), LOAD_PITCH);
        EXPECT_NE(pitch_loaded_into_empty.data(), nullptr);
        EXPECT_NE(pitch_loaded_into_empty.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(pitch_loaded_into_empty.data(), LOAD_DATA, LOAD_PITCH * LOAD_HEIGHT), 0);
    }

    {
        int constexpr    LOAD_WIDTH  = 32;
        int constexpr    LOAD_HEIGHT = 17;
        int constexpr    LOAD_PITCH  = 0;
        uint16_t * const LOAD_DATA   = &data[3];
        Bitmap<uint16_t> zeropitch_loaded_into_empty;
        zeropitch_loaded_into_empty.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(zeropitch_loaded_into_empty.width(), LOAD_WIDTH);
        EXPECT_EQ(zeropitch_loaded_into_empty.height(), LOAD_HEIGHT);
        EXPECT_EQ(zeropitch_loaded_into_empty.pitch(), LOAD_WIDTH * sizeof(uint16_t));
        EXPECT_NE(zeropitch_loaded_into_empty.data(), nullptr);
        EXPECT_NE(zeropitch_loaded_into_empty.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(zeropitch_loaded_into_empty.data(), LOAD_DATA, LOAD_WIDTH * sizeof(uint16_t) * LOAD_HEIGHT), 0);
    }

    // Load pitch bitmap

    {
        int constexpr    LOAD_WIDTH  = 27;
        int constexpr    LOAD_HEIGHT = 16;
        int constexpr    LOAD_PITCH  = 64;
        uint16_t * const LOAD_DATA   = &data[0];
        Bitmap<uint16_t> pitch_loaded_into_pitch(10, 10, 32 * sizeof(uint16_t));
        std::iota(pitch_loaded_into_pitch.data(), pitch_loaded_into_pitch.data() + 10 * 32, 2000);
        pitch_loaded_into_pitch.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(pitch_loaded_into_pitch.width(), LOAD_WIDTH);
        EXPECT_EQ(pitch_loaded_into_pitch.height(), LOAD_HEIGHT);
        EXPECT_EQ(pitch_loaded_into_pitch.pitch(), LOAD_PITCH);
        EXPECT_NE(pitch_loaded_into_pitch.data(), nullptr);
        EXPECT_NE(pitch_loaded_into_pitch.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(pitch_loaded_into_pitch.data(), LOAD_DATA, LOAD_PITCH * LOAD_HEIGHT), 0);
    }

    {
        int constexpr    LOAD_WIDTH  = 32;
        int constexpr    LOAD_HEIGHT = 17;
        int constexpr    LOAD_PITCH  = 0;
        uint16_t * const LOAD_DATA   = &data[3];
        Bitmap<uint16_t> zeropitch_loaded_into_pitch(10, 10, 32 * sizeof(uint16_t));
        std::iota(zeropitch_loaded_into_pitch.data(), zeropitch_loaded_into_pitch.data() + 10 * 32, 3000);
        zeropitch_loaded_into_pitch.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(zeropitch_loaded_into_pitch.width(), LOAD_WIDTH);
        EXPECT_EQ(zeropitch_loaded_into_pitch.height(), LOAD_HEIGHT);
        EXPECT_EQ(zeropitch_loaded_into_pitch.pitch(), LOAD_WIDTH * sizeof(uint16_t));
        EXPECT_NE(zeropitch_loaded_into_pitch.data(), nullptr);
        EXPECT_NE(zeropitch_loaded_into_pitch.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(zeropitch_loaded_into_pitch.data(), LOAD_DATA, LOAD_WIDTH * sizeof(uint16_t) * LOAD_HEIGHT), 0);
    }

    // Load nopitch bitmap

    {
        int constexpr    LOAD_WIDTH  = 27;
        int constexpr    LOAD_HEIGHT = 16;
        int constexpr    LOAD_PITCH  = 64;
        uint16_t * const LOAD_DATA   = &data[0];
        Bitmap<uint16_t> pitch_loaded_into_nopitch(10, 10);
        std::iota(pitch_loaded_into_nopitch.data(), pitch_loaded_into_nopitch.data() + 10 * 10, 2000);
        pitch_loaded_into_nopitch.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(pitch_loaded_into_nopitch.width(), LOAD_WIDTH);
        EXPECT_EQ(pitch_loaded_into_nopitch.height(), LOAD_HEIGHT);
        EXPECT_EQ(pitch_loaded_into_nopitch.pitch(), LOAD_PITCH);
        EXPECT_NE(pitch_loaded_into_nopitch.data(), nullptr);
        EXPECT_NE(pitch_loaded_into_nopitch.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(pitch_loaded_into_nopitch.data(), LOAD_DATA, LOAD_PITCH * LOAD_HEIGHT), 0);
    }

    {
        int constexpr    LOAD_WIDTH  = 32;
        int constexpr    LOAD_HEIGHT = 17;
        int constexpr    LOAD_PITCH  = 0;
        uint16_t * const LOAD_DATA   = &data[3];
        Bitmap<uint16_t> zeropitch_loaded_into_nopitch(10, 10);
        std::iota(zeropitch_loaded_into_nopitch.data(), zeropitch_loaded_into_nopitch.data() + 10 * 10, 3000);
        zeropitch_loaded_into_nopitch.load(LOAD_WIDTH, LOAD_HEIGHT, LOAD_PITCH, LOAD_DATA);

        EXPECT_EQ(zeropitch_loaded_into_nopitch.width(), LOAD_WIDTH);
        EXPECT_EQ(zeropitch_loaded_into_nopitch.height(), LOAD_HEIGHT);
        EXPECT_EQ(zeropitch_loaded_into_nopitch.pitch(), LOAD_WIDTH * sizeof(uint16_t));
        EXPECT_NE(zeropitch_loaded_into_nopitch.data(), nullptr);
        EXPECT_NE(zeropitch_loaded_into_nopitch.data(), LOAD_DATA);
        EXPECT_EQ(memcmp(zeropitch_loaded_into_nopitch.data(), LOAD_DATA, LOAD_WIDTH * sizeof(uint16_t) * LOAD_HEIGHT), 0);
    }
}

TEST(BitmapTest, Region)
{
    Bitmap<uint16_t> empty;
    Bitmap<uint16_t> source(127, 256, 256 * sizeof(uint16_t)); // note: pitch vs. width
    std::iota(source.data(), source.data() + 256 * 256, 0);

    // Region of an empty bitmap

    {
        int const REGION_X      = 0;
        int const REGION_Y      = 0;
        int const REGION_WIDTH  = 64;
        int const REGION_HEIGHT = 64;

        Bitmap<uint16_t> region = empty.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT);

        EXPECT_EQ(region.width(), 0);
        EXPECT_EQ(region.height(), 0);
        EXPECT_EQ(region.pitch(), 0);
        EXPECT_EQ(region.data(), nullptr);
    }

    {
        int const    REGION_X      = 0;
        int const    REGION_Y      = 0;
        int const    REGION_WIDTH  = 64;
        int const    REGION_HEIGHT = 64;
        size_t const REGION_PITCH  = 128 * sizeof(uint16_t);

        Bitmap<uint16_t> region = empty.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT, REGION_PITCH);

        EXPECT_EQ(region.width(), 0);
        EXPECT_EQ(region.height(), 0);
        EXPECT_EQ(region.pitch(), 0);
        EXPECT_EQ(region.data(), nullptr);
    }

    // Region of an entire bitmap

    {
        int const REGION_X      = 0;
        int const REGION_Y      = 0;
        int const REGION_WIDTH  = source.width();
        int const REGION_HEIGHT = source.height();

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT);

        EXPECT_EQ(region.width(), REGION_WIDTH);
        EXPECT_EQ(region.height(), REGION_HEIGHT);
        EXPECT_EQ(region.pitch(), REGION_WIDTH * sizeof(uint16_t));
        EXPECT_TRUE(
            imageIsSame(source.data(REGION_X, REGION_Y),
                        region.data(0, 0),
                        REGION_WIDTH,
                        REGION_HEIGHT,
                        source.pitch(),
                        REGION_WIDTH * sizeof(uint16_t)));
    }

    {
        int const    REGION_X      = 0;
        int const    REGION_Y      = 0;
        int const    REGION_WIDTH  = source.width();
        int const    REGION_HEIGHT = source.height();
        size_t const REGION_PITCH  = source.width() * 2 * sizeof(uint16_t);

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT, REGION_PITCH);

        EXPECT_EQ(region.width(), REGION_WIDTH);
        EXPECT_EQ(region.height(), REGION_HEIGHT);
        EXPECT_EQ(region.pitch(), REGION_PITCH);
        EXPECT_TRUE(
            imageIsSame(source.data(REGION_X, REGION_Y),
                        region.data(0, 0),
                        REGION_WIDTH,
                        REGION_HEIGHT,
                        source.pitch(),
                        REGION_PITCH));
    }

    // Region of a portion of a bitmap

    {
        int const REGION_X      = 31;
        int const REGION_Y      = 63;
        int const REGION_WIDTH  = 33;
        int const REGION_HEIGHT = 65;

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT);

        EXPECT_EQ(region.width(), REGION_WIDTH);
        EXPECT_EQ(region.height(), REGION_HEIGHT);
        EXPECT_EQ(region.pitch(), REGION_WIDTH * sizeof(uint16_t));
        EXPECT_TRUE(
            imageIsSame(source.data(REGION_X, REGION_Y),
                        region.data(0, 0),
                        REGION_WIDTH,
                        REGION_HEIGHT,
                        source.pitch(),
                        REGION_WIDTH * sizeof(uint16_t)));
    }

    {
        int const    REGION_X      = 31;
        int const    REGION_Y      = 63;
        int const    REGION_WIDTH  = 33;
        int const    REGION_HEIGHT = 65;
        size_t const REGION_PITCH  = REGION_WIDTH * 2 * sizeof(uint16_t);

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT, REGION_PITCH);

        EXPECT_EQ(region.width(), REGION_WIDTH);
        EXPECT_EQ(region.height(), REGION_HEIGHT);
        EXPECT_EQ(region.pitch(), REGION_PITCH);
        EXPECT_TRUE(
            imageIsSame(source.data(REGION_X, REGION_Y),
                        region.data(0, 0),
                        REGION_WIDTH,
                        REGION_HEIGHT,
                        source.pitch(),
                        REGION_PITCH));
    }

    // Region outside of bounds of bitmap

    {
        int const REGION_X      = -source.width();
        int const REGION_Y      = -source.height();
        int const REGION_WIDTH  = 4 * source.width();
        int const REGION_HEIGHT = 4 * source.height();

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT);

        EXPECT_EQ(region.width(), source.width());
        EXPECT_EQ(region.height(), source.height());
        EXPECT_EQ(region.pitch(), source.width() * sizeof(uint16_t));
        EXPECT_TRUE(
            imageIsSame(source.data(0, 0),
                        region.data(0, 0),
                        source.width(),
                        source.height(),
                        source.pitch(),
                        source.width() * sizeof(uint16_t)));
    }

    {
        int const    REGION_X      = -source.width();
        int const    REGION_Y      = -source.height();
        int const    REGION_WIDTH  = 4 * source.width();
        int const    REGION_HEIGHT = 4 * source.height();
        size_t const REGION_PITCH  = source.width() * 2 * sizeof(uint16_t);

        Bitmap<uint16_t> region = source.region(REGION_X, REGION_Y, REGION_WIDTH, REGION_HEIGHT, REGION_PITCH);

        EXPECT_EQ(region.width(), source.width());
        EXPECT_EQ(region.height(), source.height());
        EXPECT_EQ(region.pitch(), REGION_PITCH);
        EXPECT_TRUE(
            imageIsSame(source.data(0, 0),
                        region.data(0, 0),
                        source.width(),
                        source.height(),
                        source.pitch(),
                        REGION_PITCH));
    }
}

TEST(BitmapTest, Pixel)
{
    std::vector<uint16_t> data(128 * 200);
    std::iota(data.begin(), data.end(), 0);

    {
        int const WIDTH  = 99;
        int const HEIGHT = 200;
        size_t    PITCH  = 128 * sizeof(uint16_t);
        uint16_t const * const DATA = &data[0];

        Bitmap<uint16_t> const constructed(WIDTH, HEIGHT, PITCH, DATA);
        EXPECT_EQ(constructed.pixel(0, 0), *DATA);
        EXPECT_EQ(constructed.pixel(2, 1), *(DATA + 130));
    }

    {
        int const WIDTH  = 128;
        int const HEIGHT = 200;
        size_t    PITCH  = 0;
        uint16_t const * const DATA = &data[0];

        Bitmap<uint16_t> constructed(128, 200, 0, DATA);
        EXPECT_EQ(constructed.pixel(0, 0), *DATA);
        EXPECT_EQ(constructed.pixel(2, 1), *(DATA + 130));
    }
}

TEST(BitmapTest, StaticTraits)
{
    static_assert(std::is_same<Bitmap<uint32_t>::PixelType, uint32_t>::value, "PixelType must match template argument");
    EXPECT_EQ(Bitmap<uint32_t>::PIXEL_SIZE, sizeof(uint32_t));
    EXPECT_EQ(Bitmap<uint8_t>::PIXEL_SIZE, 1u);
}

TEST(BitmapTest, Clear)
{
    Bitmap<uint16_t> bitmap(256, 127);
    std::iota(bitmap.data(), bitmap.data() + 127 * 256, 0);

    bitmap.clear();

    EXPECT_EQ(bitmap.width(), 0);
    EXPECT_EQ(bitmap.height(), 0);
    EXPECT_EQ(bitmap.pitch(), 0);
    EXPECT_EQ(bitmap.data(), nullptr);
    EXPECT_EQ(bitmap.pixel(0, 0), 0);

    // Clearing an already-empty bitmap is a no-op.
    bitmap.clear();
    EXPECT_EQ(bitmap.width(), 0);
    EXPECT_EQ(bitmap.data(), nullptr);
}

TEST(BitmapTest, Copy)
{
    // Use an 8-bit pixel so that pitch == width and the byte and pixel strides coincide.
    Bitmap<uint8_t> source(8, 8);
    std::iota(source.data(), source.data() + 8 * 8, 0);

    // Whole-bitmap copy into a zeroed destination.
    {
        Bitmap<uint8_t> dst(8, 8);
        std::fill(dst.data(), dst.data() + 8 * 8, uint8_t(0));

        dst.copy(source, Rect{ 0, 0, 8, 8 }, 0, 0);

        EXPECT_TRUE(imageIsSame(dst.data(), source.data(), 8, 8, dst.pitch(), source.pitch()));
    }

    // Sub-region copied to the destination origin.
    {
        Bitmap<uint8_t> dst(8, 8);
        std::fill(dst.data(), dst.data() + 8 * 8, uint8_t(0));

        dst.copy(source, Rect{ 2, 3, 3, 2 }, 0, 0);

        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 3; ++c)
                EXPECT_EQ(dst.pixel(c, r), source.pixel(2 + c, 3 + r));
    }

    // A source rect partially outside the source is clipped, not copied out of bounds.
    {
        Bitmap<uint8_t> dst(8, 8);
        std::fill(dst.data(), dst.data() + 8 * 8, uint8_t(0xAA));

        dst.copy(source, Rect{ 6, 6, 8, 8 }, 0, 0);

        // Only the in-bounds 2x2 corner of the source is copied.
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 2; ++c)
                EXPECT_EQ(dst.pixel(c, r), source.pixel(6 + c, 6 + r));
        // Untouched destination pixels keep their fill value.
        EXPECT_EQ(dst.pixel(7, 7), 0xAA);
    }

    // A fully out-of-bounds source rect copies nothing.
    {
        Bitmap<uint8_t> dst(8, 8);
        std::fill(dst.data(), dst.data() + 8 * 8, uint8_t(0xAA));

        dst.copy(source, Rect{ 100, 100, 4, 4 }, 0, 0);

        for (int i = 0; i < 8 * 8; ++i)
            EXPECT_EQ(dst.data()[i], 0xAA);
    }
}

int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
