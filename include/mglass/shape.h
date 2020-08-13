#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, Rect
#include <functional>           // std::function
#include <cmath>                // std::floor

namespace mglass
{
    // TODO: move to CRTP
    struct Shape
    {
        virtual Point<mglass::float_type> getCenter() const noexcept = 0;
        virtual mglass::float_type getWidth() const noexcept = 0;
        virtual mglass::float_type getHeight() const noexcept = 0;

        // returns coordinates of `point` after resizing the shape by `scaleFactor` times
        // `scaleFactor` must be within the (0; +inf) range; undefined behavior otherwise
        // `point` must be inside the shape; undefined behavior otherwise (TODO: make the condition more friendly)
        virtual Point<mglass::float_type> getPointAtScaled(mglass::float_type scaleFactor, Point<mglass::float_type> point) const = 0;

        virtual void rasterizeOnto(const Rect& rect, std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const = 0;

    protected:
        // dtor will not be invoked by the library
        ~Shape() noexcept = default;
    };


    inline Rect getShapeRect(const Shape& shape)
    {
        const auto center = shape.getCenter();
        const auto halfWidth = shape.getWidth() / 2;
        const auto halfHeight = shape.getHeight() / 2;

        const auto leftTopX = static_cast<mglass::int_type>(std::floor(center.x - halfWidth));
        const auto rightBottomX = static_cast<mglass::int_type>(std::ceil(center.x + halfWidth));
        const mglass::size_type rectWidth = (rightBottomX < leftTopX) ? 0 : (rightBottomX - leftTopX + 1);

        const auto leftTopY = static_cast<mglass::int_type>(std::ceil(center.y + halfHeight));
        const auto rightBottomY = static_cast<mglass::int_type>(std::floor(center.y - halfHeight));
        const mglass::size_type rectHeight = (leftTopY < rightBottomY) ? 0 : (leftTopY - rightBottomY + 1);

        return { {leftTopX, leftTopY}, rectWidth, rectHeight};
    }
}

#endif // ndef MAGNIFYING_GLASS_SHAPE_H
