#include "mglass/rectangle_shape.h"


namespace mglass::shapes
{
    Rectangle::Rectangle(
        mglass::Point<mglass::float_type> center,
        mglass::float_type width,
        mglass::float_type height) noexcept
        : center_(center)
        , width_(width)
        , height_(height)
    {}


    ShapeRectArea Rectangle::getBoundsImpl() const noexcept
    {
        return { { center_.x - width_ / 2, center_.y + height_ / 2 }, width_, height_ };
    }


    Point<mglass::float_type> Rectangle::getPointAtScaledImpl(
        mglass::float_type scaleFactor,
        Point<mglass::float_type> point) const noexcept
    {
        const auto dx = point.x - center_.x;
        const auto dy = point.y - center_.y;

        return { center_.x + dx / scaleFactor, center_.y + dy / scaleFactor };
    }

} // namespace mglass::shapes
