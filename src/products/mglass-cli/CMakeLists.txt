add_executable(mglass-cli main.cpp)

enable_extra_compiler_warnings(mglass-cli)

target_link_libraries(mglass-cli mglass)
