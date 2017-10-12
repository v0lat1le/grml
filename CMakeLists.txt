cmake_minimum_required(VERSION 3.0.0)
project(grml VERSION 0.0.0)

## Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Project Dependencies
include(vendor/boost/CMakeLists.txt)

## Libraries
add_library(parser INTERFACE)
target_include_directories(parser INTERFACE lib)
target_link_libraries(parser INTERFACE Boost::boost)

add_library(inferer lib/inferer.cpp)
target_include_directories(inferer PUBLIC lib)
target_link_libraries(inferer parser)

# Applications
add_executable(${PROJECT_NAME} bin/grml.cpp)
target_link_libraries(${PROJECT_NAME} parser)

# Tests
include(CTest)
enable_testing()

add_executable(test_parser lib/test_parser.cpp)
target_link_libraries(test_parser
    parser
    Boost::unit_test_framework
)
add_test(NAME test_parser COMMAND test_parser)

add_executable(test_inferer lib/test_inferer.cpp)
target_link_libraries(test_inferer
    inferer
    Boost::unit_test_framework
)
add_test(NAME test_inferer COMMAND test_inferer)
