project(intersections)

cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(WARNING_FLAGS "-Wall" "-Wextra" "-Werror")
elseif (MSVC)
    set(WARNING_FLAGS "/W4")
endif ()

find_package(RapidJSON REQUIRED)

# library
add_library(intersections
        "include/intersections.h"
        "include/interval.h"
        "include/rectangle.h"
        "src/fast.cpp"
        "src/simple.cpp"
        "src/transitions.h")
target_include_directories(intersections PUBLIC "include/")
target_compile_options(intersections PRIVATE "${WARNING_FLAGS}")

# tests
add_executable(tests "src/test.cpp")
target_compile_options(tests PRIVATE "${WARNING_FLAGS}")
set_target_properties(tests PROPERTIES OUTPUT_NAME "tests")
target_link_libraries(tests intersections)

# utility
add_executable(main "src/main.cpp" src/rapidjson_assert.h)
target_compile_options(main PRIVATE "${WARNING_FLAGS}")
set_target_properties(main PROPERTIES OUTPUT_NAME "intersections")
target_link_libraries(main intersections)
target_include_directories(main SYSTEM PRIVATE "${RapidJSON_SOURCE_DIR}/include/")
