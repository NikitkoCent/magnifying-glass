#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, RectArea, IntegralRectArea, int_type, size_type, float_type
#include <cmath>                // std::ceil, std::floor
#include <type_traits>          // std::is_invocable_v
#include <utility>              // std::forward


namespace mglass
{
    using ShapeRectArea = RectArea<float_type>;


    // The RasterizationContextBase interface provides definitions for information
    //  about point rasterized onto some pixel (it is used at Shape::rasterizeOnto)
    //
    // Probably there is no need for CRTP, it is used to explicit define the interface.
    // It should be replaced by C++20 concepts, but we don't have compilers support C++20 well yet.
    // Another way is to make some traits like is_shape<T> but it is less-readable.
    template<typename Derived>
    struct RasterizationContextBase
    {
        // Returns coordinates of the pixel onto which the point was rasterized.
        [[nodiscard]] Point<int_type> getRasterizedPoint() const noexcept
        {
            return static_cast<const Derived*>(this)->getRasterizedPointImpl();
        }

        // Returns density of the pixel retrieved by getRasterizedPoint() method.
        // Returned value must be inside the range [0; 1].
        // Pixels closer to the edges of the shape should have less value of the density.
        [[nodiscard]] float_type getPixelDensity() const noexcept
        {
            return static_cast<const Derived*>(this)->getPixelDensityImpl();
        }

    protected: // crtp methods implementation
        [[noreturn]] Point<int_type> getRasterizedPointImpl() const noexcept
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
        }

        [[noreturn]] float_type getPixelDensityImpl() const noexcept
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
        }

    protected:
        // dtor will not be invoked by the library
        ~RasterizationContextBase() noexcept = default;
    };


    namespace detail
    {
        template<typename T>
        constexpr bool rasterizationContextIsDerivedFromBase(RasterizationContextBase<T>*)
        {
            return true;
        }

        constexpr bool rasterizationContextIsDerivedFromBase(...)
        {
            return false;
        }
    } // namespace detail


    // The Shape interface provides definitions for objects that represent some form of magnifying glass
    //  (e.g. rectangle or ellipse magnifying glass).
    //
    // As in the case of RasterizationContextBase there is no real need for CRTP.
    // It should be replaced by C++20 concepts or traits.
    template<typename Derived, typename RasterizationContextT>
    struct Shape
    {
        using RasterizationContext = RasterizationContextT;
        static_assert(
            detail::rasterizationContextIsDerivedFromBase( static_cast<RasterizationContext*>(nullptr) ),
            "RasterizationContext must be derived from mglass::RasterizationContextBase");


        // returns a bounding box of the Shape
        [[nodiscard]] ShapeRectArea getBounds() const
        {
            return static_cast<const Derived*>(this)->getBoundsImpl();
        }

        // rasterizes this shape onto the area described by `rect`
        // `consumer` will be invoked on each rasterized point.
        //  It must be invocable by object of RasterizationContext type.
        // RasterizationContext::getRasterizedPoint will return coordinates of the pixel onto which a shape's point was rasterized.
        // Returned values of the `consumer` will be ignored.
        template<typename ConsumerFunctor>
        void rasterizeOnto(const IntegralRectArea& rect, ConsumerFunctor&& consumer) const
        {
            static_assert(
                std::is_invocable_v<ConsumerFunctor, RasterizationContext>,
                "ConsumerFunctor must be invocable with the arguments (RasterizationContext)"
            );

            static_cast<const Derived*>(this)->rasterizeOntoImpl(rect, std::forward<ConsumerFunctor>(consumer));
        }

    protected: // ctors/dtor
        constexpr Shape() noexcept = default;

        // dtor will not be invoked by the library
        ~Shape() noexcept = default;

    protected: // crtp methods implementation
        [[noreturn]] ShapeRectArea getBoundsImpl() const
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
        }

        template<typename ConsumerFunctor>
        [[noreturn]] void rasterizeOntoImpl(
            [[maybe_unused]] IntegralRectArea rect,
            [[maybe_unused]] ConsumerFunctor&& consumer) const
        {
            static_assert(detail::dependent_false_v<Derived>, "is not implemented");
        }
    };


    template<typename Impl, typename Ctx>
    IntegralRectArea getShapeIntegralBounds(const Shape<Impl, Ctx>& shape)
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
