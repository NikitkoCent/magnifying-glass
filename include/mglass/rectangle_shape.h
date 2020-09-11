#ifndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
#define MAGNIFYING_GLASS_RECTANGLE_SHAPE_H

#include "mglass/shape.h"
#include <utility>              // std::forward
#include <algorithm>            // std::min, std::max
#include <cassert>              // assert


namespace mglass::shapes
{
    namespace detail
    {
        class RectangleRastrContext final : public RasterizationContextBase<RectangleRastrContext>
        {
            // for accessing to getRasterizedPointImpl(), getPixelDensityImpl() from base
            friend struct RasterizationContextBase<RectangleRastrContext>;

        public:
            RectangleRastrContext(
                Point<int_type> rasterizedPoint,
                Point<float_type> rectCenter,
                float_type rectWidth,
                float_type rectHeight) noexcept
                : rasterizedPoint_(rasterizedPoint)
                , rectCenter_(rectCenter)
                , rectWidth_(rectWidth)
                , rectHeight_(rectHeight)
            {}

        private: // RasterizationContextBase<RectangleRastrContext> implementation
            Point<int_type> getRasterizedPointImpl() const noexcept { return rasterizedPoint_; }

            float_type getPixelDensityImpl() const noexcept
            {
                using namespace mglass::literals;

                auto [x, y] = pointCast<float_type>(rasterizedPoint_);
                x -= rectCenter_.x;
                y -= rectCenter_.y;

                const auto relXLength = 1_flt - 2_flt * std::abs(x) / rectWidth_;
                const auto relYLength = 1_flt - 2_flt * std::abs(y) / rectHeight_;

                assert( ((relXLength >= 0_flt) && (relXLength <= 1_flt)) );
                assert( ((relYLength >= 0_flt) && (relYLength <= 1_flt)) );

                return std::pow(relXLength * relYLength, 1_flt / 5_flt);
            }

        private:
            Point<int_type> rasterizedPoint_;
            Point<float_type> rectCenter_;
            float_type rectWidth_;
            float_type rectHeight_;
        };
    } // namespace detail


    class Rectangle : public Shape<Rectangle, detail::RectangleRastrContext>
    {
        friend struct Shape<Rectangle, detail::RectangleRastrContext>;

    public: // ctors/dtor
        explicit Rectangle(
            Point<float_type> center = {0, 0},
            float_type width = 0,
            float_type height = 0) noexcept;

        ~Rectangle() noexcept = default;

    protected:
        Point<float_type> center_;
        float_type width_;
        float_type height_;

    private: // Shape<Rectangle> implementation
        [[nodiscard]] ShapeRectArea getBoundsImpl() const noexcept;

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(IntegralRectArea rect, ConsumerFunctor&& consumer) const
        {
            using literals::operator""_flt;

            if ((rect.width < 1) || (rect.height < 1))
                return;

            const ShapeRectArea thisPreciseBounds = getBounds();
            const IntegralRectArea thisIntegralBounds = getShapeIntegralBounds(*this);
            if ((thisIntegralBounds.width < 1) || (thisIntegralBounds.height < 1))
                return;

            const int_type rectBottomRightX = rect.topLeft.x + static_cast<int_type>(rect.width - 1);
            const int_type rectBottomRightY = rect.topLeft.y - static_cast<int_type>(rect.height - 1);

            const int_type thisBottomRightX =
                thisIntegralBounds.topLeft.x + static_cast<int_type>(thisIntegralBounds.width - 1);
            const int_type thisBottomRightY =
                thisIntegralBounds.topLeft.y - static_cast<int_type>(thisIntegralBounds.height - 1);

            const int_type xStart = [&rect, &thisPreciseBounds, &thisIntegralBounds]{
                int_type result = (std::max)(rect.topLeft.x, thisIntegralBounds.topLeft.x);
                if ( (static_cast<float_type>(result) + 0.5_flt) < (thisPreciseBounds.topLeft.x) )
                    ++result;
                return result;
            }();
            const int_type xEnd = [&thisPreciseBounds, rectBottomRightX, thisBottomRightX]{
                int_type result = (std::min)(rectBottomRightX, thisBottomRightX);
                const float_type afterRightX = (thisPreciseBounds.topLeft.x + thisPreciseBounds.width);
                if ( (static_cast<float_type>(result) + 0.5_flt) >= afterRightX)
                    --result;
                if ( (static_cast<float_type>(result) + 0.5_flt) >= afterRightX)
                    --result;
                return ++result;
            }();

            if (xStart >= xEnd)
                return;

            const int_type yStart = [&rect, &thisPreciseBounds, &thisIntegralBounds]{
                int_type result = (std::min)(rect.topLeft.y, thisIntegralBounds.topLeft.y);
                if ( (static_cast<float_type>(result) + 0.5_flt) > thisPreciseBounds.topLeft.y )
                    --result;
                if ( (static_cast<float_type>(result) + 0.5_flt) > thisPreciseBounds.topLeft.y )
                    --result;
                return result;
            }();
            const int_type yEnd = [&thisPreciseBounds, rectBottomRightY, thisBottomRightY]{
                int_type result = (std::max)(rectBottomRightY, thisBottomRightY);
                const float_type beforeBottomY = thisPreciseBounds.topLeft.y - thisPreciseBounds.height;
                if ( (static_cast<float_type>(result) + 0.5_flt) <= beforeBottomY )
                    ++result;
                if ( (static_cast<float_type>(result) + 0.5_flt) <= beforeBottomY )
                    ++result;
                return --result;
            }();

            for (int_type y = yStart; y > yEnd; --y)
            {
                for (int_type x = xStart; x < xEnd; ++x)
                {
                    (void)std::forward<ConsumerFunctor>(consumer)(
                        detail::RectangleRastrContext{ {x, y}, center_, width_, height_ }
                    );
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
