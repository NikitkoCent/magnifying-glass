add_executable(mglass-gui main.cpp)

enable_extra_compiler_warnings(mglass-gui)

target_link_libraries(mglass-gui mglass)
