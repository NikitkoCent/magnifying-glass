#ifndef MAGNIFYING_GLASS_MAGNIFIERS_H
#define MAGNIFYING_GLASS_MAGNIFIERS_H

#include "mglass/primitives.h"
#include "mglass/shape.h"
#include "mglass/image.h"

namespace mglass::magnifiers
{
    Image nearestNeighbor(const Shape& shape, mglass::float_type scaleFactor, const Image& imageSrc, Point<mglass::int_type> imageLeftTop);

    Image nearestNeighborInterpolated(const Shape& shape, mglass::float_type scaleFactor, const Image& imageSrc, Point<mglass::int_type> imageLeftTop);
}

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
