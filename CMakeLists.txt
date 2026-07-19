cmake_minimum_required(VERSION 4.2)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

project(sosal)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

add_compile_options(-fsanitize=address,leak,undefined)
add_link_options(-fsanitize=address,leak,undefined)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
        FILE_SET modules
        TYPE CXX_MODULES FILES
        modules/examples/rb_tree_ex.cppm
)

target_link_libraries(${PROJECT_NAME} gtest)