#ifndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
#define MAGNIFYING_GLASS_ELLIPSE_SHAPE_H

#include "mglass/shape.h"   // Shape
#include <utility>          // std::forward


namespace mglass::shapes
{
    namespace detail
    {
        class EllipseRastrContext final : public RasterizationContextBase<EllipseRastrContext>
        {
            // for accessing to getRasterizedPointImpl(), getPixelDensityImpl() from base
            friend struct RasterizationContextBase<EllipseRastrContext>;

        public:
            EllipseRastrContext(
                Point<int_type> rasterizedPoint,
                float_type x2,
                float_type a2Inversed,
                float_type y2divb2
            ) noexcept
                : rasterizedPoint_(rasterizedPoint)
                , x2_(x2)
                , a2Inversed_(a2Inversed)
                , y2divb2_(y2divb2)
            {}

        private: // RasterizationContextBase<EllipseRastrContext> implementation
            Point<int_type> getRasterizedPointImpl() const noexcept { return rasterizedPoint_; }

            float_type getPixelDensityImpl() const noexcept
            {
                using namespace literals;

                const float_type result = x2_ * a2Inversed_ + y2divb2_;
                return 1_flt - (result * result) * (result * result);
            }

        private:
            Point<int_type> rasterizedPoint_;
            float_type x2_;
            float_type a2Inversed_;
            float_type y2divb2_;
        };
    } // namespace detail


    class Ellipse : public Shape<Ellipse, detail::EllipseRastrContext>
    {
        friend struct Shape<Ellipse, detail::EllipseRastrContext>;

    public: // ctors/dtor
        // if xAxis is not inside the range [0; +inf) or yAxis is not inside the range [0; +inf),
        //  behaviour of other methods is undefined
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
            const auto a2Inversed = 1_flt / a2;
            // b^2 == (yAxisLength_ / 2) ^ 2
            const auto b2 = (yAxisLength_ * yAxisLength_) / 4;
            const auto b2Inversed = 1_flt / b2;

            const auto xStart = thisIntegralBounds.topLeft.x;
            const auto xEnd = thisIntegralBounds.topLeft.x + static_cast<int_type>(thisIntegralBounds.width);

            const auto yStart = thisIntegralBounds.topLeft.y;
            const auto yEnd = thisIntegralBounds.topLeft.y - static_cast<int_type>(thisIntegralBounds.height);

            for (int_type yUnaligned = yStart; yUnaligned > yEnd; --yUnaligned)
            {
                if (yUnaligned < rectYMin)
                    break;
                if (yUnaligned > rectYMax)
                    continue;

                // b^2 * x^2 + a^2 * y^2 <= a^2 * b^2
                // equals to
                // b^2 * x^2 + a^2(y^2 - b^2) <= 0
                // equals to
                // b^2 * x^2 <= -a^2(y^2 - b^2)
                // equals to
                // b^2 * x^2 <= a^2(b^2 - y^2)
                // equals to
                // x^2 <= a^2/b^2 * (b^2 - y^2)
                // equals to
                // x^2 <= a^2 * 1/b^2 * (b^2 - y^2)

                // +0.5 is for moving to the pixel's center
                const float_type y = (static_cast<float_type>(yUnaligned) + 0.5_flt) - center_.y;
                const float_type y2 = y * y;
                const float_type y2divb2 = y2 * b2Inversed;

                const float_type rightPart = a2 - a2 * y2divb2;

                for (int_type xUnaligned = xStart; xUnaligned < xEnd; ++xUnaligned)
                {
                    if (xUnaligned > rectXMax)
                        break;
                    if (xUnaligned < rectXMin)
                        continue;

                    // +0.5 is for moving to the pixel's center
                    const float_type x = (static_cast<float_type>(xUnaligned) + 0.5_flt) - center_.x;
                    const float_type x2 = x * x;

                    if (x2 <= rightPart)
                    {
                        float_type pointDensity = x2 * a2Inversed + y2divb2;
                        pointDensity = 1_flt - (pointDensity * pointDensity) * (pointDensity * pointDensity);

                        (void)std::forward<ConsumerFunctor>(consumer)(
                            detail::EllipseRastrContext{ { xUnaligned, yUnaligned }, x2, a2Inversed, y2divb2 }
                        );
                    }
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_ELLIPSE_SHAPE_H
