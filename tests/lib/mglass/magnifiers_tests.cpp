#include "mglass/mglass.h"      // mglass::*
#include "mglass/magnifiers.h"  // mglass::magnifiers::*
#include "mglass/shapes.h"      // mglass::shapes::*
#include "gtest/gtest.h"


// ====================================================================================================================
// shapes::Ellipse
// ====================================================================================================================

TEST(MGLASS_NEAREST_NEIGHBOR, ELLIPSE_SCALE_2P5_SIZE_250_100_AT_IMG_CENTER)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory

    const auto lenna = mglass::Image::fromPNGFile("resources/Lenna.png");

    const mglass::IntegralRectArea imgArea{
        {0, 0},
        lenna.getWidth(),
        lenna.getHeight()
    };

    const mglass::shapes::Ellipse shape{
        imgArea.getCenter(),
        250,
        100
    };

    mglass::Image actualOutputImg;
    mglass::magnifiers::nearestNeighbor(shape, 2.5, lenna, imgArea.topLeft, actualOutputImg);

    const auto expectedOutputImg = mglass::Image::fromPNGFile("resources/lenna_ellipse_2.5_250_100_at_center.png");

    ASSERT_EQ(actualOutputImg, expectedOutputImg);
}

TEST(MGLASS_NEAREST_NEIGHBOR, ELLIPSE_SCALE_4_SIZE_189_300_AT_499_M275)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory

    const auto lenna = mglass::Image::fromPNGFile("resources/Lenna.png");

    const mglass::IntegralRectArea imgArea{
        {0, 0},
        lenna.getWidth(),
        lenna.getHeight()
    };

    const mglass::shapes::Ellipse shape{
        {499, -275},
        189,
        300
    };

    mglass::Image actualOutputImg;
    mglass::magnifiers::nearestNeighbor(shape, 4, lenna, imgArea.topLeft, actualOutputImg);

    const auto expectedOutputImg = mglass::Image::fromPNGFile("resources/lenna_ellipse_4_189_300_at_499_-275.png");

    ASSERT_EQ(actualOutputImg, expectedOutputImg);
}


// ====================================================================================================================
// shapes::Rectangle
// ====================================================================================================================

TEST(MGLASS_NEAREST_NEIGHBOR, RECTANGLE_SCALE_2P5_SIZE_250_100_AT_IMG_CENTER)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory

    const auto lenna = mglass::Image::fromPNGFile("resources/Lenna.png");

    const mglass::IntegralRectArea imgArea{
        {0, 0},
        lenna.getWidth(),
        lenna.getHeight()
    };

    const mglass::shapes::Rectangle shape{
        imgArea.getCenter(),
        250,
        100
    };

    mglass::Image actualOutputImg;
    mglass::magnifiers::nearestNeighbor(shape, 2.5, lenna, imgArea.topLeft, actualOutputImg);

    const auto expectedOutputImg = mglass::Image::fromPNGFile("resources/lenna_rectangle_2.5_250_100_at_center.png");

    ASSERT_EQ(actualOutputImg, expectedOutputImg);
}

TEST(MGLASS_NEAREST_NEIGHBOR, RECTANGLE_SCALE_4_SIZE_189_300_AT_499_M275)
{
    // please make sure you are running this tests at "magnifying-glass/tests" working directory

    const auto lenna = mglass::Image::fromPNGFile("resources/Lenna.png");

    const mglass::IntegralRectArea imgArea{
        {0, 0},
        lenna.getWidth(),
        lenna.getHeight()
    };

    const mglass::shapes::Rectangle shape{
        {499, -275},
        189,
        300
    };

    mglass::Image actualOutputImg;
    mglass::magnifiers::nearestNeighbor(shape, 4, lenna, imgArea.topLeft, actualOutputImg);

    const auto expectedOutputImg = mglass::Image::fromPNGFile("resources/lenna_rectangle_4_189_300_at_499_-275.png");

    ASSERT_EQ(actualOutputImg, expectedOutputImg);
}

