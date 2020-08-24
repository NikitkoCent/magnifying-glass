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
            // Avoiding floating-point computations via replacing float_type by uint_fast32_t.
            // Floating-point numbers like 123.456 will be stored as 123456 of uint_fast32_t through multiplying by 10^5.
            // It will allow to keep up to 5 decimal places (10^5).
            static constexpr std::uint_fast32_t multiplier = 100000;
            static constexpr float_type multiplierF = multiplier;
            static constexpr std::uint_fast32_t halfMultiplier = multiplier / 2;

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
            std::uint_fast32_t neighborsParts[9];


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

                result.neighborsParts[0] = static_cast<std::uint_fast32_t>(beforeLeftLength * afterTopLength * multiplierF);
                result.neighborsParts[1] = static_cast<std::uint_fast32_t>(insideXLength    * afterTopLength * multiplierF);
                result.neighborsParts[2] = static_cast<std::uint_fast32_t>(afterRightLength * afterTopLength * multiplierF);
                result.neighborsParts[3] = static_cast<std::uint_fast32_t>(beforeLeftLength * insideYLength * multiplierF);
                result.neighborsParts[4] = static_cast<std::uint_fast32_t>(insideXLength    * insideYLength * multiplierF);
                result.neighborsParts[5] = static_cast<std::uint_fast32_t>(afterRightLength * insideYLength * multiplierF);
                result.neighborsParts[6] = static_cast<std::uint_fast32_t>(beforeLeftLength * beforeBottomLength * multiplierF);
                result.neighborsParts[7] = static_cast<std::uint_fast32_t>(insideXLength    * beforeBottomLength * multiplierF);
                result.neighborsParts[8] = static_cast<std::uint_fast32_t>(afterRightLength * beforeBottomLength * multiplierF);

                return result;
            }

            ARGB applyTo(const Point<size_type> pixelPos, const Image& imageSrc) const
            {
                struct ARGB128
                {
                    std::uint_fast32_t a;
                    std::uint_fast32_t r;
                    std::uint_fast32_t g;
                    std::uint_fast32_t b;

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

                //std::uint_fast32_t fA = 0;
                std::uint_fast32_t r32 = 0;
                std::uint_fast32_t g32 = 0;
                std::uint_fast32_t b32 = 0;

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
