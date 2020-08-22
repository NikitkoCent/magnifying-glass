#ifndef MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPE_H
#define MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPE_H

#include "mglass/primitives.h"
#include <string_view>


// forward declarations
namespace mglass
{
    class Image;
}


namespace mglassext
{
    struct PolymorphicShape
    {
    public: // dtor
        virtual ~PolymorphicShape() noexcept = default;

    public: // modifiers
        void moveCenterTo(mglass::Point<mglass::float_type> newCenter) noexcept
        {
            return moveCenterTo(newCenter.x, newCenter.y);
        }

        virtual void moveCenterTo(mglass::float_type newCenterX, mglass::float_type newCenterY) noexcept = 0;

        virtual void setSize(mglass::float_type newWidth, mglass::float_type newHeight) noexcept(false) = 0;

    public: // getters
        virtual std::string_view getIdentifier() const noexcept = 0;

        virtual mglass::Point<mglass::float_type> getCenter() const noexcept = 0;
        virtual mglass::float_type getWidth() const noexcept = 0;
        virtual mglass::float_type getHeight() const noexcept = 0;

        virtual void applyNearestNeighbor(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const = 0;

        virtual void applyNearestNeighborAntiAliased(
            mglass::float_type scaleFactor,
            const mglass::Image& imageSrc,
            mglass::Point<mglass::int_type> imageTopLeft,
            mglass::Image& imageDst,
            bool enableAlphaBlending) const = 0;
    };
} // mglassext

#endif // ndef MAGNIFYING_GLASS_EXTENSIONS_POLYMORPHIC_SHAPE_H
