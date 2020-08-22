#include "mglass/mglass.h"      // mglass::*
#include "mglass/shapes.h"      // mglass::shapes*
#include "mglass/magnifiers.h"  // mglass::magnifiers::*
#include <iostream>             // std::cin, std::cout, std::ostream
#include <fstream>              // std::ifstream, std::ofstream
#include <string>               // std::string
#include <string_view>          // std::string_view
#include <exception>            // std::exception
#include <stdexcept>            // std::runtime_error, std::system_error
#include <optional>             // std::optional
#include <variant>              // std::variant, std::visit
//#include <charconv>             // std::from_chars
#include <cstdlib>              // std::strtof
#include <utility>              // std::move
#include <type_traits>          // std::remove_reference_t, std::remove_cv_t


// TODO: use polymorphic shapes


using mglass::float_type;
using mglass::int_type;

namespace shapes { using namespace mglass::shapes; }


struct CmdArgs
{
    static constexpr int requiredArgsCount = 4;


    static CmdArgs parse(int argc, char* argv[]) noexcept(false);

    static std::ostream& writeHelp(std::ostream& stream);


    std::ostream& dumpValues(std::ostream& stream) const;


    mglass::Image image;
    // TODO: move to std::filepath
    std::string outputFilePath;
    std::variant<shapes::Ellipse, shapes::Rectangle> shape;
    float_type scaleFactor = 2;
    bool antialiasingIsEnabled = false;
    bool alphaBlendingIsEnabled = false;
    mglass::Point<int_type> imageTopLeft = {0, 0};
};


int main(const int argc, char *argv[])
{
    try
    {
        if (argc < 1 + CmdArgs::requiredArgsCount)
        {
            std::cerr << "Not enough parameters." << std::endl;
            CmdArgs::writeHelp(std::cout) << std::endl;
            return 1;
        }

        const auto args = CmdArgs::parse(argc - 1, argv + 1);
        args.dumpValues(std::clog) << std::endl;

        mglass::Image outputImage;

        std::visit(
            [&args, &outputImage](const auto& shape){
                if (args.antialiasingIsEnabled)
                    mglass::magnifiers::nearestNeighborInterpolated(
                        shape,
                        args.scaleFactor,
                        args.image,
                        args.imageTopLeft,
                        outputImage,
                        args.alphaBlendingIsEnabled
                    );
                else
                    mglass::magnifiers::nearestNeighbor(
                        shape,
                        args.scaleFactor,
                        args.image,
                        args.imageTopLeft,
                        outputImage,
                        args.alphaBlendingIsEnabled
                    );
            },
            args.shape
        );

        std::ofstream outputFile{args.outputFilePath, std::ios::binary};
        if (!outputFile.is_open())
            throw std::runtime_error{"Failed to open file for writing: \"" + args.outputFilePath + '\"'};

        outputImage.saveToPNGStream(outputFile);

        return 0;
    }
    catch (const std::exception& err)
    {
        std::cerr << "Error: \"" << err.what() << "\".\n";
        return 254;
    }
    catch (...)
    {
        std::cerr << "Unknown error.\n";
        return 255;
    }
}


// CmdArgs implementation

CmdArgs CmdArgs::parse(const int argc, char **argv) noexcept(false)
{
    using namespace std::string_literals;

    // TODO: move to std::filepath
    std::optional<decltype(CmdArgs::outputFilePath)> outputFilePath                 = std::nullopt;
    std::optional<decltype(CmdArgs::shape)> shape                                   = std::nullopt;
    std::optional<float_type> shapeWidth                                            = std::nullopt;
    std::optional<float_type> shapeHeight                                           = std::nullopt;
    std::optional<float_type> shapeCenterX                                          = std::nullopt;
    std::optional<float_type> shapeCenterY                                          = std::nullopt;
    std::optional<decltype(CmdArgs::scaleFactor)> scaleFactor                       = std::nullopt;
    bool antialiasingIsEnabled                                                      = false;
    bool alphaBlendingIsEnabled                                                     = false;
    std::optional<float_type> imageTopLeftX                                         = std::nullopt;
    std::optional<float_type> imageTopLeftY                                         = std::nullopt;

    for (int i = 0; i < (argc - 1); ++i)
    {
        const std::string_view arg = argv[i];

        if (arg.substr(0, 9) == "--output=")
        {
            if (outputFilePath.has_value())
                throw std::runtime_error("`--output` parameter occurs several times");

            outputFilePath = arg.substr(9);
        }
        else if (arg.substr(0, 8) == "--shape=")
        {
            if (shape.has_value())
                throw std::runtime_error("`--shape` parameter occurs several times");

            const auto shapeIdentifier = arg.substr(8);
            if (shapeIdentifier == "ellipse")
                shape.emplace(shapes::Ellipse{});
            else if (shapeIdentifier == "rectangle")
                shape.emplace(shapes::Rectangle{});
            else
                throw std::runtime_error("unknown value of the `--shape` parameter \""s
                                         .append(shapeIdentifier)
                                         .append("\""));
        }
        else if (arg.substr(0, 5) == "--dx=")
        {
            if (shapeWidth.has_value())
                throw std::runtime_error("`--dx` parameter occurs several times");

            float_type w;
            const std::string_view wStr = arg.substr(5);

            // looks like std::from_chars for floating-point are not supported by libc++ yet
            //if (auto [p, ec] = std::from_chars(wStr.data(), wStr.data() + wStr.length(), w); ec != std::errc{})
            //    throw std::system_error(std::make_error_code(ec), "failed to parse value of the `--dx` parameter");

            {
                char* parseEnd;
                if ( w = std::strtof(wStr.data(), &parseEnd); parseEnd != (wStr.data() + wStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--dx` parameter");
            }

            shapeWidth = w;
        }
        else if (arg.substr(0, 5) == "--dy=")
        {
            if (shapeHeight.has_value())
                throw std::runtime_error("`--dy` parameter occurs several times");

            float_type h;
            const std::string_view hStr = arg.substr(5);

            {
                char* parseEnd;
                if ( h = std::strtof(hStr.data(), &parseEnd); parseEnd != (hStr.data() + hStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--dy` parameter");
            }

            shapeHeight = h;
        }
        else if (arg.substr(0, 8) == "--scale=")
        {
            if (scaleFactor.has_value())
                throw std::runtime_error("`--scale` parameter occurs several times");

            float_type s;
            const std::string_view sStr = arg.substr(8);

            {
                char* parseEnd;
                if ( s = std::strtof(sStr.data(), &parseEnd); parseEnd != (sStr.data() + sStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--scale` parameter");
            }

            scaleFactor = s;
        }
        else if (arg == "--enable-antialiasing")
        {
            if (antialiasingIsEnabled)
                throw std::runtime_error("`--enable-antialiasing` parameter occurs several times");
            antialiasingIsEnabled = true;
        }
        else if (arg == "--enable-alphablending")
        {
            if (alphaBlendingIsEnabled)
                throw std::runtime_error("`--enable-alphablending` parameter occurs several times");
            alphaBlendingIsEnabled = true;
        }
        else if (arg.substr(0, 6) == "--shx=")
        {
            if (shapeCenterX.has_value())
                throw std::runtime_error("`--shx` parameter occurs several times");

            float_type shx;
            const std::string_view shxStr = arg.substr(6);

            {
                char* parseEnd;
                if ( shx = std::strtof(shxStr.data(), &parseEnd); parseEnd != (shxStr.data() + shxStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--shx` parameter");
            }

            shapeCenterX = shx;
        }
        else if (arg.substr(0, 6) == "--shy=")
        {
            if (shapeCenterY.has_value())
                throw std::runtime_error("`--shy` parameter occurs several times");

            float_type shy;
            const std::string_view shyStr = arg.substr(6);

            {
                char* parseEnd;
                if ( shy = std::strtof(shyStr.data(), &parseEnd); parseEnd != (shyStr.data() + shyStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--shy` parameter");
            }

            shapeCenterY = shy;
        }
        else if (arg.substr(0, 6) == "--imx=")
        {
            if (imageTopLeftX.has_value())
                throw std::runtime_error("`--imx` parameter occurs several times");

            int_type imx;
            const std::string_view imxStr = arg.substr(6);

            {
                char* parseEnd;
                if ( imx = std::strtof(imxStr.data(), &parseEnd); parseEnd != (imxStr.data() + imxStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--imx` parameter");
            }

            imageTopLeftX = imx;
        }
        else if (arg.substr(0, 6) == "--imy=")
        {
            if (imageTopLeftY.has_value())
                throw std::runtime_error("`--imy` parameter occurs several times");

            int_type imy;
            const std::string_view imyStr = arg.substr(6);

            {
                char* parseEnd;
                if ( imy = std::strtof(imyStr.data(), &parseEnd); parseEnd != (imyStr.data() + imyStr.length()) )
                    throw std::runtime_error("failed to parse value of the `--imy` parameter");
            }

            imageTopLeftY = imy;
        }
        else
            throw std::runtime_error("unknown parameter \""s
                                     .append(arg)
                                     .append("\""));
    }

    if (!outputFilePath.has_value())
        throw std::runtime_error("`--output` parameter was not set");
    if (!shape.has_value())
        throw std::runtime_error("`--shape` parameter was not set");
    if (!shapeWidth.has_value())
        throw std::runtime_error("`--dx` parameter was not set");
    if (!shapeHeight.has_value())
        throw std::runtime_error("`--dy` parameter was not set");

    CmdArgs result;
    result.image                  = mglass::Image::fromPNGFile(argv[argc - 1]);
    result.outputFilePath         = std::move(*outputFilePath);
    result.imageTopLeft.x         = imageTopLeftX.value_or(0);
    result.imageTopLeft.y         = imageTopLeftY.value_or(0);
    result.scaleFactor            = scaleFactor.value_or(2);
    result.antialiasingIsEnabled  = antialiasingIsEnabled;
    result.alphaBlendingIsEnabled = alphaBlendingIsEnabled;

    // TODO: implement
    //std::visit(
    //    [&shapeWidth, &shapeHeight, &shapeCenterX, &shapeCenterY, &result](auto& shape) {
    //        using ShapeT = std::remove_cv_t< std::remove_reference_t<decltype(shape)> >;
    //
    //        const auto centerX = shapeCenterX.value_or();
    //
    //        result.shape.emplace<ShapeT>();
    //    },
    //    *shape
    //);

    return result;
}

std::ostream& CmdArgs::writeHelp(std::ostream& stream)
{
    return stream
        << "Usage:\n"
           "\n"
           "  mglass-cli [options] <path-to-input-image>\n"
           "\n"
           "Options are:\n"
           "\n"
           "  --output=<path-to-file>  = Required. Specify a file to which the magnified image will be written.\n"
           "  --shape=<identifier>     = Required. Specify a shape of the magnifying glass. Supported \n"
           "                             identifiers are: `ellipse`, `rectangle`.\n"
           "  --dx=<value>             = Required. Specify the floating-point width of the magnifying glass.\n"
           "                             Value must be inside the range [0; +inf).\n"
           "  --dy=<value>             = Required. Specify the floating-point height of the magnifying glass.\n"
           "                             Value must be inside the range [0; +inf).\n"
           "  [--scale=<value>]        = Optional. Specify a floating-point scale factor of the magnifying glass.\n"
           "                             Value must be inside the range (0; +inf). Set to 2.0 by default.\n"
           "  [--enable-antialiasing]  = Optional. Enable anti-alising. Disabled by default.\n"
           "  [--enable-alphablending] = Optional. Enable alpha blending. Disabled by default.\n"
           "  [--shx=<value>]          = Optional. Specify a floating-point x-coordinate of the CENTER point of \n"
           "                             the magnifying glass in the Cartesian coordinate system.\n"
           "                             Set to the center of the loaded image by default.\n"
           "  [--shx=<value>]          = Optional. Specify a floating-point y-coordinate of the CENTER point of \n"
           "                             the magnifying glass in the Cartesian coordinate system.\n"
           "                             Set to the center of the loaded image by default.\n"
           "  [--imx=<value>]          = Optional. Specify an integer x-coordinate of the TOP LEFT point of \n"
           "                             the loaded image in the Cartesian coordinate system.\n"
           "                             Set to 0 by default.\n"
           "  [--imy=<value>]          = Optional. Specify an integer y-coordinate of the TOP LEFT point of \n"
           "                             the loaded image in the Cartesian coordinate system.\n"
           "                             Set to 0 by default.";
}


std::ostream& CmdArgs::dumpValues(std::ostream& stream) const
{
    // TODO: dump shape's width, height, center
    return stream << "CmdArgs[" << this << "] values:\n"
                     "\toutput file path: \"" << outputFilePath << "\"\n"
                     "\tshape: " << (std::holds_alternative<shapes::Ellipse>(shape)
                                    ? "shape"
                                    : std::holds_alternative<shapes::Rectangle>(shape)
                                    ? "rectangle"
                                    : "unknown") << "\n"
                     "\tantialiasing: " << (antialiasingIsEnabled ? "enabled" : "disabled") << "\n"
                     "\talphablending: " << (alphaBlendingIsEnabled ? "enabled" : "disabled") << "\n"
                     "\timage top left: (" << imageTopLeft.x << ", " << imageTopLeft.y << ")\n"
                     "\timage size: " << image.getWidth() << "x" << image.getHeight();
}
