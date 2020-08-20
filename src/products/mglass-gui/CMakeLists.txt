set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# From official Qt's CMake tutorial
if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets)

if (NOT Qt5Core_FOUND)
    message(WARNING "Qt5::Core component was not found. mglass-gui target will not be generated.")
    return()
endif(NOT Qt5Core_FOUND)
if (NOT Qt5Gui_FOUND)
    message(WARNING "Qt5::Gui component was not found. mglass-gui target will not be generated.")
    return()
endif(NOT Qt5Gui_FOUND)
if (NOT Qt5Widgets_FOUND)
    message(WARNING "Qt5::Widgets component was not found. mglass-gui target will not be generated.")
    return()
endif(NOT Qt5Widgets_FOUND)

add_executable(mglass-gui WIN32
               "mainwindow.h"
               "mainwindow.cpp"
               "mainwindow.ui"
               "imageview.h"
               "imageview.cpp"
               "polymorphic_shape.h"
               "polymorphic_shapes.h"
               "polymorphic_shapes.cpp"
               "main.cpp")

enable_extra_compiler_warnings(mglass-gui)

target_link_libraries(mglass-gui
                      mglass
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets)

# copy Qt dlls to exe build dir
add_custom_command(TARGET mglass-gui POST_BUILD

                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                   "$<TARGET_FILE:Qt5::Core>" "$<TARGET_FILE:Qt5::Gui>" "$<TARGET_FILE:Qt5::Widgets>" "$<TARGET_FILE_DIR:mglass-gui>")

if (WIN32)
    # QPA plugin(s)
    add_custom_command(TARGET mglass-gui POST_BUILD

                       COMMAND "${CMAKE_COMMAND}" -E make_directory
                       "$<TARGET_FILE_DIR:mglass-gui>/platforms"

                       COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                       "$<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>" "$<TARGET_FILE_DIR:mglass-gui>/platforms/")
endif(WIN32)
