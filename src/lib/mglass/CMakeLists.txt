add_library(mglass STATIC
            "${magnifying-glass_SOURCE_DIR}/include/mglass/mglass.h"
            "mglass.cpp")

enable_extra_compiler_warnings(mglass)
