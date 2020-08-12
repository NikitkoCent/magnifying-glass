#ifndef MAGNIFYING_GLASS_SHAPES_H
#define MAGNIFYING_GLASS_SHAPES_H

#include "mglass/shape.h"

namespace mglass::shapes
{
    class Ellipse final : public Shape
    {
    public: // ctors/dtor
        explicit Ellipse(
            mglass::Point<mglass::float_type> center = {0, 0},
            mglass::float_type xAxis = 0,
            mglass::float_type yAxis = 0) noexcept(false);

        ~Ellipse() noexcept = default;

    public: // modifiers
        Ellipse& moveTo(mglass::Point<mglass::float_type> newCenter) noexcept(false);
        Ellipse& moveBy(mglass::float_type dx, mglass::float_type dy) noexcept(false);

        Ellipse& setXAxis(mglass::float_type xAxis) noexcept(false);
        Ellipse& setYAxis(mglass::float_type yAxis) noexcept(false);

    public: // getters
        Point<mglass::float_type> getCenter() const noexcept override;

        mglass::float_type getWidth() const noexcept override;
        mglass::float_type getHeight() const noexcept override;

        Point<mglass::float_type> getPointAtScaled(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point) const override;

        void rasterizeOnto(
            const Rect& rect,
            std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const override;

    private:
        mglass::Point<mglass::float_type> center_;
        mglass::float_type xAxisLength_;
        mglass::float_type yAxisLength_;
    };


    class Rectangle : public Shape
    {

    };
}

#endif // ndef MAGNIFYING_GLASS_SHAPES_H
