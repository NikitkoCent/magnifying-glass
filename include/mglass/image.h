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
    public: // ctors/dtor
        explicit Image(size_type width = 0, size_type height = 0, ARGB color = ARGB::black());

        // throws std::runtime_error if it is failed to parse the stream
        static Image fromPNGStream(std::istream& stream) noexcept(false);

        // throws std::runtime_error if it is failed to open/parse the file
        // TODO: move to std::filesystem::path
        static Image fromPNGFile(std::string_view filePath) noexcept(false);

    public: // modifiers
        // content of the image is undefined after resizing
        // no memory re-allocations will be performed if (getWidth() * getHeight()) >= (newWidth * newHeight)
        void setSize(size_type newWidth, size_type newHeight);

        void setPixelAt(size_type x, size_type y, ARGB color);

        void fill(ARGB color);

    public: // getters
        [[nodiscard]] size_type getWidth() const noexcept;
        [[nodiscard]] size_type getHeight() const noexcept;

        [[nodiscard]] ARGB getPixelAt(size_type x, size_type y) const;

        void saveToPNGStream(std::ostream& stream) const;

        // throws std::runtime_error if it is failed to save this to the file at `filePath`
        // TODO: move to std::filesystem::path
        void saveToPNGFile(std::string_view filePath) const noexcept(false);

    private:
        std::vector<ARGB> data_;
        size_type width_;
        size_type height_;
    };
} // namespace mglass

#endif // ndef MAGNIFYING_GLASS_IMAGE_H
