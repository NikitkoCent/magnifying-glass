cmake_minimum_required(VERSION 3.8.2)
project(magnifying-glass CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

include("third-party/cmake-modules/EnableExtraCompilerWarnings.cmake")

add_subdirectory(src)
add_subdirectory(third-party)

# Enable testing for the project
enable_testing()
add_subdirectory(tests)
