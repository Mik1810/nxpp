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
option(NXPP_BUILD_SMOKE_TESTS "Build a reduced nxpp smoke test subset" OFF)
option(NXPP_BUILD_LARGE_GRAPH_COMPARE "Build the nxpp large-graph comparison test" OFF)
set(NXPP_HEADER_UNDER_TEST "" CACHE STRING "Override the header path used by test sources")

if(NXPP_BUILD_TESTS OR NXPP_BUILD_SMOKE_TESTS)
    enable_testing()

    if(NXPP_BUILD_SMOKE_TESTS)
        set(NXPP_FORMAL_TEST_SOURCES
            tests/test_core.cpp
            tests/test_edge_cases.cpp
        )
    else()
        set(NXPP_FORMAL_TEST_SOURCES
            tests/test_core.cpp
            tests/test_attributes.cpp
            tests/test_edge_cases.cpp
            tests/test_flow.cpp
            tests/test_remove_node.cpp
            tests/test_multigraph.cpp
        )
    endif()

    foreach(test_source IN LISTS NXPP_FORMAL_TEST_SOURCES)
        get_filename_component(test_name "${test_source}" NAME_WE)
        add_executable("${test_name}" "${test_source}")
        target_link_libraries("${test_name}" PRIVATE nxpp)
        target_include_directories("${test_name}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
        if(NXPP_HEADER_UNDER_TEST)
            target_compile_definitions("${test_name}" PRIVATE NXPP_HEADER_UNDER_TEST="${NXPP_HEADER_UNDER_TEST}")
        endif()
        add_test(NAME "${test_name}" COMMAND "${test_name}")
    endforeach()
endif()

if(NXPP_BUILD_LARGE_GRAPH_COMPARE)
    enable_testing()

    add_executable(test_large_graph_compare tests/test_large_graph_compare.cpp)
    target_link_libraries(test_large_graph_compare PRIVATE nxpp)
    target_include_directories(test_large_graph_compare PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    if(NXPP_HEADER_UNDER_TEST)
        target_compile_definitions(test_large_graph_compare PRIVATE NXPP_HEADER_UNDER_TEST="${NXPP_HEADER_UNDER_TEST}")
    endif()
    add_test(NAME test_large_graph_compare COMMAND test_large_graph_compare)
endif()
