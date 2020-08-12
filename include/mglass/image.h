#ifndef MAGNIFYING_GLASS_IMAGE_H
#define MAGNIFYING_GLASS_IMAGE_H

#include "mglass/primitives.h"  // size_type
#include <cstdint>              // std::uint8_t
#include <cstddef>              // std::size_t
#include <iosfwd>               // std::istream

namespace mglass
{
    struct ARGB
    {
        std::uint8_t a;
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;
    };


    class Image
    {
    public: // nested types

    public: // ctors/dtor
        template<typename BytesInputIt>
        static Image fromPNG(BytesInputIt pngBegin, BytesInputIt pngEnd);

    public: // modifiers
        // content of the image is undefined after resizing
        // no momery re-allocations will be performed if (getWidth() * getHeight()) >= (newWidth * newHeight)
        void setSize(mglass::size_type newWidth, mglass::size_type newHeight);

        void setPixelAt(mglass::size_type x, mglass::size_type y, ARGB color);

        void fill(ARGB color);

    public: // getters
        mglass::size_type getWidth() const noexcept;
        mglass::size_type getHeight() const noexcept;

        ARGB getPixelAt(mglass::size_type x, mglass::size_type y) const;
    };
}

#endif // ndef MAGNIFYING_GLASS_IMAGE_H
