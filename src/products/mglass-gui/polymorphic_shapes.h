#ifndef MGLASS_GUI_POLYMORPHIC_SHAPES_H
#define MGLASS_GUI_POLYMORPHIC_SHAPES_H

#include "polymorphic_shape.h"
#include "mglass/shapes.h"      // mglass::shapes::*


struct PolymorphicRectangle final : PolymorphicShape, mglass::shapes::Rectangle
{
    PolymorphicRectangle(mglass::float_type dx, mglass::float_type dy) noexcept;
    ~PolymorphicRectangle() noexcept = default;


    void moveCenterTo(mglass::float_type x, mglass::float_type y) noexcept override
    {
        center_.x = x;
        center_.y = y;
    }


    void applyNearestNeighbor(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const override;

    void applyNearestNeighborInterpolated(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const override;
};


struct PolymorphicEllipse final : PolymorphicShape, mglass::shapes::Ellipse
{
    PolymorphicEllipse(mglass::float_type dx, mglass::float_type dy) noexcept;
    ~PolymorphicEllipse() noexcept = default;


    void moveCenterTo(mglass::float_type x, mglass::float_type y) noexcept override
    {
        center_.x = x;
        center_.y = y;
    }


    void applyNearestNeighbor(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const override;

    void applyNearestNeighborInterpolated(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst
    ) const override;
};


#endif // ndef MGLASS_GUI_POLYMORPHIC_SHAPES_H
