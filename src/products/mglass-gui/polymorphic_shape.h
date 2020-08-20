#ifndef MGLASS_GUI_POLYMORPHIC_SHAPE_H
#define MGLASS_GUI_POLYMORPHIC_SHAPE_H

#include "mglass/mglass.h" // mglass::*


struct PolymorphicShape
{
    virtual ~PolymorphicShape() noexcept = default;


    void moveCenterTo(mglass::Point<mglass::float_type> newCenter) noexcept
    {
        moveCenterTo(newCenter.x, newCenter.y);
    }

    virtual void moveCenterTo(mglass::float_type x, mglass::float_type y) noexcept = 0;


    virtual void applyNearestNeighbor(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const = 0;

    virtual void applyNearestNeighborInterpolated(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const = 0;
};

#endif // ndef MGLASS_GUI_POLYMORPHIC_SHAPE_H
