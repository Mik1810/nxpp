cmake_minimum_required(VERSION 3.16)
project(nxpp_external_vendored LANGUAGES CXX)

if(NOT DEFINED NXPP_ROOT)
    message(FATAL_ERROR "NXPP_ROOT must point to the nxpp repository root")
endif()

if(NOT EXISTS "${NXPP_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "NXPP_ROOT does not look like the nxpp repository root: ${NXPP_ROOT}")
endif()

add_subdirectory("${NXPP_ROOT}" "${CMAKE_BINARY_DIR}/_deps/nxpp")

add_executable(nxpp_external_vendored main.cpp)
target_link_libraries(nxpp_external_vendored PRIVATE nxpp::nxpp)
target_compile_features(nxpp_external_vendored PRIVATE cxx_std_20)
