#include "mglass/shapes.h"

namespace mglass::shapes
{
    Ellipse::Ellipse(
        mglass::Point<mglass::float_type> center,
        mglass::float_type xAxis,
        mglass::float_type yAxis) noexcept(false)
        : center_(center)
        , xAxisLength_(xAxis)
        , yAxisLength_(yAxis)
    {}


    ShapeRectArea Ellipse::getBounds() const
    {
        const auto halfWidth = xAxisLength_ / 2;
        const auto halfHeight = yAxisLength_ / 2;
        const Point topLeft{ center_.x - halfWidth, center_.y + halfHeight };

        return { topLeft, xAxisLength_, yAxisLength_ };
    }


    Point<mglass::float_type> Ellipse::getPointAtScaled(
        mglass::float_type scaleFactor,
        Point<mglass::float_type> point) const
    {
        const auto dx = point.x - center_.x;
        const auto dy = point.y - center_.y;

        return { center_.x + dx / scaleFactor, center_.y + dy / scaleFactor};
    }


    void Ellipse::rasterizeOnto(
        const IntegralRectArea& rect,
        std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const
    {
        // ellipse equation:
        // x^2 / a^2 + y^2 / b^2 = 1
        // avoid division ops (for performance reasons): b^2 * x^2 + a^2 * y^2 = a^2 * b^2

        if ((rect.width < 1) || (rect.height < 1))
            return;

        const IntegralRectArea thisIntegralBounds = getShapeIntegralBounds(*this);
        if ((thisIntegralBounds.width < 1) || (thisIntegralBounds.height < 1))
            return;

        const auto rectXMin = rect.topLeft.x;
        const auto rectXMax = rect.topLeft.x + static_cast<mglass::int_type>(rect.width) - 1;

        const auto rectYMin = rect.topLeft.y - static_cast<mglass::int_type>(rect.height) + 1;
        const auto rectYMax = rect.topLeft.y;

        // TODO: optimize the algorithm:
        //  1. find for every y the exact range [xMin, xMax] for which every point (x, y) is inside the ellipse

        // a^2 == (xAxisLength_ / 2) ^ 2
        const auto a2 = (xAxisLength_ * xAxisLength_) / 4;
        // b^2 == (yAxisLength_ / 2) ^ 2
        const auto b2 = (yAxisLength_ * yAxisLength_) / 4;

        const auto xStart = thisIntegralBounds.topLeft.x;
        const auto xEnd = thisIntegralBounds.topLeft.x + static_cast<mglass::int_type>(thisIntegralBounds.width);

        const auto yStart = thisIntegralBounds.topLeft.y;
        const auto yEnd = thisIntegralBounds.topLeft.y - static_cast<mglass::int_type>(thisIntegralBounds.height);

        for (mglass::int_type yUnaligned = yStart; yUnaligned > yEnd; --yUnaligned)
        {
            if ((yUnaligned < rectYMin) || (yUnaligned > rectYMax))
                continue;

            // b^2 * x^2 + a^2 * y^2 <= a^2 * b^2
            // equals to
            // b^2 * x^2 + a^2(y^2 - b^2) <= 0
            // equals to
            // b^2 * x^2 <= -a^2(y^2 - b^2)
            // equals to
            // b^2 * x^2 <= a^2(b^2 - y^2)

            const auto y = static_cast<mglass::float_type>(yUnaligned) - center_.y;
            const auto y2 = static_cast<mglass::float_type>(y * y);

            const auto rightPart = a2 * (b2 - y2);

            for (mglass::int_type xUnaligned = xStart; xUnaligned < xEnd; ++xUnaligned)
            {
                if ((xUnaligned < rectXMin) || (xUnaligned > rectXMax))
                    continue;

                const auto x = static_cast<mglass::float_type>(xUnaligned) - center_.x;

                const auto b2x2 = b2 * static_cast<mglass::float_type>(x * x);

                if (b2x2 <= rightPart)
                {
                    // TODO: compute the second parameter of the consumer
                    consumer({xUnaligned, yUnaligned}, 1);
                }
            }
        }
    }
} // namespace mglass::shapes
