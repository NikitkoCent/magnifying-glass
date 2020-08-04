#ifndef MAGNIFYING_GLASS_IMAGE_H
#define MAGNIFYING_GLASS_IMAGE_H

#include <cstdint>      // std::uint8_t
#include <cstddef>      // std::size_t
#include <iosfwd>       // std::istream

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
        using size_type = std::size_t;

    public: // ctors/dtor
        template<typename BytesInputIt>
        static Image fromPNG(BytesInputIt pngBegin, BytesInputIt pngEnd);

    public: // modifiers


    public: // getters
        size_type getWidth() const noexcept;
        size_type getHeight() const noexcept;
    };
}

#endif // ndef MAGNIFYING_GLASS_IMAGE_H
