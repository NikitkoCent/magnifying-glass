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

# Retrieve the absolute path to qmake and then use that path to find
# the binaries
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
if ( (NOT _qmake_executable) OR (_qmake_executable STREQUAL "_qmake_executable-NOTFOUND") )
    message(WARNING "Failed to find determine a directory of qmake's executable. mglass-gui target will not be generated.")
    return()
endif ( (NOT _qmake_executable) OR (_qmake_executable STREQUAL "_qmake_executable-NOTFOUND") )
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

add_executable(mglass-gui WIN32 MACOSX_BUNDLE
               "mainwindow.h"
               "mainwindow.cpp"
               "mainwindow.ui"
               "imageview.h"
               "imageview.cpp"
               "main.cpp")

target_include_directories(mglass-gui
                           PRIVATE "${magnifying-glass_SOURCE_DIR}/src/products/internal")

# Use custom Info.plist file for macOS's bundles (it's for enabling high-dpi)
set_target_properties(mglass-gui PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${magnifying-glass_SOURCE_DIR}/src/build/Info.plist")

enable_extra_compiler_warnings(mglass-gui)

target_link_libraries(mglass-gui
                      mglass
                      mglassext
                      Qt5::Core
                      Qt5::Gui
                      Qt5::Widgets)

if (WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE "windeployqt"
                 HINTS "${_qt_bin_dir}"
                 REQUIRED)

    add_custom_command(TARGET mglass-gui POST_BUILD

                       COMMAND "${CMAKE_COMMAND}" -E env
                       PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:mglass-gui>"

                       COMMENT "Deploying Qt (via windeployqt)...")
elseif (APPLE)
    find_program(MACDEPLOYQT_EXECUTABLE "macdeployqt"
                 HINTS "${_qt_bin_dir}"
                 REQUIRED)

    add_custom_command(TARGET mglass-gui POST_BUILD

                       COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                       "$<TARGET_FILE_DIR:mglass-gui>/../.." -always-overwrite

                       COMMENT "Deploying Qt (via macdeployqt)...")
elseif (UNIX)
    find_program(LINUXDEPLOYQT_EXECUTABLE "linuxdeployqt-continuous-x86_64.AppImage")

    if ( (NOT LINUXDEPLOYQT_EXECUTABLE) OR (LINUXDEPLOYQT_EXECUTABLE STREQUAL "LINUXDEPLOYQT_EXECUTABLE-NOTFOUND") )
        message(WARNING "`linuxdeployqt-continuous-x86_64.AppImage` utility was not found. Dependencies of the mglass-gui target will not be deployed.")
        return()
    endif ()

    add_custom_command(TARGET mglass-gui POST_BUILD

                       COMMAND "${CMAKE_COMMAND}" -E make_directory
                       "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/bin"

                       COMMAND "${CMAKE_COMMAND}" -E make_directory
                       "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/lib"

                       COMMAND "${CMAKE_COMMAND}" -E make_directory
                       "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/share/applications"

                       COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                       "$<TARGET_FILE:mglass-gui>" "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/bin/"

                       COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                       "${magnifying-glass_SOURCE_DIR}/src/build/mglass-gui.desktop" "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/share/applications/"

                       COMMAND "${LINUXDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE_DIR:mglass-gui>/mglass-gui.AppDir/usr/share/applications/mglass-gui.desktop"
                       -always-overwrite
                       -bundle-non-qt-libs
                       -verbose=2

                       COMMENT "Deploying Qt, making AppDir bundle (via linuxdeployqt)...")

#    # TODO: rename libQt5<...>.so.5.14.1 to libQt5<...>.so.5
#    # TODO: CI build requires libicu
#
#    add_custom_command(TARGET mglass-gui POST_BUILD
#
#                       COMMAND "${CMAKE_COMMAND}" -E make_directory
#                       "$<TARGET_FILE_DIR:mglass-gui>/platforms"
#
#                       COMMAND "${CMAKE_COMMAND}" -E copy_if_different
#                       "$<TARGET_FILE:Qt5::QXcbIntegrationPlugin>" "$<TARGET_FILE_DIR:mglass-gui>/platforms/"
#
#                       COMMAND "${CMAKE_COMMAND}" -E copy_if_different
#                       "$<TARGET_FILE:Qt5::Core>" "$<TARGET_FILE:Qt5::Gui>" "$<TARGET_FILE:Qt5::Widgets>" "$<TARGET_FILE_DIR:mglass-gui>"
#
#                       COMMENT "Deploying Qt...")
endif (WIN32)
