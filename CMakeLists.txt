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
