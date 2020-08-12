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


    Ellipse& Ellipse::moveTo(mglass::Point<mglass::float_type> newCenter) noexcept(false)
    {
        center_ = newCenter;

        return *this;
    }

    Ellipse& Ellipse::moveBy(mglass::float_type dx, mglass::float_type dy) noexcept(false)
    {
        center_.x += dx;
        center_.y += dy;

        return *this;
    }


    Ellipse& Ellipse::setXAxis(mglass::float_type xAxis) noexcept(false)
    {
        // TODO: check float correctness and >=0
        xAxisLength_ = xAxis;

        return *this;
    }

    Ellipse& Ellipse::setYAxis(mglass::float_type yAxis) noexcept(false)
    {
        // TODO: check float correctness and >=0
        yAxisLength_ = yAxis;

        return *this;
    }


    Point<mglass::float_type> Ellipse::getCenter() const noexcept
    {
        return center_;
    }

    mglass::float_type Ellipse::getWidth() const noexcept
    {
        return xAxisLength_;
    }

    mglass::float_type Ellipse::getHeight() const noexcept
    {
        return yAxisLength_;
    }


    Point<mglass::float_type> Ellipse::getPointAtScaled(
        mglass::float_type scaleFactor,
        Point<mglass::float_type> point) const
    {
        const auto dx = center_.x - point.x;
        const auto dy = center_.y - point.y;

        return { center_.x + dx / scaleFactor, center_.y + dy / scaleFactor};
    }


    void Ellipse::rasterizeOnto(
        const Rect& rect,
        std::function<void(Point<mglass::int_type>, mglass::float_type)> consumer) const
    {
        // ellipse equation:
        // x^2 / a^2 + y^2 / b^2 = 1
        // avoid division ops (for performance reasons): b^2 * x^2 + a^2 * y^2 = a^2 * b^2

        if ((rect.width < 1) || (rect.height < 1))
            return;

        const Rect shapeRect = getShapeRect(*this);
        if ((shapeRect.width < 1) || (shapeRect.height < 1))
            return;

        const auto rectXMin = rect.topLeft.x;
        const auto rectXMax = rect.topLeft.x + static_cast<mglass::int_type>(rect.width) - 1;

        const auto rectYMin = rect.topLeft.y - static_cast<mglass::int_type>(rect.height) + 1;
        const auto rectYMax = rect.topLeft.y;

        // TODO: optimize the algorithm:
        //  1. find for every y the exact range [xMin, xMax] for which every point (x, y) is inside the ellipse

        // a^2 == (getWidth() / 2) ^ 2
        const auto a2 = (getWidth() * getWidth()) / 4;
        // b^2 == (getHeight() / 2) ^ 2
        const auto b2 = (getHeight() * getHeight()) / 4;

        const auto xStart = shapeRect.topLeft.x;
        const auto xEnd = shapeRect.topLeft.x + static_cast<mglass::int_type>(shapeRect.width);

        const auto yStart = shapeRect.topLeft.y;
        const auto yEnd = shapeRect.topLeft.y - static_cast<mglass::int_type>(shapeRect.height);

        for (mglass::int_type y = yStart; y > yEnd; --y)
        {
            if ((y < rectYMin) || (y > rectYMax))
                continue;

            // b^2 * x^2 + a^2 * y^2 <= a^2 * b^2
            // equals to
            // b^2 * x^2 + a^2(y^2 - b^2) <= 0
            // equals to
            // b^2 * x^2 <= -a^2(y^2 - b^2)
            // equals to
            // b^2 * x^2 <= a^2(b^2 - y^2)

            const auto y2 = static_cast<mglass::float_type>(y * y);
            const auto rightPart = a2 * (b2 - y2);

            for (mglass::int_type x = xStart; x < xEnd; ++x)
            {
                if ((x < rectXMin) || (x > rectXMax))
                    continue;

                const auto b2x2 = b2 * static_cast<mglass::float_type>(x * x);
                if (b2x2 <= rightPart)
                {
                    // TODO: compute the second parameter of the consumer
                    consumer({x, y}, 1);
                }
            }
        }
    }
} // namespace mglass::shapes
