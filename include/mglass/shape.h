#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, RectArea, IntegralRectArea, int_type, size_type, float_type
#include <cmath>                // std::ceil, std::floor
#include <type_traits>          // std::is_invocable_v
#include <utility>              // std::forward


namespace mglass
{
    using ShapeRectArea = RectArea<float_type>;


    // The Shape interface provides definitions for objects that represent some form of magnifying glass
    //  (e.g. rectangle or ellipse magnifying glass).
    //
    // Probably there is no need for CRTP. It should be replaced by C++20 concepts
    //  but we don't have compilers support C++20 well yet.
    // Another way is to make some traits like is_shape<T> but it is less-readable.
    template<typename Derived>
    struct Shape
    {
        // returns a bounding box of the Shape
        [[nodiscard]] ShapeRectArea getBounds() const
        {
            return static_cast<const Derived*>(this)->getBoundsImpl();
        }

        // TODO: add docs
        template<typename ConsumerFunctor>
        void rasterizeOnto(const IntegralRectArea& rect, ConsumerFunctor&& consumer) const
        {
            static_assert(
                std::is_invocable_v<ConsumerFunctor, Point<int_type>, float_type>,
                "ConsumerFunctor must be invocable with the arguments (Point<int_type>, float_type)"
            );

            static_cast<const Derived*>(this)->rasterizeOntoImpl(rect, std::forward<ConsumerFunctor>(consumer));
        }

    protected: // ctors/dtor
        constexpr Shape() noexcept = default;

        // dtor will not be invoked by the library
        ~Shape() noexcept = default;

    protected: // crtp methods implementation
        [[nodiscard]] ShapeRectArea getBoundsImpl() const
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
            return {};
        }

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(
            [[maybe_unused]] IntegralRectArea rect,
            [[maybe_unused]] ConsumerFunctor&& consumer) const
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
        }
    };


    template<typename Impl>
    IntegralRectArea getShapeIntegralBounds(const Shape<Impl>& shape)
    {
        const auto preciseBounds = shape.getBounds();

        const auto bottomRightX = static_cast<int_type>(std::ceil(preciseBounds.topLeft.x + preciseBounds.width)) - 1;
        const auto bottomRightY = static_cast<int_type>(std::floor(preciseBounds.topLeft.y - preciseBounds.height)) + 1;

        IntegralRectArea result;

        result.topLeft.x = static_cast<int_type>(std::floor(preciseBounds.topLeft.x));
        result.topLeft.y = static_cast<int_type>(std::ceil(preciseBounds.topLeft.y));
        result.width = static_cast<size_type>(bottomRightX - result.topLeft.x + 1);
        result.height = static_cast<size_type>(result.topLeft.y - bottomRightY + 1);

        return result;
    }
} // namespace mglass

#endif // ndef MAGNIFYING_GLASS_SHAPE_H
