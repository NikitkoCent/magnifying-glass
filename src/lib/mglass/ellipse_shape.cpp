#include "mglass/ellipse_shape.h"


namespace mglass::shapes
{
    Ellipse::Ellipse(
        mglass::Point<mglass::float_type> center,
        mglass::float_type xAxis,
        mglass::float_type yAxis) noexcept
        : center_(center)
        , xAxisLength_(xAxis)
        , yAxisLength_(yAxis)
    {}


    ShapeRectArea Ellipse::getBoundsImpl() const noexcept
    {
        const auto halfWidth = xAxisLength_ / 2;
        const auto halfHeight = yAxisLength_ / 2;
        const Point topLeft{ center_.x - halfWidth, center_.y + halfHeight };

        return { topLeft, xAxisLength_, yAxisLength_ };
    }


    Point<mglass::float_type> Ellipse::getPointAtScaledImpl(
        mglass::float_type scaleFactor,
        Point<mglass::float_type> point) const noexcept
    {
        const auto dx = point.x - center_.x;
        const auto dy = point.y - center_.y;

        return { center_.x + dx / scaleFactor, center_.y + dy / scaleFactor};
    }
} // namespace mglass::shapes
