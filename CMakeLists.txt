cmake_minimum_required(VERSION 3.0.0)
project(grml VERSION 0.0.0)

## Compiler
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## Project Dependencies
find_package(Boost 1.65 COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

## Project Sources
file(GLOB_RECURSE PROJECT_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE TEST_SRCS ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.h)

# Main
add_executable(${PROJECT_NAME} ${PROJECT_SRCS})
target_link_libraries(${PROJECT_NAME})

# Tests
include(CTest)
enable_testing()

add_executable(tests ${TEST_SRCS})
target_link_libraries(tests
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
add_test(NAME tests COMMAND tests)
