#include "mglass/mglass.h"
#include "resources/lenna_data.h"   // resources::lenna_512_512::*
#include "gtest/gtest.h"
#include <utility>                  // std::move, std::pair
#include <sstream>                  // std::stringstream


// ====================================================================================================================
// ctors
// ====================================================================================================================

TEST(MGLASS_IMAGE, CTOR_DEFAULT)
{
    const mglass::Image img;

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_0_0)
{
    const mglass::Image img{0, 0};

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_0_1)
{
    const mglass::Image img{0, 1};

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_1_0)
{
    const mglass::Image img{1, 0};

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_100_0)
{
    const mglass::Image img{100, 0};

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_0_100)
{
    const mglass::Image img{0, 100};

    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, CTOR_100_200)
{
    const mglass::Image img{100, 200};

    ASSERT_EQ(img.getWidth(), 100);
    ASSERT_EQ(img.getHeight(), 200);

    for (mglass::size_type y = 0; y < img.getHeight(); ++y)
    {
        for (mglass::size_type x = 0; x < img.getWidth(); ++x)
        {
            ASSERT_EQ(img.getPixelAt(x, y), mglass::ARGB::transparent());
        }
    }
}

TEST(MGLASS_IMAGE, CTOR_100_200_COLORED)
{
    constexpr mglass::ARGB color{192, 20, 138, 45};

    const mglass::Image img{100, 200, color};

    ASSERT_EQ(img.getWidth(), 100);
    ASSERT_EQ(img.getHeight(), 200);

    for (mglass::size_type y = 0; y < img.getHeight(); ++y)
    {
        for (mglass::size_type x = 0; x < img.getWidth(); ++x)
        {
            ASSERT_EQ(img.getPixelAt(x, y), color);
        }
    }
}


TEST(MGLASS_IMAGE, COPYCTOR_FROM_EMPTY)
{
    mglass::Image from;
    mglass::Image to{from};

    ASSERT_EQ(to.getWidth(), from.getWidth());
    ASSERT_EQ(to.getHeight(), from.getHeight());

    EXPECT_TRUE( (to == from) );
    EXPECT_FALSE( (to != from) );

    to.setSize(30, 30);

    ASSERT_EQ(from.getWidth(), 0);
    ASSERT_EQ(from.getHeight(), 0);

    ASSERT_EQ(to.getWidth(), 30);
    ASSERT_EQ(to.getHeight(), 30);

    EXPECT_FALSE( (to == from) );
    EXPECT_TRUE( (to != from) );
}

TEST(MGLASS_IMAGE, COPYCTOR_FROM_NONEMPTY)
{
    constexpr mglass::size_type width = 349;
    constexpr mglass::size_type height = 68;
    constexpr mglass::ARGB color{39, 84, 67, 59};

    mglass::Image from{width, height, color};
    mglass::Image to = from;

    ASSERT_EQ(from.getWidth(), width);
    ASSERT_EQ(from.getHeight(), height);

    ASSERT_EQ(to.getWidth(), from.getWidth());
    ASSERT_EQ(to.getHeight(), from.getHeight());

    for (mglass::size_type y = 0; y < height; ++y)
    {
        for (mglass::size_type x = 0; x < width; ++x)
        {
            ASSERT_EQ(from.getPixelAt(x, y), color);
            ASSERT_EQ(from.getPixelAt(x, y), to.getPixelAt(x, y));
        }
    }

    ASSERT_TRUE( (from == to) );
    ASSERT_FALSE( (from != to) );

    to.setPixelAt(19, 64, mglass::ARGB::black());

    ASSERT_FALSE( (from == to) );
    ASSERT_TRUE( (from != to) );
}


TEST(MGLASS_IMAGE, MOVECTOR_FROM_EMPTY)
{
    mglass::Image from;
    const mglass::Image to{std::move(from)};

    EXPECT_EQ(to.getWidth(), 0);
    EXPECT_EQ(to.getHeight(), 0);
}

TEST(MGLASS_IMAGE, MOVECTOR_FROM_NONEMPTY)
{
    constexpr mglass::size_type width = 97;
    constexpr mglass::size_type height = 234;
    constexpr mglass::ARGB color{98, 51, 236, 0};

    mglass::Image from{width, height, color};
    const mglass::Image to{std::move(from)};

    ASSERT_EQ(from.getWidth(), 0);  // NOLINT
    ASSERT_EQ(from.getHeight(), 0); // NOLINT

    ASSERT_EQ(to.getWidth(), width);
    ASSERT_EQ(to.getHeight(), height);

    for (mglass::size_type y = 0; y < height; ++y)
    {
        for (mglass::size_type x = 0; x < width; ++x)
        {
            ASSERT_EQ(to.getPixelAt(x, y), color);
        }
    }
}

// TODO: copy/move - assignments


// ====================================================================================================================
// setSize
// ====================================================================================================================

TEST(MGLASS_IMAGE, SETSIZE)
{
    mglass::Image img;

    img.setSize(0, 0);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);

    img.setSize(0, 1);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);

    img.setSize(1, 0);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);

    img.setSize(100, 0);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);

    img.setSize(0, 100);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);

    img.setSize(100, 200);
    EXPECT_EQ(img.getWidth(), 100);
    EXPECT_EQ(img.getHeight(), 200);

    img.setSize(250, 100);
    EXPECT_EQ(img.getWidth(), 250);
    EXPECT_EQ(img.getHeight(), 100);

    img.setSize(150, 150);
    EXPECT_EQ(img.getWidth(), 150);
    EXPECT_EQ(img.getHeight(), 150);

    img.setSize(150, 50);
    EXPECT_EQ(img.getWidth(), 150);
    EXPECT_EQ(img.getHeight(), 50);

    img.setSize(0, 50);
    EXPECT_EQ(img.getWidth(), 0);
    EXPECT_EQ(img.getHeight(), 0);
}


// ====================================================================================================================
// setPixelAt / getPixelAt
// ====================================================================================================================

TEST(MGLASS_IMAGE, SETPIXELAT_1_1)
{
    mglass::Image img{1, 1};

    constexpr mglass::ARGB color{ 73, 46, 87, 111 };

    img.setPixelAt(0, 0, color);
    ASSERT_EQ(img.getPixelAt(0, 0), color);
}

TEST(MGLASS_IMAGE, SETPIXELAT_SEQUENTIAL)
{
    mglass::Image img{512, 256};

    {
        mglass::ARGB color{0, 0, 0, 0};
        for (mglass::size_type y = 0; y < img.getHeight(); ++y)
        {
            for (mglass::size_type x = 0; x < img.getWidth(); ++x)
            {
                ++color.a;
                ++color.r;
                ++color.g;
                ++color.b;

                img.setPixelAt(x, y, color);
            }
        }
    }

    {
        mglass::ARGB color{0, 0, 0, 0};
        for (mglass::size_type y = 0; y < img.getHeight(); ++y)
        {
            for (mglass::size_type x = 0; x < img.getWidth(); ++x)
            {
                ++color.a;
                ++color.r;
                ++color.g;
                ++color.b;

                ASSERT_EQ(img.getPixelAt(x, y), color);
            }
        }
    }
}

TEST(MGLASS_IMAGE, SETPIXELAT_NONSEQUENTIAL)
{
    mglass::Image img{512, 256, mglass::ARGB::black()};

    constexpr std::pair<mglass::Point<mglass::size_type>, mglass::ARGB> points[10]{
        { {352, 17 }, {233, 207, 195, 144} },
        { {206, 167}, {237, 55,  65,  34 } },
        { {250, 121}, {188, 44,  217, 82 } },
        { {33,  116}, {107, 11,  89,  180} },
        { {264, 89 }, {45,  15,  42,  140} },
        { {35,  101}, {66,  40,  53,  92 } },
        { {273, 135}, {93,  172, 104, 24 } },
        { {93,  39 }, {194, 42,  91,  189} },
        { {0,   0  }, {247, 115, 154, 230} },
        { {158, 78 }, {125, 190, 248, 69 } },
    };

    for (const auto& [p, c]: points)
    {
        img.setPixelAt(p.x, p.y, c);
    }

    for (const auto& [p, c]: points)
    {
        ASSERT_EQ(img.getPixelAt(p.x, p.y), c);
    }
}


// ====================================================================================================================
// fill
// ====================================================================================================================

TEST(MGLASS_IMAGE, FILL_EMPTY)
{
    constexpr mglass::ARGB color{213, 59, 80, 9};

    mglass::Image img;

    img.fill(color);

    ASSERT_EQ(img.getWidth(), 0);
    ASSERT_EQ(img.getHeight(), 0);
}

TEST(MGLASS_IMAGE, FILL_1_1)
{
    constexpr mglass::ARGB color{213, 59, 80, 9};

    mglass::Image img{1, 1};

    img.fill(color);

    ASSERT_EQ(img.getWidth(), 1);
    ASSERT_EQ(img.getHeight(), 1);

    ASSERT_EQ(img.getPixelAt(0, 0), color);
}

TEST(MGLASS_IMAGE, FILL_250_519)
{
    constexpr mglass::ARGB color{59, 213, 9, 80};
    constexpr mglass::size_type width = 250;
    constexpr mglass::size_type height = 519;

    mglass::Image img{width, height};

    img.fill(color);

    ASSERT_EQ(img.getWidth(), width);
    ASSERT_EQ(img.getHeight(), height);

    for (mglass::size_type y = 0; y < height; ++y)
    {
        for (mglass::size_type x = 0; x < width; ++x)
        {
            ASSERT_EQ(img.getPixelAt(x, y), color);
        }
    }
}


// ====================================================================================================================
// fromPNGStream/saveToPNGStream
// ====================================================================================================================

TEST(MGLASS_IMAGE, STREAM_SAVE_PARSE_EMPTY)
{
    mglass::Image emptyImg;

    std::stringstream imgStream;
    emptyImg.saveToPNGStream(imgStream);

    ASSERT_THROW(mglass::Image::fromPNGStream(imgStream), std::runtime_error);
}

TEST(MGLASS_IMAGE, STREAM_SAVE_PARSE_MIN)
{
    constexpr mglass::ARGB color{ 198, 53, 72, 34 };
    mglass::Image srcImg{1, 1, color};

    std::stringstream imgStream;

    srcImg.saveToPNGStream(imgStream);
    const auto dstImg = mglass::Image::fromPNGStream(imgStream);

    ASSERT_EQ(srcImg, dstImg);

    ASSERT_EQ(dstImg.getWidth(), 1);
    ASSERT_EQ(dstImg.getHeight(), 1);
    ASSERT_EQ(dstImg.getPixelAt(0, 0), color);
}

TEST(MGLASS_IMAGE, STREAM_SAVE_PARSE_250_519)
{
    constexpr mglass::size_type width = 250;
    constexpr mglass::size_type height = 519;

    mglass::Image srcImg{width, height};

    {
        mglass::ARGB color{0, 0, 0, 0};
        for (mglass::size_type y = 0; y < height; ++y)
        {
            for (mglass::size_type x = 0; x < width; ++x)
            {
                ++color.a;
                ++color.r;
                ++color.g;
                ++color.b;

                srcImg.setPixelAt(x, y, color);
            }
        }
    }

    std::stringstream imgStream;

    srcImg.saveToPNGStream(imgStream);
    const auto dstImg = mglass::Image::fromPNGStream(imgStream);

    ASSERT_EQ(srcImg, dstImg);

    ASSERT_EQ(dstImg.getWidth(), width);
    ASSERT_EQ(dstImg.getHeight(), height);

    {
        mglass::ARGB color{0, 0, 0, 0};
        for (mglass::size_type y = 0; y < height; ++y)
        {
            for (mglass::size_type x = 0; x < width; ++x)
            {
                ++color.a;
                ++color.r;
                ++color.g;
                ++color.b;

                ASSERT_EQ(dstImg.getPixelAt(x, y), color);
            }
        }
    }
}


// ====================================================================================================================
// fromPNGFile/saveToPNGFile
// ====================================================================================================================

TEST(MGLASS_IMAGE, STREAM_PARSE_FILE_LENNA)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory

    const auto lenna = mglass::Image::fromPNGFile("resources/Lenna.png");

    mglass::size_type i = 0;
    for (mglass::size_type y = 0; y < 512; ++y)
    {
        for (mglass::size_type x = 0; x < 512; ++x)
        {
            ASSERT_EQ(lenna.getPixelAt(x, y), resources::lenna_512_512::data[i++]);
        }
    }
}

TEST(MGLASS_IMAGE, STREAM_PARSE_FILE_NOTEXISTING)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory
    ASSERT_THROW(mglass::Image::fromPNGFile("resources/notexists.png"), std::runtime_error);
}
