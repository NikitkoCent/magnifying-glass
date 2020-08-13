#include "mglass/image.h"
#include "lodepng/lodepng.h"
#include <string>

namespace mglass
{
    Image::Image(mglass::size_type width, mglass::size_type height, ARGB color)
        : data_(width * height, color)
        , width_(height > 0 ? width : 0)
        , height_(width_ > 0 ? height : 0)
    {}


    Image Image::fromPNGFile(std::string_view filePath) noexcept(false)
    {
        std::vector<unsigned char> rawRGBA;
        unsigned width, height;

        const auto err = lodepng::decode(rawRGBA, width, height, std::string{filePath});
        if (err)
            return Image{}; // TODO: throw exception

        if ( (rawRGBA.size() % 4) != 0 )
            return Image{}; // TODO: throw exception

        Image result;
        result.setSize(width, height);

        std::vector<unsigned char>::size_type i = 0;

        for (mglass::size_type y = 0; y < height; ++y)
        {
            for (mglass::size_type x = 0; x < width; ++x)
            {
                ARGB pixel{};
                pixel.r = static_cast<std::uint8_t>(rawRGBA[i++]);
                pixel.g = static_cast<std::uint8_t>(rawRGBA[i++]);
                pixel.b = static_cast<std::uint8_t>(rawRGBA[i++]);
                pixel.a = static_cast<std::uint8_t>(rawRGBA[i++]);

                result.setPixelAt(x, y, pixel);
            }
        }

        return result;
    }


    void Image::setSize(mglass::size_type newWidth, mglass::size_type newHeight)
    {
        width_ = newWidth;
        height_ = newHeight;

        if (width_ < 1) height_ = 0;
        if (height_ < 1) width_ = 0;

        data_.resize(width_ * height_);
    }


    void Image::setPixelAt(mglass::size_type x, mglass::size_type y, ARGB color)
    {
        data_[y * width_ + x] = color;
    }


    void Image::fill(ARGB color)
    {
        for (ARGB& pixel : data_)
            pixel = color;
    }


    mglass::size_type Image::getWidth() const noexcept
    {
        return width_;
    }

    mglass::size_type Image::getHeight() const noexcept
    {
        return height_;
    }


    ARGB Image::getPixelAt(mglass::size_type x, mglass::size_type y) const
    {
        return data_[y * width_ + x];
    }


    void Image::saveToPNGFile(std::string_view filePath) const
    {
        std::vector<unsigned char> rawRGBA;
        rawRGBA.reserve(width_ * height_);

        for (const ARGB pixel : data_)
        {
            rawRGBA.emplace_back(pixel.r);
            rawRGBA.emplace_back(pixel.g);
            rawRGBA.emplace_back(pixel.b);
            rawRGBA.emplace_back(pixel.a);
        }

        const auto err = lodepng::encode(std::string{filePath}, rawRGBA, width_, height_);
        if (err)
            ; // TODO: throw exception
    }
}
