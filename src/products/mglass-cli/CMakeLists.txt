add_executable(mglass-cli main.cpp)

enable_extra_compiler_warnings(mglass-cli)

target_include_directories(mglass-cli
                           PRIVATE "${magnifying-glass_SOURCE_DIR}/src/products/internal")

target_link_libraries(mglass-cli mglass mglassext)
