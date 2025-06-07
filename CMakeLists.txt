cmake_minimum_required(VERSION 3.15)
project(conceal LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(CONCEAL_USE_JSON "Enable JSON support via nlohmann/json" ON)
set(CONCEAL_HIDDEN_MSG "<hidden>" CACHE STRING "Hidden message for conceal output")

add_library(conceal INTERFACE)
target_include_directories(conceal INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(conceal INTERFACE CONCEAL_HIDDEN_MSG="${CONCEAL_HIDDEN_MSG}")

if(CONCEAL_USE_JSON)
    find_package(nlohmann_json REQUIRED)
    target_link_libraries(conceal INTERFACE nlohmann_json::nlohmann_json)
    target_compile_definitions(conceal INTERFACE CONCEAL_USE_JSON)
endif()

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    target_link_libraries(conceal INTERFACE GTest::gtest GTest::gtest_main)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
