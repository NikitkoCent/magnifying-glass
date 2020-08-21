#ifndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
#define MAGNIFYING_GLASS_RECTANGLE_SHAPE_H

#include "mglass/shape.h"
#include <utility>              // std::forward
#include <algorithm>            // std::min, std::max


namespace mglass::shapes
{
    class Rectangle : public Shape<Rectangle>
    {
        friend struct Shape<Rectangle>;

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
            if ((rect.width < 1) || (rect.height < 1))
                return;

            const IntegralRectArea thisIntegralBounds = getShapeIntegralBounds(*this);
            if ((thisIntegralBounds.width < 1) || (thisIntegralBounds.height < 1))
                return;

            const int_type rectBottomRightX = rect.topLeft.x + static_cast<int_type>(rect.width - 1);
            const int_type rectBottomRightY = rect.topLeft.y - static_cast<int_type>(rect.height - 1);

            const int_type thisBottomRightX =
                thisIntegralBounds.topLeft.x + static_cast<int_type>(thisIntegralBounds.width - 1);
            const int_type thisBottomRightY =
                thisIntegralBounds.topLeft.y - static_cast<int_type>(thisIntegralBounds.height - 1);

            const int_type xStart = (std::max)(rect.topLeft.x, thisIntegralBounds.topLeft.x);
            const int_type xEnd = (std::min)(rectBottomRightX, thisBottomRightX) + 1;

            if (xStart >= xEnd)
                return;

            const int_type yStart = (std::min)(rect.topLeft.y, thisIntegralBounds.topLeft.y);
            const int_type yEnd = (std::max)(rectBottomRightY, thisBottomRightY) - 1;

            for (int_type y = yStart; y > yEnd; --y)
            {
                for (int_type x = xStart; x < xEnd; ++x)
                {
                    // TODO: compute the second parameter of the consumer
                    (void)std::forward<ConsumerFunctor>(consumer)({x, y}, 1);
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
