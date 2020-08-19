#ifndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
#define MAGNIFYING_GLASS_RECTANGLE_SHAPE_H

#include "mglass/shape.h"
#include "mglass/primitives.h"  // mglass::Point, mglass::float_type
#include <utility>              // std::forward
#include <cmath>                // std::min, std::max


namespace mglass::shapes
{
    class Rectangle : public Shape<Rectangle>
    {
        friend struct Shape<Rectangle>;

    public: // ctors/dtor
        explicit Rectangle(
            mglass::Point<mglass::float_type> center = {0, 0},
            mglass::float_type width = 0,
            mglass::float_type height = 0) noexcept;

        ~Rectangle() noexcept = default;

    protected:
        mglass::Point<mglass::float_type> center_;
        mglass::float_type width_;
        mglass::float_type height_;

    private: // Shape<Rectangle> implementation
        ShapeRectArea getBoundsImpl() const noexcept;

        Point<mglass::float_type> getPointAtScaledImpl(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point
        ) const noexcept;

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(IntegralRectArea rect, ConsumerFunctor&& consumer) const
        {
            if ((rect.width < 1) || (rect.height < 1))
                return;

            const IntegralRectArea thisIntegralBounds = getShapeIntegralBounds(*this);
            if ((thisIntegralBounds.width < 1) || (thisIntegralBounds.height < 1))
                return;

            const mglass::int_type rectBottomRightX = rect.topLeft.x + static_cast<mglass::int_type>(rect.width - 1);
            const mglass::int_type rectBottomRightY = rect.topLeft.y - static_cast<mglass::int_type>(rect.height - 1);

            const mglass::int_type thisBottomRightX = thisIntegralBounds.topLeft.x + static_cast<mglass::int_type>(thisIntegralBounds.width - 1);
            const mglass::int_type thisBottomRightY = thisIntegralBounds.topLeft.y - static_cast<mglass::int_type>(thisIntegralBounds.height - 1);

            const mglass::int_type xStart = (std::max)(rect.topLeft.x, thisIntegralBounds.topLeft.x);
            const mglass::int_type xEnd = (std::min)(rectBottomRightX, thisBottomRightX) + 1;

            if (xStart >= xEnd)
                return;

            const mglass::int_type yStart = (std::min)(rect.topLeft.y, thisIntegralBounds.topLeft.y);
            const mglass::int_type yEnd = (std::max)(rectBottomRightY, thisBottomRightY) - 1;

            for (mglass::int_type y = yStart; y > yEnd; --y)
            {
                for (mglass::int_type x = xStart; x < xEnd; ++x)
                {
                    // TODO: compute the second parameter of the consumer
                    (void)std::forward<ConsumerFunctor>(consumer)({x, y}, 1);
                }
            }
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
