get_filename_component(NXPP_SOURCE_PATH "${CURRENT_PORT_DIR}/../../../.." ABSOLUTE)

vcpkg_cmake_configure(
    SOURCE_PATH "${NXPP_SOURCE_PATH}"
    OPTIONS
        -DNXPP_BUILD_TESTS=OFF
        -DNXPP_BUILD_SMOKE_TESTS=OFF
        -DNXPP_BUILD_LARGE_GRAPH_COMPARE=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME nxpp CONFIG_PATH lib/cmake/nxpp)

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/lib"
    "${CURRENT_PACKAGES_DIR}/debug"
)

file(INSTALL
    "${NXPP_SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright
)
