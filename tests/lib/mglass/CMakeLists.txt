find_package(Threads)

add_executable(mglasstests tests.cpp)

target_link_libraries(mglasstests gtest gmock_main Threads::Threads)

add_test(NAME mglasstests_all COMMAND mglasstests)
# add_test(NAME test_name COMMAND mglasstests --gtest_filter=<FILTER-MASK>)
