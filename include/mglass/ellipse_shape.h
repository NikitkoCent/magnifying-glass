#ifndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
#define MAGNIFYING_GLASS_ELLIPSE_SHAPE_H

#include "mglass/shape.h"   // Shape
#include <utility>          // std::forward
#include <algorithm>        // std::min


namespace mglass::shapes
{
    class Ellipse : public Shape<Ellipse>
    {
        friend struct Shape<Ellipse>;

    public: // ctors/dtor
        explicit Ellipse(
            Point<float_type> center = {0, 0},
            float_type xAxis = 0,
            float_type yAxis = 0) noexcept;

        ~Ellipse() noexcept = default;

    protected:
        Point<float_type> center_;
        float_type xAxisLength_;
        float_type yAxisLength_;

    private: // Shape<Ellipse> implementation
        [[nodiscard]] ShapeRectArea getBoundsImpl() const noexcept;

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(IntegralRectArea rect, ConsumerFunctor&& consumer) const
        {
            using namespace literals;

            // ellipse equation:
            // x^2 / a^2 + y^2 / b^2 = 1
            // avoid division ops (for performance reasons): b^2 * x^2 + a^2 * y^2 = a^2 * b^2

            if ((rect.width < 1) || (rect.height < 1))
                return;

            const IntegralRectArea thisIntegralBounds = getShapeIntegralBounds(*this);
            if ((thisIntegralBounds.width < 1) || (thisIntegralBounds.height < 1))
                return;

            const auto rectXMin = rect.topLeft.x;
            const auto rectXMax = rect.topLeft.x + static_cast<int_type>(rect.width - 1);

            const auto rectYMin = rect.topLeft.y - static_cast<int_type>(rect.height - 1);
            const auto rectYMax = rect.topLeft.y;

            // TODO: optimize the algorithm:
            //  1. find for every y the exact range [xMin, xMax] for which every point (x, y) is inside the ellipse

            // a^2 == (xAxisLength_ / 2) ^ 2
            const auto a2 = (xAxisLength_ * xAxisLength_) / 4;
            // b^2 == (yAxisLength_ / 2) ^ 2
            const auto b2 = (yAxisLength_ * yAxisLength_) / 4;

            const auto xStart = thisIntegralBounds.topLeft.x;
            const auto xEnd = thisIntegralBounds.topLeft.x + static_cast<int_type>(thisIntegralBounds.width);

            const auto yStart = thisIntegralBounds.topLeft.y;
            const auto yEnd = thisIntegralBounds.topLeft.y - static_cast<int_type>(thisIntegralBounds.height);

            for (int_type yUnaligned = yStart; yUnaligned > yEnd; --yUnaligned)
            {
                // TODO: optimize (yUnaligned < rectYMin)
                if ((yUnaligned < rectYMin) || (yUnaligned > rectYMax))
                    continue;

                // b^2 * x^2 + a^2 * y^2 <= a^2 * b^2
                // equals to
                // b^2 * x^2 + a^2(y^2 - b^2) <= 0
                // equals to
                // b^2 * x^2 <= -a^2(y^2 - b^2)
                // equals to
                // b^2 * x^2 <= a^2(b^2 - y^2) // TODO: make x^2 <= a^2 - y^2 / b^2

                // +0.5 is for moving to the pixel's center
                const float_type y = (static_cast<float_type>(yUnaligned) + 0.5_flt) - center_.y;
                const float_type y2 = y * y;

                const float_type rightPart = a2 * (b2 - y2);

                for (int_type xUnaligned = xStart; xUnaligned < xEnd; ++xUnaligned)
                {
                    // TODO: optimize (xUnaligned > rectXMax)
                    if ((xUnaligned < rectXMin) || (xUnaligned > rectXMax))
                        continue;

                    // +0.5 is for moving to the pixel's center
                    const float_type x = (static_cast<float_type>(xUnaligned) + 0.5_flt) - center_.x;

                    const float_type b2x2 = b2 * (x * x);

                    if (b2x2 <= rightPart)
                    {
                        (void)std::forward<ConsumerFunctor>(consumer)(
                            { xUnaligned, yUnaligned },
                            (std::min)(rightPart - b2x2 + 0.5_flt, 1_flt)
                        );
                    }
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
