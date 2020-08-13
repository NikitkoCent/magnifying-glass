#ifndef MAGNIFYING_GLASS_IMAGE_H
#define MAGNIFYING_GLASS_IMAGE_H

#include "mglass/primitives.h"  // size_type
#include <cstdint>              // std::uint8_t
#include <iosfwd>               // std::istream
#include <string_view>          // std::string_view
#include <vector>               // std::vector

namespace mglass
{
    struct ARGB final
    {
        std::uint8_t a;
        std::uint8_t r;
        std::uint8_t g;
        std::uint8_t b;


        static constexpr ARGB black() { return {255, 0, 0, 0}; }
        static constexpr ARGB transparent() { return {0, 255, 255, 255}; }
    };


    // Used coordinate system:
    //              (0;0)       x(width)
    //                O------------->
    //                |*****
    //                |*****
    //                |****O
    //                |  (5;3)
    //                |
    //      y(height) |
    //                v
    class Image final
    {
    public: // nested types

    public: // ctors/dtor
        explicit Image(mglass::size_type width = 0, mglass::size_type height = 0, ARGB color = ARGB::black());

        static Image fromPNGStream(std::istream& stream) noexcept(false); // TODO: implement

        // TODO: move to std::filesystem::path
        static Image fromPNGFile(std::string_view filePath) noexcept(false);

    public: // modifiers
        // content of the image is undefined after resizing
        // no memory re-allocations will be performed if (getWidth() * getHeight()) >= (newWidth * newHeight)
        void setSize(mglass::size_type newWidth, mglass::size_type newHeight);

        void setPixelAt(mglass::size_type x, mglass::size_type y, ARGB color);

        void fill(ARGB color);

    public: // getters
        mglass::size_type getWidth() const noexcept;
        mglass::size_type getHeight() const noexcept;

        ARGB getPixelAt(mglass::size_type x, mglass::size_type y) const;

        void saveToPNGStream(std::ostream& stream) const; // TODO: implement
        void saveToPNGFile(std::string_view filePath) const;

    private:
        std::vector<ARGB> data_;
        mglass::size_type width_;
        mglass::size_type height_;
    };
}

#endif // ndef MAGNIFYING_GLASS_IMAGE_H
