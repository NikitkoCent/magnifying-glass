#ifndef MAGNIFYING_GLASS_PRIMITIVES_H
#define MAGNIFYING_GLASS_PRIMITIVES_H

#include <cstdint>      // std::int32_t, std::uint32_t
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
        static_assert(!std::is_same_v<Numerical, bool>, "bool is not allowed here");

        Numerical x;
        Numerical y;
    };


    struct Rect
    {
        Point<mglass::int_type> topLeft;
        mglass::size_type width;
        mglass::size_type height;
    };
}

#endif // ndef MAGNIFYING_GLASS_PRIMITIVES_H
