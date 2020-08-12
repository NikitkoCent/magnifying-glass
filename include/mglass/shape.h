#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, Rect
#include <functional>           // std::function
#include <cmath>                // std::floor
#include <cassert>              // assert

namespace mglass
{
    // TODO: move to CRTP
    struct Shape
    {
        virtual Point<mglass::float_type> getCenter() const noexcept = 0;
        virtual mglass::float_type getWidth() const noexcept = 0;
        virtual mglass::float_type getHeight() const noexcept = 0;

        virtual Point<mglass::float_type> getPointAtScaled(mglass::float_type scaleFactor, Point<mglass::float_type> point) const = 0;

        virtual void rasterizeOnto(const Rect& rect, std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const = 0;

    protected:
        // dtor will not be invoked by the library
        ~Shape() noexcept = default;
    };


    inline Rect getShapeRect(const Shape& shape)
    {
        const auto widthFloat = std::ceil(shape.getWidth());
        const auto heightFloat = std::ceil(shape.getHeight());

        assert( ((widthFloat > 0) && (heightFloat > 0)) );

        auto leftTopFloat = shape.getCenter();
        leftTopFloat.x = std::floor(leftTopFloat.x - shape.getWidth() / 2);
        leftTopFloat.y = std::floor(leftTopFloat.y - shape.getHeight() / 2);

        const auto leftTop = pointCast<mglass::int_type>(leftTopFloat);

        return {leftTop, static_cast<mglass::size_type>(widthFloat), static_cast<mglass::size_type>(heightFloat)};
    }
}

#endif // ndef MAGNIFYING_GLASS_SHAPE_H
