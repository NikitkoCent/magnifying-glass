#ifndef MAGNIFYING_GLASS_PRIMITIVES_H
#define MAGNIFYING_GLASS_PRIMITIVES_H

#include <cstdint>      // std::int32_t
#include <cstddef>      // std::size_t
#include <type_traits>  // std::is_arithmetic_v, std::is_same_v

namespace mglass
{
    namespace detail
    {
        template<typename...>
        inline constexpr bool dependent_false_v = false;
    } // namespace detail


    using size_type  = std::size_t;
    using int_type   = std::int32_t;
    using float_type = float;


    namespace literals
    {
        constexpr size_type operator""_szt(unsigned long long v) { return v; }
        constexpr int_type operator""_intt(unsigned long long v) { return v; }
        constexpr float_type operator""_flt(long double v) { return v; }
        constexpr float_type operator""_flt(unsigned long long v) { return v; }
    }


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


        [[nodiscard]] Point<CoordinateT> getBottomRight() const noexcept
        {
            if constexpr (std::is_integral_v<CoordinateT> && std::is_integral_v<SizeT>)
            {
                Point<CoordinateT> result = topLeft;

                if ((width < 1) || (height < 1))
                    return result;

                result.x += static_cast<CoordinateT>(width - 1);
                result.y -= static_cast<CoordinateT>(height - 1);

                return result;
            }
            else
            {
                static_assert(
                    detail::dependent_false_v<CoordinateT, SizeT>,
                    "right bottom point of an area can be computed only for integral types");
            }
        }

        [[nodiscard]] Point<float_type> getCenter() const noexcept
        {
            if constexpr (std::is_integral_v<CoordinateT> && std::is_integral_v<SizeT>)
            {
                auto result = pointCast<float_type>(topLeft);

                if ((width < 1) || (height < 1))
                    return result;

                const auto halfWidth = static_cast<float_type>(width - 1) / 2;
                const auto halfHeight = static_cast<float_type>(height - 1) / 2;

                result.x += halfWidth;
                result.y -= halfHeight;

                return result;
            }
            else
            {
                static_assert(
                    detail::dependent_false_v<CoordinateT, SizeT>,
                    "center point of an area can be computed only for integral types");
            }
        }
    };

    template<typename CoordinateT, typename SizeT>
    RectArea(Point<CoordinateT>, SizeT, SizeT) -> RectArea<CoordinateT, SizeT>;


    using IntegralRectArea = RectArea<int_type, size_type>;
} // namespace mglass

#endif // ndef MAGNIFYING_GLASS_PRIMITIVES_H
