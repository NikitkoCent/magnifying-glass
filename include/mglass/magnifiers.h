#ifndef MAGNIFYING_GLASS_MAGNIFIERS_H
#define MAGNIFYING_GLASS_MAGNIFIERS_H

#include "mglass/primitives.h"
#include "mglass/shape.h"
#include "mglass/image.h"
#include <cmath>                // std::floor
#include <cassert>              // assert


namespace mglass::magnifiers
{
    namespace detail
    {
        Point<float_type> getAreaCenter(IntegralRectArea area) noexcept;

        Point<int_type> getAreaRightBottom(IntegralRectArea area) noexcept;

        // if `point` is not inside the `area` returns a closest point is inside it
        // otherwise returns `point`
        Point<float_type> restrictPointBy(IntegralRectArea area, Point<float_type> point);

        // returns a new end which distance to the `start` is scaleFactor * distance(`start`, `end`)
        Point<float_type> scaleVectorBy(float_type scaleFactor, Point<float_type> start, Point<float_type> end) noexcept;
    } // namespace detail


    // `imageDst` will have size is getShapeIntegralBounds(`shape`).width x getShapeIntegralBounds(`shape`).height
    // if `imageSrc` and `imageDst` point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighbor(
        const Shape<ShapeImpl>& shape,
        float_type scaleFactor,
        const Image& imageSrc,
        Point<int_type> imageTopLeft,
        Image& imageDst,
        bool enableAlphaBlending = false)
    {
        const IntegralRectArea shapeIntegralBounds = getShapeIntegralBounds(shape);

        imageDst.setSize(shapeIntegralBounds.width, shapeIntegralBounds.height);
        if ( (imageDst.getWidth() < 1) || (imageDst.getHeight() < 1) )
            return;
        imageDst.fill(ARGB::transparent());

        const IntegralRectArea imageSrcBounds{imageTopLeft, imageSrc.getWidth(), imageSrc.getHeight()};
        const auto scaleCenter = detail::restrictPointBy(imageSrcBounds, detail::getAreaCenter(shapeIntegralBounds));
        const float_type srcScaleFactor = 1 / scaleFactor;

        shape.rasterizeOnto(
            imageSrcBounds,
            [srcScaleFactor, &imageSrc, imageSrcBounds, &imageDst, scaleCenter, shapeIntegralBounds](const Point<int_type> rasterizePoint, float){
                auto srcPointFloat = detail::scaleVectorBy(
                    srcScaleFactor,
                    scaleCenter,
                    pointCast<float_type>(rasterizePoint)
                );
                srcPointFloat.x = std::floor(srcPointFloat.x);
                srcPointFloat.y = std::floor(srcPointFloat.y);

                auto srcPointSigned = pointCast<int_type>(srcPointFloat);
                srcPointSigned.x -= imageSrcBounds.topLeft.x;
                srcPointSigned.y = imageSrcBounds.topLeft.y - srcPointSigned.y;

                if ((srcPointSigned.x < 0) || (srcPointSigned.y < 0))
                    return;

                const auto [srcX, srcY] = pointCast<size_type>(srcPointSigned);

                if ((srcX >= imageSrc.getWidth()) || (srcY >= imageSrc.getHeight()))
                    return;

                const auto srcPixel = imageSrc.getPixelAt(srcX, srcY);

                assert( (rasterizePoint.x >= shapeIntegralBounds.topLeft.x) );
                assert( (rasterizePoint.y <= shapeIntegralBounds.topLeft.y) );

                const auto dstX = static_cast<size_type>(rasterizePoint.x - shapeIntegralBounds.topLeft.x);
                const auto dstY = static_cast<size_type>(shapeIntegralBounds.topLeft.y - rasterizePoint.y);

                assert( (dstX < imageDst.getWidth()) );
                assert( (dstY < imageDst.getHeight()) );

                imageDst.setPixelAt(dstX, dstY, srcPixel);
        });
    }

    // `i`mageDst` will have size is getShapeIntegralBounds(`shape`).width x getShapeIntegralBounds(`shape`).height
    // if `imageSrc` and `imageDst` point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighborInterpolated(
        const Shape<ShapeImpl>& shape,
        float_type scaleFactor,
        const Image& imageSrc,
        Point<int_type> imageLeftTop,
        Image& imageDst,
        bool enableAlphaBlending = false)
    {
        // TODO: implement
    }
} // namespace mglass::magnifiers

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
