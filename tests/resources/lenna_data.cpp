#include "lenna_data.h"


namespace resources::lenna_512_512
{
    const std::array<mglass::ARGB, 512 * 512> data{{
#include "lenna_data_blob.txt"
    }};
} // namespace resources::lenna_512_512
