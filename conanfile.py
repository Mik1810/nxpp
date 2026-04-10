from conan import ConanFile
from conan.tools.files import copy


class NxppConan(ConanFile):
    name = "nxpp"
    version = "1.0.19"
    package_type = "header-library"
    default_options = {"boost/*:header_only": True}

    license = "MIT"
    url = "https://github.com/Mik1810/nxpp"
    description = "Header-only C++20 graph library on top of Boost Graph Library"
    topics = ("graph", "boost", "bgl", "header-only", "cxx20")

    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "include/*", "cmake/*", "CMakeLists.txt", "LICENSE", "README.md"
    no_copy_source = True

    def package_id(self):
        self.info.clear()

    def requirements(self):
        self.requires("boost/1.86.0")

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst=self.package_folder)
        copy(self, "*.hpp", src=self.source_folder, dst=self.package_folder)

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.set_property("cmake_file_name", "nxpp")
        self.cpp_info.set_property("cmake_target_name", "nxpp::nxpp")
        self.cpp_info.requires = ["boost::boost"]
