#include "Bitmap/Bitmap.h"

#include "gtest/gtest.h"

TEST(BitmapTest, Placeholder)
{
    EXPECT_TRUE(false);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int rv = RUN_ALL_TESTS();
    return rv;
}
