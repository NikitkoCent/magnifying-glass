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

    Point<float_type> scaleVectorBy(const float_type scaleFactor, const Point<float_type> start, Point<float_type> end) noexcept
    {
        end.x = start.x + (end.x - start.x) * scaleFactor;
        end.y = start.y + (end.y - start.y) * scaleFactor;

        return end;
    }
} // namespace mglass::magnifiers::detail
