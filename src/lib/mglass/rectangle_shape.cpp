#include "mglass/rectangle_shape.h"


namespace mglass::shapes
{
    Rectangle::Rectangle(Point<float_type> center, float_type width, float_type height) noexcept
        : center_(center)
        , width_(width)
        , height_(height)
    {}


    ShapeRectArea Rectangle::getBoundsImpl() const noexcept
    {
        return { { center_.x - width_ / 2, center_.y + height_ / 2 }, width_, height_ };
    }

} // namespace mglass::shapes
