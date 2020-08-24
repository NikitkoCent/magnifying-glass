#ifndef MAGNIFYING_GLASS_MAGNIFIERS_H
#define MAGNIFYING_GLASS_MAGNIFIERS_H

#include "mglass/primitives.h"
#include "mglass/shape.h"
#include "mglass/image.h"
#include <cmath>                // std::floor
#include <cassert>              // assert

#include <algorithm>            // std::min
#include <array>                // std::array


namespace mglass::magnifiers
{
    namespace detail
    {
        // if `point` is not inside the `area` returns a closest point is inside it
        // otherwise returns `point`
        Point<float_type> restrictPointBy(IntegralRectArea area, Point<float_type> point);

        // returns a new end which distance to the `start` is scaleFactor * distance(`start`, `end`)
        Point<float_type> scaleVectorBy(float_type scaleFactor, Point<float_type> start, Point<float_type> end) noexcept;


        struct InterpolationInfo
        {
            // how much parts of the neighbors pixels should be taken into account (range [0; 1])
            // [0] is about (center.x - 1, center.y + 1)
            // [1] is about (center.x    , center.y + 1)
            // [2] is about (center.x + 1, center.y + 1)
            // [3] is about (center.x - 1, center.y    )
            // [4] is about (center.x    , center.y    )
            // [5] is about (center.x + 1, center.y    )
            // [6] is about (center.x - 1, center.y - 1)
            // [7] is about (center.x    , center.y - 1)
            // [8] is about (center.x + 1, center.y - 1)
            float_type neighborsParts[9];


            // `pixelBottomLeft` must be { std::floor(`point`.x), std::floor(`point`.y) }
            static InterpolationInfo calculateFor(const Point<float_type> point, const Point<float_type> pixelBottomLeft)
            {
                const Point<float_type> pixelTopRight { pixelBottomLeft.x + 1, pixelBottomLeft.y + 1 };

                // let's make "pixel" around the `point`
                const Point<float_type> fakePixelBottomLeft { point.x - 0.5f, point.y - 0.5f };
                const Point<float_type> fakePixelTopRight { point.x + 0.5f, point.y + 0.5f };

                const float_type beforeLeftLength = (std::max)(pixelBottomLeft.x - fakePixelBottomLeft.x, 0.f);
                const float_type beforeBottomLength = (std::max)(pixelBottomLeft.y - fakePixelBottomLeft.y, 0.f);

                const float_type afterRightLength = (std::max)(fakePixelTopRight.x - pixelTopRight.x, 0.f);
                const float_type afterTopLength = (std::max)(fakePixelTopRight.y - pixelTopRight.y, 0.f);

                const float_type insideXLength = 1 - std::abs(fakePixelTopRight.x - pixelTopRight.x);
                const float_type insideYLength = 1 - std::abs(fakePixelTopRight.y - pixelTopRight.y);

                InterpolationInfo result{};

                result.neighborsParts[0] = beforeLeftLength * afterTopLength;
                result.neighborsParts[1] = insideXLength    * afterTopLength;
                result.neighborsParts[2] = afterRightLength * afterTopLength;
                result.neighborsParts[3] = beforeLeftLength * insideYLength;
                result.neighborsParts[4] = insideXLength    * insideYLength;
                result.neighborsParts[5] = afterRightLength * insideYLength;
                result.neighborsParts[6] = beforeLeftLength * beforeBottomLength;
                result.neighborsParts[7] = insideXLength    * beforeBottomLength;
                result.neighborsParts[8] = afterRightLength * beforeBottomLength;

                return result;
            }

            ARGB applyTo(const Point<size_type> pixelPos, const Image& imageSrc) const
            {
                std::array<ARGB, 9> srcPixels; // 4th is center

                const auto [centerX, centerY] = pixelPos;
                const auto minX = (std::max<size_type>)(centerX, 1) - 1;
                const auto maxX = (std::min<size_type>)(centerX + 2, imageSrc.getWidth()) - 1;
                const auto minY = (std::max<size_type>)(centerY, 1) - 1;
                const auto maxY = (std::min<size_type>)(centerY + 2, imageSrc.getHeight()) - 1;

                srcPixels[0] = imageSrc.getPixelAt(minX,    minY);
                srcPixels[1] = imageSrc.getPixelAt(centerX, minY);
                srcPixels[2] = imageSrc.getPixelAt(maxX,    minY);
                srcPixels[3] = imageSrc.getPixelAt(minX,    centerY);
                srcPixels[4] = imageSrc.getPixelAt(centerX, centerY);
                srcPixels[5] = imageSrc.getPixelAt(maxX,    centerY);
                srcPixels[6] = imageSrc.getPixelAt(minX,    maxY);
                srcPixels[7] = imageSrc.getPixelAt(centerX, maxY);
                srcPixels[8] = imageSrc.getPixelAt(maxX,    maxY);

                //float_type fA = 0;
                float_type fR = 0;
                float_type fG = 0;
                float_type fB = 0;

                for (unsigned i = 0; i < 9; ++i)
                {
                    fR += neighborsParts[i] * static_cast<float_type>(srcPixels[i].r);
                    fG += neighborsParts[i] * static_cast<float_type>(srcPixels[i].g);
                    fB += neighborsParts[i] * static_cast<float_type>(srcPixels[i].b);
                }

                ARGB result{255, static_cast<std::uint8_t>(fR), static_cast<std::uint8_t>(fG), static_cast<std::uint8_t>(fB)};
                return result;
            }
        };


        template<bool EnableAlphaBlending, bool EnableInterpolation>
        struct RasterizationConsumer
        {
            const float_type scaleFactor;
            const Image& imageSrc;
            const IntegralRectArea imageSrcBounds;
            Image& imageDst;
            const Point<float_type> scaleCenter;
            const IntegralRectArea shapeIntegralBounds;


            void operator()(const Point<int_type> rasterizePoint, [[maybe_unused]] float pixelInsidePart) const
            {
                const Point<float_type> srcPointFloat = detail::scaleVectorBy(
                    scaleFactor,
                    scaleCenter,
                    pointCast<float_type>(rasterizePoint)
                );

                const Point<float_type> pixelStartFloat{ std::floor(srcPointFloat.x), std::floor(srcPointFloat.y) };

                auto pixelStartSigned = pointCast<int_type>(pixelStartFloat);
                pixelStartSigned.x -= imageSrcBounds.topLeft.x;
                pixelStartSigned.y = imageSrcBounds.topLeft.y - pixelStartSigned.y;

                if ((pixelStartSigned.x < 0) || (pixelStartSigned.y < 0))
                    return;

                const auto pixelStart = pointCast<size_type>(pixelStartSigned);

                if ((pixelStart.x >= imageSrc.getWidth()) || (pixelStart.y >= imageSrc.getHeight()))
                    return;

                const auto srcPixel = obtainSrcPixel(pixelStart, pixelStartFloat, srcPointFloat);

                assert( (rasterizePoint.x >= shapeIntegralBounds.topLeft.x) );
                assert( (rasterizePoint.y <= shapeIntegralBounds.topLeft.y) );

                const auto dstX = static_cast<size_type>(rasterizePoint.x - shapeIntegralBounds.topLeft.x);
                const auto dstY = static_cast<size_type>(shapeIntegralBounds.topLeft.y - rasterizePoint.y);

                assert( (dstX < imageDst.getWidth()) );
                assert( (dstY < imageDst.getHeight()) );

                imageDst.setPixelAt(dstX, dstY, srcPixel);
            }

        private:
            [[nodiscard]] ARGB obtainSrcPixel(
                const Point<size_type> pixelPos,
                [[maybe_unused]] const Point<float_type> pixelPosFloat,
                [[maybe_unused]] const Point<float_type> point) const
            {
                if constexpr (EnableInterpolation)
                {
                    return InterpolationInfo::calculateFor(point, pixelPosFloat).applyTo(pixelPos, imageSrc);
                }
                else
                {
                    return imageSrc.getPixelAt(pixelPos.x, pixelPos.y);
                }
            }
        };


        template<bool EnableAlphaBlending, bool EnableInterpolating, typename ShapeImpl>
        void nearestNeighbor(
            const Shape<ShapeImpl>& shape,
            float_type scaleFactor,
            const Image& imageSrc,
            Point<int_type> imageTopLeft,
            Image& imageDst)
        {
            const IntegralRectArea shapeIntegralBounds = getShapeIntegralBounds(shape);

            imageDst.setSize(shapeIntegralBounds.width, shapeIntegralBounds.height);
            if ( (imageDst.getWidth() < 1) || (imageDst.getHeight() < 1) )
                return;
            imageDst.fill(ARGB::transparent());

            const IntegralRectArea imageSrcBounds{imageTopLeft, imageSrc.getWidth(), imageSrc.getHeight()};
            const auto scaleCenter = detail::restrictPointBy(imageSrcBounds, shapeIntegralBounds.getCenter());
            const float_type srcScaleFactor = 1 / scaleFactor;

            shape.rasterizeOnto(
                imageSrcBounds,
                RasterizationConsumer<EnableAlphaBlending, EnableInterpolating>{
                    srcScaleFactor,
                    imageSrc,
                    imageSrcBounds,
                    imageDst,
                    scaleCenter,
                    shapeIntegralBounds
                }
            );
        }
    } // namespace detail


    // `imageDst` will have size is getShapeIntegralBounds(`shape`).width x getShapeIntegralBounds(`shape`).height
    // if `imageSrc` and `imageDst` point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighbor(
        const Shape<ShapeImpl>& shape,
        const float_type scaleFactor,
        const Image& imageSrc,
        const Point<int_type> imageTopLeft,
        Image& imageDst,
        const bool enableAlphaBlending = false)
    {
        if (enableAlphaBlending)
            return detail::nearestNeighbor<true, false>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
        else
            return detail::nearestNeighbor<false, false>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
    }

    // `i`mageDst` will have size is getShapeIntegralBounds(`shape`).width x getShapeIntegralBounds(`shape`).height
    // if `imageSrc` and `imageDst` point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    // TODO: alpha blending
    template<typename ShapeImpl>
    void nearestNeighborInterpolated(
        const Shape<ShapeImpl>& shape,
        const float_type scaleFactor,
        const Image& imageSrc,
        const Point<int_type> imageTopLeft,
        Image& imageDst,
        const bool enableAlphaBlending = false)
    {
        if (enableAlphaBlending)
            return detail::nearestNeighbor<true, true>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
        else
            return detail::nearestNeighbor<false, true>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
    }
} // namespace mglass::magnifiers

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
