#ifndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
#define MAGNIFYING_GLASS_ELLIPSE_SHAPE_H

#include "mglass/shape.h"
#include <utility>          // std::forward


namespace mglass::shapes
{
    class Ellipse : public Shape<Ellipse>
    {
        friend struct Shape<Ellipse>;

    public: // ctors/dtor
        explicit Ellipse(
            mglass::Point<mglass::float_type> center = {0, 0},
            mglass::float_type xAxis = 0,
            mglass::float_type yAxis = 0) noexcept;

        ~Ellipse() noexcept = default;

    protected:
        mglass::Point<mglass::float_type> center_;
        mglass::float_type xAxisLength_;
        mglass::float_type yAxisLength_;

    private: // Shape<Ellipse> implementation
        ShapeRectArea getBoundsImpl() const noexcept;

        Point<mglass::float_type> getPointAtScaledImpl(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point
        ) const noexcept;

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(IntegralRectArea rect, ConsumerFunctor&& consumer) const
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
                        (void)std::forward<ConsumerFunctor>(consumer)({ xUnaligned, yUnaligned }, 1);
                    }
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
