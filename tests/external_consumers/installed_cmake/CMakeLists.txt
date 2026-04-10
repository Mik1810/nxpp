cmake_minimum_required(VERSION 3.16)
project(nxpp_external_installed LANGUAGES CXX)

find_package(nxpp CONFIG REQUIRED)

add_executable(nxpp_external_installed main.cpp)
target_link_libraries(nxpp_external_installed PRIVATE nxpp::nxpp)
target_compile_features(nxpp_external_installed PRIVATE cxx_std_20)
