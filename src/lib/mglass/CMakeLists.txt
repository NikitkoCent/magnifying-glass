add_library(mglass STATIC
            "${magnifying-glass_SOURCE_DIR}/include/mglass/mglass.h"
            "${magnifying-glass_SOURCE_DIR}/include/mglass/image.h"
            "${magnifying-glass_SOURCE_DIR}/include/mglass/primitives.h"
            "${magnifying-glass_SOURCE_DIR}/include/mglass/shape.h"
            "${magnifying-glass_SOURCE_DIR}/include/mglass/shapes.h"
            "${magnifying-glass_SOURCE_DIR}/include/mglass/magnifiers.h"
            "mglass.cpp"
            "magnifiers.cpp"
            "shapes.cpp")

enable_extra_compiler_warnings(mglass)

target_include_directories(mglass
                           PRIVATE "${magnifying-glass_SOURCE_DIR}/third-party/lodepng")

target_link_libraries(mglass
                      PRIVATE lodepng)
