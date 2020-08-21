#ifndef MAGNIFYING_GLASS_PRIMITIVES_H
#define MAGNIFYING_GLASS_PRIMITIVES_H

#include <cstdint>      // std::int32_t
#include <cstddef>      // std::size_t
#include <type_traits>  // std::is_same_v
#include <algorithm>    // std::min, std::max

namespace mglass
{
    using size_type  = std::size_t;
    using int_type   = std::int32_t;
    using float_type = float;


    template<typename Numerical>
    struct Point
    {
        static_assert(std::is_arithmetic_v<Numerical>, "non arithmetic types are not allowed here");
        static_assert(!std::is_same_v<Numerical, bool>, "bool is useless here");

        Numerical x;
        Numerical y;
    };

    template<typename T>
    Point(T, T) -> Point<T>;


    template<typename To, typename From>
    Point<To> pointCast(const Point<From> point)
    {
        return { static_cast<To>(point.x), static_cast<To>(point.y) };
    }


    template<typename CoordinateT, typename SizeT = CoordinateT>
    struct RectArea
    {
        static_assert(std::is_arithmetic_v<CoordinateT>, "non arithmetic types are not allowed here");
        static_assert(!std::is_same_v<CoordinateT, bool>, "bool is useless here");

        static_assert(std::is_arithmetic_v<SizeT>, "non arithmetic types are not allowed here");
        static_assert(!std::is_same_v<SizeT, bool>, "bool is not allowed here");


        Point<CoordinateT> topLeft;
        SizeT width;
        SizeT height;


        // if this and `other` don't intersect result will have width == 0 and height == 0
        RectArea getIntersectionWith(const RectArea other) const noexcept
        {
            const CoordinateT resultLeftX = (std::max)(topLeft.x, other.topLeft.x);

            const CoordinateT thisRightX = topLeft.x + static_cast<CoordinateT>(width);
            const CoordinateT otherRightX = other.topLeft.x + static_cast<CoordinateT>(other.width);
            const CoordinateT resultRightX = (std::min)(thisRightX, otherRightX);

            if (resultLeftX >= resultRightX)
                return { {0, 0}, 0, 0 };

            const CoordinateT resultTopY = (std::min)(topLeft.y, other.topLeft.y);

            const CoordinateT thisBottomY = topLeft.y - static_cast<CoordinateT>(height);
            const CoordinateT otherBottomY = other.topLeft.y - static_cast<CoordinateT>(other.height);
            const CoordinateT resultBottomY = (std::max)(thisBottomY, otherBottomY);

            if (resultTopY <= resultBottomY)
                return { {0, 0}, 0, 0 };

            const auto resultWidth = static_cast<SizeT>(resultRightX - resultLeftX);
            const auto resultHeight = static_cast<SizeT>(resultTopY - resultBottomY);

            return { {resultLeftX, resultTopY}, resultWidth, resultHeight };
        }
    };

    template<typename Coordinate, typename Size>
    RectArea(Point<Coordinate>, Size, Size) -> RectArea<Coordinate, Size>;


    using IntegralRectArea = RectArea<mglass::int_type, mglass::size_type>;
}

#endif // ndef MAGNIFYING_GLASS_PRIMITIVES_H
