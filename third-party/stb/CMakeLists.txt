add_library(stb_image STATIC
            "stb/stb_image.h"
            "stb_image.cpp")

target_compile_definitions(stb_image
                           PUBLIC "STBI_ONLY_PNG")


add_library(stb_image_write STATIC
            "stb/stb_image_write.h"
            "stb_image_write.cpp")

# breaks the building, see https://github.com/nothings/stb/issues/793
#target_compile_definitions(stb_image_write
#                           PUBLIC "STBI_WRITE_NO_STDIO")
