#include "mglass/magnifiers.h"
#include <cmath>                // std::ceil, std::floor
#include <cassert>              // assert


namespace mglass::magnifiers
{
    void nearestNeighbor(
        const Shape& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageTopLeft,
        Image& imageDst)
    {
        const auto shapeRect = getShapeRect(shape);

        imageDst.setSize(shapeRect.width, shapeRect.height);
        imageDst.fill({0, 0, 0, 0});

        const Rect imageRect{imageTopLeft, shapeRect.width, shapeRect.height};

        shape.rasterizeOnto(
            imageRect,
            [&shape, shapeRect, scaleFactor, &imageSrc, imageTopLeft, &imageDst](const Point<mglass::int_type> rasterizePoint, float){
                auto srcPointFloat = shape.getPointAtScaled(scaleFactor, pointCast<mglass::float_type>(rasterizePoint));
                srcPointFloat.x = std::floor(srcPointFloat.x);
                srcPointFloat.y = std::floor(srcPointFloat.y);

                auto srcPointSigned = pointCast<mglass::int_type>(srcPointFloat);
                srcPointSigned.x -= imageTopLeft.x;
                srcPointSigned.y -= imageTopLeft.y;

                assert( ((srcPointSigned.x >= 0) && (srcPointSigned.y >= 0)) );

                const auto [srcX, srcY] = pointCast<mglass::size_type>(srcPointSigned);
                const auto srcPixel = imageSrc.getPixelAt(srcX, srcY);

                assert( (rasterizePoint.x >= shapeRect.topLeft.x) );
                assert( (rasterizePoint.y <= shapeRect.topLeft.y) );

                const auto dstX = static_cast<mglass::size_type>(rasterizePoint.x - shapeRect.topLeft.x);
                const auto dstY = static_cast<mglass::size_type>(shapeRect.topLeft.y - rasterizePoint.y);

                imageDst.setPixelAt(dstX, dstY, srcPixel);
        });
    }
} // namespace mglass::magnifiers
