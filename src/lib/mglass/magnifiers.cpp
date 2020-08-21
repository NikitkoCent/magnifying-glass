#include "mglass/magnifiers.h"
#include <algorithm>            // std::min, std::max

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
        result.y -= halfHeight;

        return result;
    }

    Point<int_type> getAreaRightBottom(const IntegralRectArea area) noexcept
    {
        Point<int_type> result = area.topLeft;

        if ((area.width < 1) || (area.height < 1))
            return result;

        result.x += static_cast<int_type>(area.width - 1);
        result.y -= static_cast<int_type>(area.height - 1);

        return result;
    }

    Point<float_type> restrictPointBy(const IntegralRectArea area, Point<float_type> point)
    {
        const auto topLeftFloat = pointCast<float_type>(area.topLeft);
        const auto rightBottomFloat = pointCast<float_type>(getAreaRightBottom(area));

        point.x = (std::min)( (std::max)(point.x, topLeftFloat.x), rightBottomFloat.x );
        point.y = (std::min)( (std::max)(point.y, rightBottomFloat.y), topLeftFloat.y );

        return point;
    }

    Point<float_type> scaleVectorBy(const float_type scaleFactor, const Point<float_type> start, Point<float_type> end) noexcept
    {
        end.x = start.x + (end.x - start.x) * scaleFactor;
        end.y = start.y + (end.y - start.y) * scaleFactor;

        return end;
    }
} // namespace mglass::magnifiers::detail
