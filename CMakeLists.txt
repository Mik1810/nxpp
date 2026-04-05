cmake_minimum_required(VERSION 3.16)

project(nxpp LANGUAGES CXX)

find_package(Boost REQUIRED)

add_library(nxpp INTERFACE)

target_compile_features(nxpp INTERFACE cxx_std_20)

target_include_directories(nxpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${Boost_INCLUDE_DIRS}>
)

option(NXPP_BUILD_TESTS "Build nxpp formal tests" OFF)

if(NXPP_BUILD_TESTS)
    enable_testing()

    set(NXPP_FORMAL_TEST_SOURCES
        tests/test_core.cpp
        tests/test_attributes.cpp
        tests/test_edge_cases.cpp
        tests/test_flow.cpp
        tests/test_remove_node.cpp
        tests/test_multigraph.cpp
    )

    foreach(test_source IN LISTS NXPP_FORMAL_TEST_SOURCES)
        get_filename_component(test_name "${test_source}" NAME_WE)
        add_executable("${test_name}" "${test_source}")
        target_link_libraries("${test_name}" PRIVATE nxpp)
        target_include_directories("${test_name}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
        add_test(NAME "${test_name}" COMMAND "${test_name}")
    endforeach()
endif()
