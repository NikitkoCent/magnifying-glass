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
        // if `point` is not inside the `area` returns a closest point is inside it
        // otherwise returns `point`
        [[nodiscard]] Point<float_type> restrictPointBy(
            IntegralRectArea area,
            Point<float_type> point);

        // returns a new end which distance to the `start` is scaleFactor * distance(`start`, `end`)
        [[nodiscard]] Point<float_type> scaleVectorBy(
            float_type scaleFactor,
            Point<float_type> start,
            Point<float_type> end) noexcept;


        // This class encapsulates data and methods required for implementing the anti-aliasing effect
        // TODO: abstract algorithms of interpolation (smth like 'interface Interpolator').
        struct InterpolationInfo final
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


            // `pixelBottomLeft` must be equal to { std::floor(`point`.x), std::floor(`point`.y) }
            [[nodiscard]] static InterpolationInfo calculateFor(
                Point<float_type> point,
                Point<float_type> pixelBottomLeft) noexcept;

            // applies `neighborsParts` to the pixel at `imageSrc`[`pixelPos`]
            [[nodiscard]] ARGB applyTo(Point<size_type> pixelPos, const Image& imageSrc) const;
        };


        // This functor receives coordinates of the point rasterized by a shape
        //  and transforms its coordinates to coordinates on the `imageSrc`.
        // Optionally performs alpha-blending and anti-aliasing according to template flags.
        template<bool EnableAlphaBlending, bool EnableInterpolation>
        struct RasterizationConsumer
        {
            const float_type scaleFactor;
            const Image& imageSrc;
            const IntegralRectArea imageSrcBounds;
            Image& imageDst;
            const Point<float_type> scaleCenter;
            const IntegralRectArea shapeIntegralBounds;


            template<typename Impl>
            void operator()(const RasterizationContextBase<Impl>& rastrCtx) const
            {
                const auto rasterizePoint = rastrCtx.getRasterizedPoint();

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

                const auto srcPixel = obtainSrcPixel(pixelStart, pixelStartFloat, srcPointFloat, rastrCtx);

                assert( (rasterizePoint.x >= shapeIntegralBounds.topLeft.x) );
                assert( (rasterizePoint.y <= shapeIntegralBounds.topLeft.y) );

                const auto dstX = static_cast<size_type>(rasterizePoint.x - shapeIntegralBounds.topLeft.x);
                const auto dstY = static_cast<size_type>(shapeIntegralBounds.topLeft.y - rasterizePoint.y);

                assert( (dstX < imageDst.getWidth()) );
                assert( (dstY < imageDst.getHeight()) );

                imageDst.setPixelAt(dstX, dstY, srcPixel);
            }

        private:
            template<typename Impl>
            [[nodiscard]] ARGB obtainSrcPixel(
                const Point<size_type> pixelPos,
                [[maybe_unused]] const Point<float_type> pixelPosFloat,
                [[maybe_unused]] const Point<float_type> point,
                [[maybe_unused]] const RasterizationContextBase<Impl>& rastrCtx) const
            {
                ARGB result;

                if constexpr (EnableInterpolation)
                {
                    result = InterpolationInfo::calculateFor(point, pixelPosFloat).applyTo(pixelPos, imageSrc);
                }
                else
                {
                    result = imageSrc.getPixelAt(pixelPos.x, pixelPos.y);
                }

                if constexpr (EnableAlphaBlending)
                {
                    result.a = static_cast<std::uint8_t>(static_cast<float_type>(result.a) * rastrCtx.getPixelDensity());
                }

                return result;
            }
        };


        template<bool EnableAlphaBlending, bool EnableInterpolating, typename ShapeImpl, typename RastrCtx>
        void nearestNeighbor(
            const Shape<ShapeImpl, RastrCtx>& shape,
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
    template<typename ShapeImpl, typename RastrCtx>
    void nearestNeighbor(
        const Shape<ShapeImpl, RastrCtx>& shape,
        const float_type scaleFactor,
        const Image& imageSrc,
        const Point<int_type> imageTopLeft,
        Image& imageDst,
        const bool enableAlphaBlending = false)
    {
        if (enableAlphaBlending)
            detail::nearestNeighbor<true, false>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
        else
            detail::nearestNeighbor<false, false>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
    }

    // `imageDst` will have size is getShapeIntegralBounds(`shape`).width x getShapeIntegralBounds(`shape`).height
    // if `imageSrc` and `imageDst` point to the same object, behaviour is undefined
    //
    // TODO: more detailed documentation
    template<typename ShapeImpl, typename RastrCtx>
    void nearestNeighborInterpolated(
        const Shape<ShapeImpl, RastrCtx>& shape,
        const float_type scaleFactor,
        const Image& imageSrc,
        const Point<int_type> imageTopLeft,
        Image& imageDst,
        const bool enableAlphaBlending = false)
    {
        if (enableAlphaBlending)
            detail::nearestNeighbor<true, true>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
        else
            detail::nearestNeighbor<false, true>(shape, scaleFactor, imageSrc, imageTopLeft, imageDst);
    }
} // namespace mglass::magnifiers

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
