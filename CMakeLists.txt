cmake_minimum_required(VERSION 3.0.0)
project(grml VERSION 0.0.0)

## Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Project Dependencies
include(vendor/boost/CMakeLists.txt)

## Libraries
include_directories(lib)

# Applications
add_executable(${PROJECT_NAME} bin/grml.cpp)
target_link_libraries(${PROJECT_NAME} Boost::boost)

# Tests
include(CTest)
enable_testing()

add_executable(tests lib/test_parser.cpp)
target_link_libraries(tests
    Boost::unit_test_framework
)
add_test(NAME tests COMMAND tests)
