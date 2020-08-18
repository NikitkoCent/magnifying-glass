#ifndef MAGNIFYING_GLASS_SHAPES_H
#define MAGNIFYING_GLASS_SHAPES_H

#include "mglass/shape.h"

namespace mglass::shapes
{
    class Ellipse : public Shape
    {
    public: // ctors/dtor
        explicit Ellipse(
            mglass::Point<mglass::float_type> center = {0, 0},
            mglass::float_type xAxis = 0,
            mglass::float_type yAxis = 0) noexcept(false);

        ~Ellipse() noexcept = default;

    public: // getters
        ShapeRectArea getBounds() const override;

        Point<mglass::float_type> getPointAtScaled(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point) const override;

        void rasterizeOnto(
            const IntegralRectArea& rect,
            std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const override;

    protected:
        mglass::Point<mglass::float_type> center_;
        mglass::float_type xAxisLength_;
        mglass::float_type yAxisLength_;
    };


    class Rectangle : public Shape
    {

    };
}

#endif // ndef MAGNIFYING_GLASS_SHAPES_H
