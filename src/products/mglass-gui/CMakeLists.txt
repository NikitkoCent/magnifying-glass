set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# From official Qt's CMake tutorial
if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(mglass-gui WIN32
               "mainwindow.h"
               "mainwindow.cpp"
               "mainwindow.ui"
               "imageview.h"
               "imageview.cpp"
               "main.cpp")

enable_extra_compiler_warnings(mglass-gui)

target_link_libraries(mglass-gui
                      mglass
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets)
