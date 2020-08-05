#ifndef MAGNIFYING_GLASS_MAGNIFIERS_H
#define MAGNIFYING_GLASS_MAGNIFIERS_H

#include "mglass/primitives.h"
#include "mglass/shape.h"
#include "mglass/image.h"

namespace mglass::magnifiers
{
    // imageDst will have width equal to std::ceil(shape.getWidth()) and height equal to std::ceil(shape.getHeight())
    void nearestNeighbor(
        const Shape& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageLeftTop,
        Image& imageDst);

    // imageDst will have width equal to std::ceil(shape.getWidth()) and height equal to std::ceil(shape.getHeight())
    void nearestNeighborInterpolated(
        const Shape& shape,
        mglass::float_type scaleFactor,
        const Image& imageSrc,
        Point<mglass::int_type> imageLeftTop,
        Image& imageDst);
}

#endif // ndef MAGNIFYING_GLASS_MAGNIFIERS_H
