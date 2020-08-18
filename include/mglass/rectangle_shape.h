#ifndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
#define MAGNIFYING_GLASS_RECTANGLE_SHAPE_H

#include "mglass/shape.h"


namespace mglass::shapes
{
    class Rectangle : public Shape<Rectangle>
    {
        friend struct Shape<Rectangle>;

    private:
        ShapeRectArea getBoundsImpl() const;

        Point<mglass::float_type> getPointAtScaledImpl(
            mglass::float_type scaleFactor,
            Point<mglass::float_type> point
        ) const;

        template<typename ConsumerFunctor>
        void rasterizeOntoImpl(IntegralRectArea rect, ConsumerFunctor&& consumer) const
        {
            static_assert(false, "is not implemented");
        }
    };
} // namespace mglass::shapes

#endif // ndef MAGNIFYING_GLASS_RECTANGLE_SHAPE_H
