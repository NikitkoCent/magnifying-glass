add_library(mglassext STATIC
            "polymorphic_shape.h"
            "polymorphic_shapes.h"
            "polymorphic_shapes.cpp"
           )

enable_extra_compiler_warnings(mglassext)

target_link_libraries(mglassext
                      PUBLIC mglass)
