#ifndef LENNA_DATA_H
#define LENNA_DATA_H

#include "mglass/image.h"   // mglass::ARGB
#include <array>            // std::array

namespace resources::lenna_512_512
{
    extern const std::array<mglass::ARGB, 512 * 512> data;
} // namespace resources::lenna_512_512

#endif // ndef LENNA_DATA_H
