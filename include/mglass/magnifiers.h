#ifndef MAGNIFYING_GLASS_MAGNIFIERS_H
#define MAGNIFYING_GLASS_MAGNIFIERS_H

#include "mglass/primitives.h"
#include "mglass/shape.h"
#include "mglass/image.h"
#include <cmath>                // std::floor
#include <cassert>              // assert


namespace mglass::magnifiers
{
    // imageDst will have size is getShapeIntegralBounds(shape).width x getShapeIntegralBounds(shape).height
    // if imageSrc and imageDst point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    template<typename ShapeImpl>
    void nearestNeighbor(
        const Shape<ShapeImpl>& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageTopLeft,
        Image& imageDst,
        bool enableAlphaBlending = false)
    {
        //TODO: fix cases when shape is not fully inside imageRect

        const auto shapeIntegralBounds = getShapeIntegralBounds(shape);

        imageDst.setSize(shapeIntegralBounds.width, shapeIntegralBounds.height);
        if ( (imageDst.getWidth() < 1) || (imageDst.getHeight() < 1) )
            return;

        imageDst.fill(ARGB::transparent());

        const IntegralRectArea imageBounds{imageTopLeft, imageSrc.getWidth(), imageSrc.getHeight()};

        shape.rasterizeOnto(
            imageBounds,
            [&shape, shapeIntegralBounds, scaleFactor, &imageSrc, imageTopLeft, &imageDst](const Point<mglass::int_type> rasterizePoint, float){
                auto srcPointFloat = shape.getPointAtScaled(scaleFactor, pointCast<mglass::float_type>(rasterizePoint));
                srcPointFloat.x = std::floor(srcPointFloat.x);
                srcPointFloat.y = std::floor(srcPointFloat.y);

                auto srcPointSigned = pointCast<mglass::int_type>(srcPointFloat);
                srcPointSigned.x -= imageTopLeft.x;
                srcPointSigned.y = imageTopLeft.y - srcPointSigned.y;

                if ((srcPointSigned.x < 0) || (srcPointSigned.y < 0))
                    return;

                const auto [srcX, srcY] = pointCast<mglass::size_type>(srcPointSigned);

                if ((srcX >= imageSrc.getWidth()) || (srcY >= imageSrc.getHeight()))
                    return;

                const auto srcPixel = imageSrc.getPixelAt(srcX, srcY);

                assert( (rasterizePoint.x >= shapeIntegralBounds.topLeft.x) );
                assert( (rasterizePoint.y <= shapeIntegralBounds.topLeft.y) );

                const auto dstX = static_cast<mglass::size_type>(rasterizePoint.x - shapeIntegralBounds.topLeft.x);
                const auto dstY = static_cast<mglass::size_type>(shapeIntegralBounds.topLeft.y - rasterizePoint.y);

                assert( (dstX < imageDst.getWidth()) );
                assert( (dstY < imageDst.getHeight()) );

                imageDst.setPixelAt(dstX, dstY, srcPixel);
        });
    }

    // imageDst will have size is getShapeIntegralBounds(shape).width x getShapeIntegralBounds(shape).height
    // if imageSrc and imageDst point to the same object, behaviour is undefineds
    //
    // TODO: more detailed documentation
    template<typename ShapeImpl>
    void nearestNeighborInterpolated(
        const Shape<ShapeImpl>& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageLeftTop,
        Image& imageDst,
        bool enableAlphaBlending = false);
}

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
