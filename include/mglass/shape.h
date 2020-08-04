#ifndef MAGNIFYING_GLASS_SHAPE_H
#define MAGNIFYING_GLASS_SHAPE_H

#include "mglass/primitives.h"  // Point, Rect
#include <functional>           // std::function

namespace mglass
{
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
}

#endif // ndef MAGNIFYING_GLASS_SHAPE_H
