#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, Rect
#include <functional>           // std::function
#include <cmath>                // std::ceil, std::floor

namespace mglass
{
    using ShapeRectArea = RectArea<mglass::float_type>;


    // TODO: move to CRTP
    struct Shape
    {
        // returns a bounding box of the Shape
        virtual ShapeRectArea getBounds() const = 0;

        // returns coordinates of `point` after resizing the shape by `scaleFactor` times
        // `scaleFactor` must be within the (0; +inf) range; undefined behavior otherwise
        // `point` must be inside the shape; undefined behavior otherwise (TODO: make the condition more friendly)
        virtual Point<mglass::float_type> getPointAtScaled(mglass::float_type scaleFactor, Point<mglass::float_type> point) const = 0;

        virtual void rasterizeOnto(const IntegralRectArea& rect, std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const = 0;

    protected:
        // dtor will not be invoked by the library
        ~Shape() noexcept = default;
    };


    inline IntegralRectArea getShapeIntegralBounds(const Shape& shape)
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
