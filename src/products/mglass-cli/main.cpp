#include "mglass/mglass.h"
#include "mglass/shapes.h"
#include "mglass/magnifiers.h"
#include <string>

int main(int argc, char *argv[])
{
    const auto lenaSrc = mglass::Image::fromPNGFile("/Users/nikitaprovotorov/lena.png");
    const auto pixelsSrc = mglass::Image::fromPNGFile("/Users/nikitaprovotorov/Untitled.png");

    {
        mglass::Image lenaCopy(lenaSrc.getWidth(), lenaSrc.getHeight());
        for (mglass::size_type y = 0; y < lenaSrc.getHeight(); ++y)
        {
            for (mglass::size_type x = 0; x < lenaSrc.getWidth(); ++x)
            {
                lenaCopy.setPixelAt(x, y, lenaSrc.getPixelAt(x, y));
            }
        }
        lenaCopy.saveToPNGFile("/Users/nikitaprovotorov/lena2.png");
    }

    mglass::shapes::Ellipse e({200, -100}, 400, 300);

    mglass::Image dst;

    //for (mglass::size_type y = 0; y < lenaSrc.getHeight(); y += 10)
    //{
    //    const mglass::float_type yFloat = -static_cast<mglass::float_type>(y);
    //
    //    for (mglass::size_type x = 0; x < lenaSrc.getWidth(); x += 10)
    //    {
    //        mglass::float_type xFloat = x;
    //
    //        mglass::shapes::Ellipse e({xFloat, yFloat}, 400, 300);
    //
    //        mglass::magnifiers::nearestNeighbor(e, 2, lenaSrc, {0, 0}, dst);
    //
    //        const auto dstPath = std::string("/Users/nikitaprovotorov/mg/lena")
    //                             + std::to_string(x)
    //                             + "_"
    //                             + std::to_string(y)
    //                             + ".png";
    //
    //        dst.saveToPNGFile(dstPath);
    //    }
    //}

    for (mglass::size_type y = 0; y < pixelsSrc.getHeight(); ++y)
    {
        const mglass::float_type yFloat = -static_cast<mglass::float_type>(y);

        for (mglass::size_type x = 0; x < pixelsSrc.getWidth(); ++x)
        {
            mglass::float_type xFloat = x;

            mglass::shapes::Ellipse e({xFloat, yFloat}, 400, 300);

            mglass::magnifiers::nearestNeighbor(e, 2, pixelsSrc, {0, 0}, dst);

            const auto dstPath = std::string("/Users/nikitaprovotorov/mgpixels/pixels")
                                 + std::to_string(y)
                                 + "_"
                                 + std::to_string(x)
                                 + ".png";

            dst.saveToPNGFile(dstPath);
        }
    }

    return 0;
}
