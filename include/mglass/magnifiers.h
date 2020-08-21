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
        Point<float_type> getAreaCenter(const IntegralRectArea area) noexcept
        {
            auto result = pointCast<float_type>(area.topLeft);

            if ((area.width < 1) || (area.height < 1))
                return result;

            const auto halfWidth = static_cast<float_type>(area.width - 1) / 2;
            const auto halfHeight = static_cast<float_type>(area.height - 1) / 2;

            result.x += halfWidth;
            result.y -= halfHeight;

            return result;
        }

        // returns a new end which distance to the `start` is scaleFactor * distance(`start`, `end`)
        Point<float_type> scaleVectorBy(const float_type scaleFactor, const Point<float_type> start, Point<float_type> end)
        {
            end.x = start.x + (end.x - start.x) * scaleFactor;
            end.y = start.y + (end.y - start.y) * scaleFactor;
            return end;
        }
    } // namespace detail


    // imageDst will have size is getShapeIntegralBounds(shape).width x getShapeIntegralBounds(shape).height
    // if imageSrc and imageDst point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighbor(
        const Shape<ShapeImpl>& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageTopLeft,
        Image& imageDst,
        bool enableAlphaBlending = false)
    {
        const IntegralRectArea shapeIntegralBounds = getShapeIntegralBounds(shape);
        const IntegralRectArea imageSrcBounds{imageTopLeft, imageSrc.getWidth(), imageSrc.getHeight()};
        const IntegralRectArea intersectionBounds = shapeIntegralBounds.getIntersectionWith(imageSrcBounds);

        imageDst.setSize(intersectionBounds.width, intersectionBounds.height);
        if ( (imageDst.getWidth() < 1) || (imageDst.getHeight() < 1) )
            return;
        imageDst.fill(ARGB::transparent());

        const Point<float_type> boundsCenter = detail::getAreaCenter(intersectionBounds);
        const mglass::float_type srcScaleFactor = 1 / scaleFactor;

        shape.rasterizeOnto(
            imageSrcBounds,
            [srcScaleFactor, &imageSrc, imageSrcBounds, &imageDst, intersectionBounds, boundsCenter](const Point<mglass::int_type> rasterizePoint, float){
                assert( (rasterizePoint.x >= intersectionBounds.topLeft.x) );
                assert( (rasterizePoint.y <= intersectionBounds.topLeft.y) );

                const auto dstX = static_cast<mglass::size_type>(rasterizePoint.x - intersectionBounds.topLeft.x);
                const auto dstY = static_cast<mglass::size_type>(intersectionBounds.topLeft.y - rasterizePoint.y);

                assert( (dstX < imageDst.getWidth()) );
                assert( (dstY < imageDst.getHeight()) );

                auto srcPointFloat = detail::scaleVectorBy(
                    srcScaleFactor,
                    boundsCenter,
                    pointCast<mglass::float_type>(rasterizePoint)
                );
                srcPointFloat.x = std::floor(srcPointFloat.x);
                srcPointFloat.y = std::floor(srcPointFloat.y);

                auto srcPointSigned = pointCast<mglass::int_type>(srcPointFloat);
                srcPointSigned.x -= imageSrcBounds.topLeft.x;
                srcPointSigned.y = imageSrcBounds.topLeft.y - srcPointSigned.y;

                if ((srcPointSigned.x < 0) || (srcPointSigned.y < 0))
                    return;

                const auto [srcX, srcY] = pointCast<mglass::size_type>(srcPointSigned);

                if ((srcX >= imageSrc.getWidth()) || (srcY >= imageSrc.getHeight()))
                    return;

                const auto srcPixel = imageSrc.getPixelAt(srcX, srcY);

                imageDst.setPixelAt(dstX, dstY, srcPixel);
        });
    }

    // imageDst will have size is getShapeIntegralBounds(shape).width x getShapeIntegralBounds(shape).height
    // if imageSrc and imageDst point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighborInterpolated(
        const Shape<ShapeImpl>& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageLeftTop,
        Image& imageDst,
        bool enableAlphaBlending = false)
    {
        // TODO: implement
    }
}

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
