#include "mglass/magnifiers.h"

namespace mglass::magnifiers::detail
{
    Point<float_type> getAreaCenter(const IntegralRectArea area) noexcept
    {
        auto result = pointCast<float_type>(area.topLeft);

        if ((area.width < 1) || (area.height < 1))
            return result;

        const auto halfWidth = static_cast<float_type>(area.width - 1) / 2;
        const auto halfHeight = static_cast<float_type>(area.height - 1) / 2;

        result.x += halfWidth;
        result.y += halfHeight;

        return result;
    }

    // returns a new end which distance to the `start` is scaleFactor * distance(`start`, `end`)
    Point<float_type> scaleVectorBy(const float_type scaleFactor, const Point<float_type> start, Point<float_type> end)
    {
        end.x = start.x + (end.x - start.x) * scaleFactor;
        end.y = start.y + (end.y - start.y) * scaleFactor;
        return end;
    }

} // namespace mglass::magnifiers::detail
