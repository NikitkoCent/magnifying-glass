#ifndef MAGNIFYING_GLASS_PRIMITIVES_H
#define MAGNIFYING_GLASS_PRIMITIVES_H

#include <cstdint>      // std::int32_t
#include <cstddef>      // std::size_t
#include <type_traits>  // std::is_same_v

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
    };

    template<typename Coordinate, typename Size>
    RectArea(Coordinate, Size, Size) -> RectArea<Coordinate, Size>;


    using IntegralRectArea = RectArea<mglass::int_type, mglass::size_type>;
}

#endif // ndef MAGNIFYING_GLASS_PRIMITIVES_H
