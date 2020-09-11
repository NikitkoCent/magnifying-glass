#include "mglass/ellipse_shape.h"


namespace mglass::shapes
{
    Ellipse::Ellipse(Point<float_type> center, float_type xAxis, float_type yAxis) noexcept
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

} // namespace mglass::shapes
