cmake_minimum_required(VERSION 3.16)

project(nxpp VERSION 1.0.1 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(Boost REQUIRED)

add_library(nxpp INTERFACE)
add_library(nxpp::nxpp ALIAS nxpp)

target_compile_features(nxpp INTERFACE cxx_std_20)

if(TARGET Boost::headers)
    set(NXPP_BOOST_TARGET Boost::headers)
elseif(TARGET Boost::boost)
    set(NXPP_BOOST_TARGET Boost::boost)
else()
    message(FATAL_ERROR "nxpp requires FindBoost to provide Boost::headers or Boost::boost.")
endif()

target_include_directories(nxpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(nxpp INTERFACE ${NXPP_BOOST_TARGET})

install(TARGETS nxpp
    EXPORT nxppTargets
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT nxppTargets
    FILE nxppTargets.cmake
    NAMESPACE nxpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nxpp
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nxppConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nxppConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nxpp
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/nxppConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/nxppConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/nxppConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nxpp
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
