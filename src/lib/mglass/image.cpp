#include "mglass/image.h"
#include "stb/stb_image.h"          // stbi_*
#include "stb/stb_image_write.h"    // stbi_write_*
#include <string>                   // std::string
#include <stdexcept>                // std::runtime_error
#include <iostream>                 // std::istream, std::ostream
#include <memory>                   // std::unique_ptr
#include <fstream>                  // std::ifstream, std::ofstream

// TBD: probably we should avoid using exceptions for indicating runtime errors
//      and should use smth like std::error_code or std::error_condition

namespace mglass
{
    Image::Image(size_type width, size_type height, ARGB color)
        : data_(width * height, color)
        , width_(height > 0 ? width : 0)
        , height_(width_ > 0 ? height : 0)
    {}


    Image Image::fromPNGStream(std::istream& stream) noexcept(false)
    {
        stbi_io_callbacks ioCallbacks;

        ioCallbacks.read = [](void* user, char* data, int size) noexcept -> int {
            if (size < 0)
                return size;

            std::istream& stream = *reinterpret_cast<std::istream*>(user);

            (void)stream.read(data, static_cast<std::streamsize>(size));

            return static_cast<int>(stream.gcount());
        };

        ioCallbacks.skip = [](void* user, int n) noexcept {
            std::istream& stream = *reinterpret_cast<std::istream*>(user);
            stream.seekg(static_cast<std::istream::off_type>(n), std::ios_base::cur);
        };

        ioCallbacks.eof = [](void* user) noexcept -> int {
            std::istream& stream = *reinterpret_cast<std::istream*>(user);
            return (!stream.good());
        };

        int widthSigned, heightSigned;
        int streamChannels;

        using StbImageHolder = std::unique_ptr<stbi_uc[], decltype(&stbi_image_free)>;
        const StbImageHolder image{
            stbi_load_from_callbacks(
                &ioCallbacks,
                &stream,
                &widthSigned,
                &heightSigned,
                &streamChannels,
                STBI_rgb_alpha
            ),
            &stbi_image_free
        };

        if (!image)
            throw std::runtime_error(stbi_failure_reason());

        if (widthSigned < 0)
            throw std::runtime_error("image width < 0");
        if (heightSigned < 0)
            throw std::runtime_error("image height < 0");

        Image result{ static_cast<size_type>(widthSigned), static_cast<size_type>(heightSigned) };

        int i = 0;
        for (ARGB& pixel : result.data_)
        {
            pixel.r = image[i++];
            pixel.g = image[i++];
            pixel.b = image[i++];
            pixel.a = image[i++];
        }

        return result;
    }

    Image Image::fromPNGFile(std::string_view filePath) noexcept(false)
    {
        std::ifstream fStream(std::string{filePath}, std::ios::binary);

        if (!fStream.is_open())
            throw std::runtime_error("failed to open the input file");

        return fromPNGStream(fStream);
    }


    void Image::setSize(size_type newWidth, size_type newHeight)
    {
        width_ = newWidth;
        height_ = newHeight;

        if (width_ < 1) height_ = 0;
        if (height_ < 1) width_ = 0;

        data_.resize(width_ * height_);
    }


    void Image::setPixelAt(size_type x, size_type y, ARGB color)
    {
        data_[y * width_ + x] = color;
    }


    void Image::fill(ARGB color)
    {
        for (ARGB& pixel : data_)
            pixel = color;
    }


    size_type Image::getWidth() const noexcept
    {
        return width_;
    }

    size_type Image::getHeight() const noexcept
    {
        return height_;
    }


    ARGB Image::getPixelAt(size_type x, size_type y) const
    {
        return data_[y * width_ + x];
    }


    void Image::saveToPNGStream(std::ostream& stream) const
    {
        stbi_write_func* const writeFn = [](void* context, void* data, int size) noexcept {
            if (size < 0)
                return;

            std::ostream& stream = *reinterpret_cast<std::ostream*>(context);

            const auto cchData = const_cast<const char*>(reinterpret_cast<char*>(data));
            stream.write(cchData, static_cast<std::streamsize>(size));
        };

        const auto rawRGBA = [this] {
            std::vector<std::uint8_t> result;
            result.reserve(getWidth() * getHeight() * 4);

            for (ARGB pixel : data_)
            {
                result.emplace_back(pixel.r);
                result.emplace_back(pixel.g);
                result.emplace_back(pixel.b);
                result.emplace_back(pixel.a);
            }

            return result;
        }();

        const auto widthSigned = static_cast<int>(getWidth());
        const auto heightSigned = static_cast<int>(getHeight());

        const auto err = stbi_write_png_to_func(
            writeFn,
            &stream,
            widthSigned,
            heightSigned,
            STBI_rgb_alpha,
            rawRGBA.data(),
            widthSigned * 4
        );

        if (err == 0)
            throw std::runtime_error("failed to write the image to the stream");
    }

    void Image::saveToPNGFile(std::string_view filePath) const noexcept(false)
    {
        std::ofstream fStream{ std::string{filePath}, std::ios::binary };
        if (!fStream.is_open())
            throw std::runtime_error("failed to open the output file");

        saveToPNGStream(fStream);
    }
} // namespace mglass
