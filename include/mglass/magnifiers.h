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


        // TODO: abstract algorithms of interpolation (smth like 'interface Interpolator').
        struct InterpolationInfo
        {
            // Avoiding floating-point computations via replacing float_type by int_fast32_t.
            // Floating-point numbers like 123.456 will be stored as 1234560 of int_fast32_t through multiplying by 10^4.
            // It will allow to keep up to 4 decimal places (10^4).
            static constexpr std::int_fast32_t multiplier = 10000;
            static constexpr float_type multiplierF = multiplier;
            static constexpr std::int_fast32_t halfMultiplier = multiplier / 2;
            static constexpr std::int_fast32_t multiplierSqrt = 100;
            static constexpr float_type multiplierSqrtF = multiplierSqrt;
            static constexpr std::int_fast32_t halfMultiplierSqrt = multiplierSqrt / 2;

            static_assert((multiplierSqrt * multiplierSqrt == multiplier));

            // how much parts of the neighbors pixels should be taken into account (range [0; 1] * multiplier)
            // [0] is about (center.x - 1, center.y + 1)
            // [1] is about (center.x    , center.y + 1)
            // [2] is about (center.x + 1, center.y + 1)
            // [3] is about (center.x - 1, center.y    )
            // [4] is about (center.x    , center.y    )
            // [5] is about (center.x + 1, center.y    )
            // [6] is about (center.x - 1, center.y - 1)
            // [7] is about (center.x    , center.y - 1)
            // [8] is about (center.x + 1, center.y - 1)
            std::int_fast32_t neighborsParts[9];


            // `pixelBottomLeft` must be { std::floor(`point`.x), std::floor(`point`.y) }
            static InterpolationInfo calculateFor(const Point<float_type> point, const Point<float_type> pixelBottomLeft)
            {
                const auto pointInt = pointCast<std::int_fast32_t>(
                    Point{point.x * multiplierSqrtF, point.y * multiplierSqrtF}
                );
                const auto pixelBottomLeftInt = pointCast<std::int_fast32_t>(
                    Point{pixelBottomLeft.x * multiplierSqrtF, pixelBottomLeft.y * multiplierSqrtF}
                );

                const Point<std::int_fast32_t> pixelTopRight { pixelBottomLeftInt.x + multiplierSqrt, pixelBottomLeftInt.y + multiplierSqrt };

                // let's make "pixel" around the `point`
                const Point<std::int_fast32_t> fakePixelBottomLeft { pointInt.x - halfMultiplierSqrt, pointInt.y - halfMultiplierSqrt };
                const Point<std::int_fast32_t> fakePixelTopRight { pointInt.x + halfMultiplierSqrt, pointInt.y + halfMultiplierSqrt };

                const auto beforeLeftLength = (std::max<std::int_fast32_t>)(pixelBottomLeftInt.x - fakePixelBottomLeft.x, 0);
                const auto beforeBottomLength = (std::max<std::int_fast32_t>)(pixelBottomLeftInt.y - fakePixelBottomLeft.y, 0);

                const auto afterRightLength = (std::max<std::int_fast32_t>)(fakePixelTopRight.x - pixelTopRight.x, 0);
                const auto afterTopLength = (std::max<std::int_fast32_t>)(fakePixelTopRight.y - pixelTopRight.y, 0);

                const std::int_fast32_t insideXLength = multiplierSqrt - std::abs(fakePixelTopRight.x - pixelTopRight.x);
                const std::int_fast32_t insideYLength = multiplierSqrt - std::abs(fakePixelTopRight.y - pixelTopRight.y);

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
                struct ARGB128
                {
                    std::int_fast32_t a;
                    std::int_fast32_t r;
                    std::int_fast32_t g;
                    std::int_fast32_t b;

                    ARGB128& operator=(ARGB c) noexcept
                    {
                        a = c.a;
                        r = c.r;
                        g = c.g;
                        b = c.b;
                        return *this;
                    }
                };

                std::array<ARGB128, 9> srcPixels; // 4th is center

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

                //std::int_fast32_t a32 = 0;
                std::int_fast32_t r32 = 0;
                std::int_fast32_t g32 = 0;
                std::int_fast32_t b32 = 0;

                for (unsigned i = 0; i < 9; ++i)
                {
                    r32 += neighborsParts[i] * srcPixels[i].r;
                    g32 += neighborsParts[i] * srcPixels[i].g;
                    b32 += neighborsParts[i] * srcPixels[i].b;
                }

                r32 = (r32 / multiplier) + ( (r32 % multiplier) < halfMultiplier ? 0 : 1 );
                g32 = (g32 / multiplier) + ( (g32 % multiplier) < halfMultiplier ? 0 : 1 );
                b32 = (b32 / multiplier) + ( (b32 % multiplier) < halfMultiplier ? 0 : 1 );

                ARGB result{255, static_cast<std::uint8_t>(r32), static_cast<std::uint8_t>(g32), static_cast<std::uint8_t>(b32)};
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
