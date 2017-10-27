cmake_minimum_required(VERSION 3.9)
project(grml VERSION 0.0.0)

## Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/W4 /std:c++latest)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif(MSVC)

## Project Dependencies
find_package(Boost 1.64 REQUIRED COMPONENTS unit_test_framework)

## Libraries
add_library(types lib/types.cpp)
target_include_directories(types PUBLIC lib)
target_link_libraries(types PUBLIC Boost::boost)

add_library(parser INTERFACE)
target_include_directories(parser INTERFACE lib)
target_link_libraries(parser INTERFACE types)

add_library(inferer lib/inferer.cpp)
target_include_directories(inferer PUBLIC lib)
target_link_libraries(inferer PUBLIC types)

# Applications
add_executable(${PROJECT_NAME} bin/grml.cpp)
target_link_libraries(${PROJECT_NAME} parser)

# Tests
include(CTest)
enable_testing()

add_executable(test_types lib/test_types.cpp)
target_link_libraries(test_types
    types
    Boost::unit_test_framework
)
add_test(NAME test_types COMMAND test_types)

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
