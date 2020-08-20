#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, Rect
#include <functional>           // std::function
#include <cmath>                // std::ceil, std::floor
#include <type_traits>          // std::is_invocable_v
#include <utility>              // std::forward


namespace mglass::detail
{
    template<typename>
    struct dependent_false
    {
        static constexpr bool value = false;
    };
}

namespace mglass
{
    using ShapeRectArea = RectArea<mglass::float_type>;


    template<typename Derived>
    struct Shape
    {
        // returns a bounding box of the Shape
        ShapeRectArea getBounds() const
        {
            return static_cast<const Derived*>(this)->getBoundsImpl();
        }

        // returns coordinates of `point` after resizing the shape by `scaleFactor` times
        // `scaleFactor` must be within the (0; +inf) range; undefined behavior otherwise
        // `point` must be inside the shape; undefined behavior otherwise (TODO: make the condition more friendly)
        Point<mglass::float_type> getPointAtScaled(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point
        ) const
        {
            return static_cast<const Derived*>(this)->getPointAtScaledImpl(scaleFactor, point);
        }

        // TODO: add docs
        template<typename ConsumerFunctor>
        void rasterizeOnto(const IntegralRectArea& rect, ConsumerFunctor&& consumer) const
        {
            static_assert(std::is_invocable_v<ConsumerFunctor, Point<mglass::int_type>, mglass::float_type>,
                "ConsumerFunctor must be invocable with the arguments (Point<mglass::int_type>, mglass::float_type)");

            static_cast<const Derived*>(this)->rasterizeOntoImpl(rect, std::forward<ConsumerFunctor>(consumer));
        }

    protected: // crtp methods implementation
        ShapeRectArea getBoundsImpl() const
        {
            static_assert(detail::dependent_false<Derived>::value, "is not implemented");
        }

        Point<mglass::float_type> getPointAtScaledImpl(
            [[maybe_unused]] mglass::float_type scaleFactor,
            [[maybe_unused]] Point<mglass::float_type> point) const
        {
            static_assert(detail::dependent_false<Derived>::value, "is not implemented");
        }

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(
            [[maybe_unused]] IntegralRectArea rect,
            [[maybe_unused]] ConsumerFunctor&& consumer) const
        {
            static_assert(detail::dependent_false<Derived>::value, "is not implemented");
        }

    protected:
        // dtor will not be invoked by the library
        ~Shape() noexcept = default;
    };


    template<typename Impl>
    inline IntegralRectArea getShapeIntegralBounds(const Shape<Impl>& shape)
    {
        const auto preciseBounds = shape.getBounds();

        const auto bottomRightX = static_cast<mglass::int_type>(std::ceil(preciseBounds.topLeft.x + preciseBounds.width)) - 1;
        const auto bottomRightY = static_cast<mglass::int_type>(std::floor(preciseBounds.topLeft.y - preciseBounds.height)) + 1;

        IntegralRectArea result;

        result.topLeft.x = static_cast<mglass::int_type>(std::floor(preciseBounds.topLeft.x));
        result.topLeft.y = static_cast<mglass::int_type>(std::ceil(preciseBounds.topLeft.y));
        result.width = static_cast<mglass::size_type>(bottomRightX - result.topLeft.x + 1);
        result.height = static_cast<mglass::size_type>(result.topLeft.y - bottomRightY + 1);

        return result;
    }
}

#endif // ndef MAGNIFYING_GLASS_SHAPE_H
