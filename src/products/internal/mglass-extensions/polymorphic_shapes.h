#ifndef MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPES_H
#define MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPES_H

#include "polymorphic_shape.h"
#include "mglass/shapes.h"      // mglass::shapes::*


namespace mglassext
{
    class PolymorphicRectangle final : public PolymorphicShape, public mglass::shapes::Rectangle
    {
    public: // ctors/dtor
        PolymorphicRectangle(mglass::float_type dx = 0, mglass::float_type dy = 0) noexcept;

    public: // modifiers
        void moveCenterTo(mglass::float_type newCenterX, mglass::float_type newCenterY) noexcept override;

        void setSize(mglass::float_type newWidth, mglass::float_type newHeight) noexcept(false) override;

    public: // getters
        std::string_view getIdentifier() const noexcept override;

        mglass::Point<mglass::float_type> getCenter() const noexcept override;
        mglass::float_type getWidth() const noexcept override;
        mglass::float_type getHeight() const noexcept override;

        void applyNearestNeighbor(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const override;

        void applyNearestNeighborAntiAliased(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const override;
    };


    class PolymorphicEllipse final : public PolymorphicShape, public mglass::shapes::Ellipse
    {
    public: // ctors/dtor
        PolymorphicEllipse(mglass::float_type dx = 0, mglass::float_type dy = 0) noexcept;

    public: // modifiers
        void moveCenterTo(mglass::float_type newCenterX, mglass::float_type newCenterY) noexcept override;

        void setSize(mglass::float_type newWidth, mglass::float_type newHeight) noexcept(false) override;

    public: // getters
        std::string_view getIdentifier() const noexcept override;

        mglass::Point<mglass::float_type> getCenter() const noexcept override;
        mglass::float_type getWidth() const noexcept override;
        mglass::float_type getHeight() const noexcept override;

        void applyNearestNeighbor(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const override;

        void applyNearestNeighborAntiAliased(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const override;
    };
} // namespace mglassext

#endif // ndef MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPES_H
