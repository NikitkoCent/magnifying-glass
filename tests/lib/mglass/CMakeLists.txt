find_package(Threads)

add_executable(mglasstests
               "image_tests.cpp"
               "ellipse_shape_tests.cpp"
               "rectangle_shape_tests.cpp"
               "magnifiers_tests.cpp"
               "${magnifying-glass_SOURCE_DIR}/tests/resources/lenna_data.h"
               "${magnifying-glass_SOURCE_DIR}/tests/resources/lenna_data.cpp")

target_include_directories(mglasstests
                           PRIVATE "${magnifying-glass_SOURCE_DIR}/tests")

target_link_libraries(mglasstests
                      mglass
                      gtest
                      gmock_main
                      Threads::Threads)

add_test(NAME mglasstests_all
         COMMAND "mglasstests"
         WORKING_DIRECTORY "${magnifying-glass_SOURCE_DIR}/tests")
# add_test(NAME test_name COMMAND mglasstests --gtest_filter=<FILTER-MASK>)
