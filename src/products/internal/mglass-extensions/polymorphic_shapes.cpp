#include "polymorphic_shapes.h"
#include "mglass/magnifiers.h"  // mglass::magnifiers::*
#include <stdexcept>            // std::invalid_argument


namespace mglassext
{
    // ================================================================================================================
    //  PolymorphicRectangle
    // ================================================================================================================

    PolymorphicRectangle::PolymorphicRectangle(mglass::float_type dx, mglass::float_type dy) noexcept
        : mglass::shapes::Rectangle({}, dx, dy)
    {}

    void PolymorphicRectangle::moveCenterTo(mglass::float_type newCenterX, mglass::float_type newCenterY) noexcept
    {
        center_.x = newCenterX;
        center_.y = newCenterY;
    }

    void PolymorphicRectangle::setSize(mglass::float_type newWidth, mglass::float_type newHeight) noexcept(false)
    {
        if (newWidth < 0)
            throw std::invalid_argument("PolymorphicRectangle::setWidth: `newWidth` < 0");
        if (newHeight < 0)
            throw std::invalid_argument("PolymorphicRectangle::setWidth: `newHeight` < 0");

        width_ = newWidth;
        height_ = newHeight;

        if (width_ == 0) height_ = 0;
        if (height_ == 0) width_ = 0;
    }

    std::string_view PolymorphicRectangle::getIdentifier() const noexcept
    {
        return "rectangle";
    }

    mglass::Point<mglass::float_type> PolymorphicRectangle::getCenter() const noexcept
    {
        return center_;
    }

    mglass::float_type PolymorphicRectangle::getWidth() const noexcept
    {
        return width_;
    }

    mglass::float_type PolymorphicRectangle::getHeight() const noexcept
    {
        return height_;
    }

    void PolymorphicRectangle::applyNearestNeighbor(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst,
        bool enableAlphaBlending) const
    {
        mglass::magnifiers::nearestNeighbor(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
    }

    void PolymorphicRectangle::applyNearestNeighborAntiAliased(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst,
        bool enableAlphaBlending) const
    {
        mglass::magnifiers::nearestNeighborInterpolated(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
    }


    // ================================================================================================================
    //  PolymorphicEllipse
    // ================================================================================================================

    PolymorphicEllipse::PolymorphicEllipse(mglass::float_type dx, mglass::float_type dy) noexcept
        : mglass::shapes::Ellipse({}, dx, dy)
    {}

    void PolymorphicEllipse::moveCenterTo(mglass::float_type newCenterX, mglass::float_type newCenterY) noexcept
    {
        center_.x = newCenterX;
        center_.y = newCenterY;
    }

    void PolymorphicEllipse::setSize(mglass::float_type newWidth, mglass::float_type newHeight) noexcept(false)
    {
        if (newWidth < 0)
            throw std::invalid_argument("PolymorphicEllipse::setWidth: `newWidth` < 0");
        if (newHeight < 0)
            throw std::invalid_argument("PolymorphicEllipse::setWidth: `newHeight` < 0");

        xAxisLength_ = newWidth;
        yAxisLength_ = newHeight;

        if (xAxisLength_ == 0) yAxisLength_ = 0;
        if (yAxisLength_ == 0) xAxisLength_ = 0;
    }

    std::string_view PolymorphicEllipse::getIdentifier() const noexcept
    {
        return "ellipse";
    }

    mglass::Point<mglass::float_type> PolymorphicEllipse::getCenter() const noexcept
    {
        return center_;
    }

    mglass::float_type PolymorphicEllipse::getWidth() const noexcept
    {
        return xAxisLength_;
    }

    mglass::float_type PolymorphicEllipse::getHeight() const noexcept
    {
        return yAxisLength_;
    }

    void PolymorphicEllipse::applyNearestNeighbor(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst,
        bool enableAlphaBlending) const
    {
        mglass::magnifiers::nearestNeighbor(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
    }

    void PolymorphicEllipse::applyNearestNeighborAntiAliased(
        mglass::float_type scaleFactor,
        const mglass::Image& imageSrc,
        mglass::Point<mglass::int_type> imageTopLeft,
        mglass::Image& imageDst,
        bool enableAlphaBlending) const
    {
        mglass::magnifiers::nearestNeighborInterpolated(*this, scaleFactor, imageSrc, imageTopLeft, imageDst, enableAlphaBlending);
    }
} // namespace mglassext
