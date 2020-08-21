#include "polymorphic_shapes.h"
#include "mglass/magnifiers.h"      // mglass::magnifiers::*


PolymorphicRectangle::PolymorphicRectangle(mglass::float_type dx, mglass::float_type dy) noexcept
    : mglass::shapes::Rectangle({0, 0}, dx, dy)
{}

void PolymorphicRectangle::applyNearestNeighbor(
    mglass::float_type scaleFactor,
    const mglass::Image& imageSrc,
    mglass::Point<mglass::int_type> imageTopLeft,
    mglass::Image& imageDst,
    bool enableAlphaBlending) const
{
    mglass::magnifiers::nearestNeighbor(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
}

void PolymorphicRectangle::applyNearestNeighborInterpolated(
    mglass::float_type scaleFactor,
    const mglass::Image& imageSrc,
    mglass::Point<mglass::int_type> imageTopLeft,
    mglass::Image& imageDst,
    bool enableAlphaBlending) const
{
    mglass::magnifiers::nearestNeighborInterpolated(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
}


PolymorphicEllipse::PolymorphicEllipse(mglass::float_type dx, mglass::float_type dy) noexcept
    : mglass::shapes::Ellipse({0, 0}, dx, dy)
{}

void PolymorphicEllipse::applyNearestNeighbor(
    mglass::float_type scaleFactor,
    const mglass::Image& imageSrc,
    mglass::Point<mglass::int_type> imageTopLeft,
    mglass::Image& imageDst,
    bool enableAlphaBlending) const
{
    mglass::magnifiers::nearestNeighbor(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
}

void PolymorphicEllipse::applyNearestNeighborInterpolated(
    mglass::float_type scaleFactor,
    const mglass::Image& imageSrc,
    mglass::Point<mglass::int_type> imageTopLeft,
    mglass::Image& imageDst,
    bool enableAlphaBlending) const
{
    mglass::magnifiers::nearestNeighborInterpolated(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
}
