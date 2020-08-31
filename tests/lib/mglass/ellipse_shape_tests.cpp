#include "mglass/shapes.h"  // mglass::shapes::Ellipse
#include "gtest/gtest.h"
#include "gmock/gmock.h"    // testing::*
#include <cstddef>          // std::size_t
#include <unordered_set>    // std::unordered_set


namespace
{
    struct PointHash
    {
        template<typename T>
        std::size_t operator()(const mglass::Point<T> p) const noexcept
        {
            return (std::hash<T>{}(p.x) ^ std::hash<T>{}(p.y));
        }
    };

    using IntPoint = mglass::Point<mglass::int_type>;
    using IntPointSet = std::unordered_set<IntPoint, PointHash>;
} // namespace


// FYI: EXPECT_EQ, EXPECT_NE are not used because of performing negative tests for operator==, operator!=.


// ====================================================================================================================
// ctors + getBounds()
// ====================================================================================================================

TEST(MGLASS_ELLIPSE_SHAPE, CTOR_DEFAULT)
{
    const mglass::shapes::Ellipse e;

    constexpr mglass::ShapeRectArea expectBounds{
        {0, 0},
        0,
        0
    };

    EXPECT_TRUE( (e.getBounds() == expectBounds) );
    EXPECT_FALSE( (e.getBounds() != expectBounds) );
}

TEST(MGLASS_ELLIPSE_SHAPE, CTOR_AT_05_m05)
{
    const mglass::shapes::Ellipse e{ {0.5, -0.5} };

    constexpr mglass::ShapeRectArea expectBounds{
        {0.5, -0.5},
        0,
        0
    };

    constexpr mglass::ShapeRectArea unexpectedBounds{
        {-0.5, 0.5},
        0,
        0
    };

    EXPECT_TRUE( (e.getBounds() == expectBounds) );
    EXPECT_FALSE( (e.getBounds() == unexpectedBounds) );

    EXPECT_FALSE( (e.getBounds() != expectBounds) );
    EXPECT_TRUE( (e.getBounds() != unexpectedBounds) );
}

TEST(MGLASS_ELLIPSE_SHAPE, CTOR_100_0_AT_05_m05)
{
    const mglass::shapes::Ellipse e{ {0.5, -0.5}, 100, 0 };

    constexpr mglass::ShapeRectArea expectBounds{
        {-49.5, -0.5},
        100,
        0
    };

    constexpr mglass::ShapeRectArea unexpectedBounds{
        {0.5, -0.5},
        0,
        0
    };

    EXPECT_TRUE( (e.getBounds() == expectBounds) );
    EXPECT_FALSE( (e.getBounds() == unexpectedBounds) );

    EXPECT_FALSE( (e.getBounds() != expectBounds) );
    EXPECT_TRUE( (e.getBounds() != unexpectedBounds) );
}

TEST(MGLASS_ELLIPSE_SHAPE, CTOR_12_213_AT_09_m14)
{
    const mglass::shapes::Ellipse e{ {0.9, -14}, 12, 213 };

    constexpr mglass::ShapeRectArea expectBounds{
        {-5.1, 92.5},
        12,
        213
    };

    constexpr mglass::ShapeRectArea unexpectedBounds{
        {-6, 93},
        12,
        213
    };

    EXPECT_TRUE( (e.getBounds() == expectBounds) );
    EXPECT_FALSE( (e.getBounds() == unexpectedBounds) );

    EXPECT_FALSE( (e.getBounds() != expectBounds) );
    EXPECT_TRUE( (e.getBounds() != unexpectedBounds) );
}


// ====================================================================================================================
// rasterizeOnto
// ====================================================================================================================

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_EMPTY)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 0, 0};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {0, 0},
        100,
        100
    };

    bool gotCalled = false;
    e.rasterizeOnto(rasterizeOntoArea, [&gotCalled](const mglass::shapes::Ellipse::RasterizationContext&) {
        gotCalled = true;
    });

    ASSERT_FALSE(gotCalled) << "Empty ellipse should rasterize no points.";
}

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_NONEMPTY_ONTO_EMPTY_AREA)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 300, 300};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {0, 0},
        0,
        0
    };

    bool gotCalled = false;
    e.rasterizeOnto(rasterizeOntoArea, [&gotCalled](const mglass::shapes::Ellipse::RasterizationContext&) {
        gotCalled = true;
    });

    ASSERT_FALSE(gotCalled) << "Empty area does not contain pixels onto which a shape can rasterize its points.";
}

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_10_5_ONTO_100_100)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 10, 5};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {-50, 50},
        100,
        100
    };

    //   minus plus
    //   5432101234
    //  ------------
    // 2|          |
    // 1| ******** |
    // 0|**********|
    //-1|**********|
    //-2| ******** |
    //
    static constexpr IntPoint expectedPoints[] {
        {-4, -2},
        {-3, -2},
        {-2, -2},
        {-1, -2},
        { 0, -2},
        { 1, -2},
        { 2, -2},
        { 3, -2},

        {-5, -1},
        {-4, -1},
        {-3, -1},
        {-2, -1},
        {-1, -1},
        { 0, -1},
        { 1, -1},
        { 2, -1},
        { 3, -1},
        { 4, -1},

        {-5,  0},
        {-4,  0},
        {-3,  0},
        {-2,  0},
        {-1,  0},
        { 0,  0},
        { 1,  0},
        { 2,  0},
        { 3,  0},
        { 4,  0},

        {-4,  1},
        {-3,  1},
        {-2,  1},
        {-1,  1},
        { 0,  1},
        { 1,  1},
        { 2,  1},
        { 3,  1},
    };

    IntPointSet actualPoints;

    e.rasterizeOnto(
        rasterizeOntoArea,
        [&actualPoints](const mglass::shapes::Ellipse::RasterizationContext& rstCtx) {
            actualPoints.emplace(rstCtx.getRasterizedPoint());
        }
    );

    ASSERT_THAT(actualPoints, testing::UnorderedElementsAreArray(expectedPoints));
}

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_10_6_ONTO_100_100)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 10, 6};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {-50, 50},
        100,
        100
    };

    //   minus plus
    //   5432101234
    //  ------------
    // 2|  ******  |
    // 1| ******** |
    // 0|**********|
    //-1|**********|
    //-2| ******** |
    //-3|  ******  |
    //
    static constexpr IntPoint expectedPoints[] {
        {-3, -3},
        {-2, -3},
        {-1, -3},
        { 0, -3},
        { 1, -3},
        { 2, -3},

        {-4, -2},
        {-3, -2},
        {-2, -2},
        {-1, -2},
        { 0, -2},
        { 1, -2},
        { 2, -2},
        { 3, -2},

        {-5, -1},
        {-4, -1},
        {-3, -1},
        {-2, -1},
        {-1, -1},
        { 0, -1},
        { 1, -1},
        { 2, -1},
        { 3, -1},
        { 4, -1},

        {-5,  0},
        {-4,  0},
        {-3,  0},
        {-2,  0},
        {-1,  0},
        { 0,  0},
        { 1,  0},
        { 2,  0},
        { 3,  0},
        { 4,  0},

        {-4,  1},
        {-3,  1},
        {-2,  1},
        {-1,  1},
        { 0,  1},
        { 1,  1},
        { 2,  1},
        { 3,  1},

        {-3, 2},
        {-2, 2},
        {-1, 2},
        { 0, 2},
        { 1, 2},
        { 2, 2},
    };

    IntPointSet actualPoints;

    e.rasterizeOnto(
        rasterizeOntoArea,
        [&actualPoints](const mglass::shapes::Ellipse::RasterizationContext& rstCtx) {
            actualPoints.emplace(rstCtx.getRasterizedPoint());
        }
     );

    ASSERT_THAT(actualPoints, testing::UnorderedElementsAreArray(expectedPoints));
}

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_10_5_ONTO_100_100_PARTIAL)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 10, 5};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {0, 98},
        100,
        100
    };

    //   minus plus
    //   5432101234
    //  ------------
    // 2|          |
    // 1|     **** |
    // 0|     *****|
    //-1|     *****|
    //-2|          |
    //
    static constexpr IntPoint expectedPoints[] {
        { 0, -1},
        { 1, -1},
        { 2, -1},
        { 3, -1},
        { 4, -1},

        { 0,  0},
        { 1,  0},
        { 2,  0},
        { 3,  0},
        { 4,  0},

        { 0,  1},
        { 1,  1},
        { 2,  1},
        { 3,  1},
    };

    IntPointSet actualPoints;

    e.rasterizeOnto(rasterizeOntoArea, [&actualPoints](const mglass::shapes::Ellipse::RasterizationContext& rstCtx) {
        actualPoints.emplace(rstCtx.getRasterizedPoint());
    });

    ASSERT_THAT(actualPoints, testing::UnorderedElementsAreArray(expectedPoints));
}

TEST(MGLASS_ELLIPSE_SHAPE, RASTERIZE_10_5_ONTO_100_100_NO_INTERSECTIONS)
{
    const mglass::shapes::Ellipse e{ {0, 0}, 10, 5};

    const mglass::IntegralRectArea rasterizeOntoArea{
        {5, 0},
        100,
        100
    };

    //   minus plus
    //   5432101234
    //  ------------
    // 2|          |
    // 1|          |
    // 0|          |
    //-1|          |
    //-2|          |
    //

    IntPointSet actualPoints;

    e.rasterizeOnto(rasterizeOntoArea, [&actualPoints](const mglass::shapes::Ellipse::RasterizationContext& rstCtx) {
        actualPoints.emplace(rstCtx.getRasterizedPoint());
    });

    ASSERT_TRUE(actualPoints.empty());
}
