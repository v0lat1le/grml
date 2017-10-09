cmake_minimum_required(VERSION 3.0.0)
project(grml VERSION 0.0.0)
set (VERBOSE ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Main
add_executable(grml main.cpp)
target_link_libraries(grml)

# Tests
include(CTest)
enable_testing()

add_executable(tests tests.cpp)
target_link_libraries(tests
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
add_test(NAME tests COMMAND tests)
