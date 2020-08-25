#include "mglass/magnifiers.h"
#include <algorithm>            // std::min, std::max


namespace mglass::magnifiers::detail
{
    Point<float_type> restrictPointBy(const IntegralRectArea area, Point<float_type> point)
    {
        const auto topLeftFloat = pointCast<float_type>(area.topLeft);
        const auto bottomRightFloat = pointCast<float_type>(area.getBottomRight());

        point.x = (std::min)( (std::max)(point.x, topLeftFloat.x), bottomRightFloat.x );
        point.y = (std::min)( (std::max)(point.y, bottomRightFloat.y), topLeftFloat.y );

        return point;
    }

    Point<float_type> scaleVectorBy(
        const float_type scaleFactor,
        const Point<float_type> start,
        Point<float_type> end) noexcept
    {
        end.x = start.x + (end.x - start.x) * scaleFactor;
        end.y = start.y + (end.y - start.y) * scaleFactor;

        return end;
    }


    // ================================================================================================================
    //  InterpolationInfo
    // ================================================================================================================

    // `pixelBottomLeft` must be equal to { std::floor(`point`.x), std::floor(`point`.y) }
    InterpolationInfo InterpolationInfo::calculateFor(
        const Point<float_type> point,
        const Point<float_type> pixelBottomLeft) noexcept
    {
        // Implementation description:
        //   Algorithm builds fake pixel (i.e. unit square) around `point`
        //   and then calculates
        //
        // Example:
        //  +---------------+---------------+---------------+
        //  |               |               |               |
        //  |               |               |               |
        //  |               |               |               |
        //  |      [0]      |      [1]      |      [2]      |
        //  |               |               |               |
        //  |          +---------------+    |               |
        //  |          |    |          |    |               |
        //  +----------|---------------|--------------------+
        //  |          |    |          |    |               |
        //  |          |    |  X       |    |               |
        //  |          |    |          |    |               |
        //  |      [3] |    |      [4] |    |      [5]      |
        //  |          |    |          |    |               |
        //  |          +---------------+    |               |
        //  |               |               |               |
        //  +-----------------------------------------------+
        //  |               |               |               |
        //  |               |               |               |
        //  |               |               |               |
        //  |      [7]      |      [8]      |      [9]      |
        //  |               |               |               |
        //  |               |               |               |
        //  |               |               |               |
        //  +---------------+---------------+---------------+
        //
        //  X is the `point`.
        //  The fake pixel intersects with the top-left pixel ([0]) with an area about 0.3 * 0.25 = 0.075
        //   so neighborsParts[0] := 0.075
        //  The intersection with center pixel ([4]) has an area about 0.7 * 0.75 = 0.525
        //   so neighborsParts[4] := 0.525
        //  The fake pixel does not intersect with pixels [2], [5], [7], [8], [9]
        //   so neighborsParts[2, 5, 7, 8, 9] := 0
        //
        //  BTW we got a normalized convolution matrix.
        //

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

        InterpolationInfo result; // NOLINT (initialization is below)

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

    ARGB InterpolationInfo::applyTo(const Point<size_type> pixelPos, const Image& imageSrc) const
    {
        // 4th is the center
        ARGB srcPixels[9];

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

        return {
            srcPixels[4].a,
            static_cast<std::uint8_t>(fR),
            static_cast<std::uint8_t>(fG),
            static_cast<std::uint8_t>(fB)
        };
    }

} // namespace mglass::magnifiers::detail
