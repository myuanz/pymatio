add_rules("mode.debug", "mode.release")

package("libmatio2")
    set_homepage("https://matio.sourceforge.io")
    set_description("MATLAB MAT File I/O Library")
    set_license("BSD-2-Clause")

    add_urls("https://github.com/tbeu/matio/archive/refs/tags/$(version).tar.gz",
             "https://github.com/tbeu/matio.git", {submodules = false})

    add_versions("v1.5.26", "4aa5ac827ee49a3111f88f8d9b8ae034b8757384477e8f29cb64582c7d54e156")

    add_configs("zlib", {description = "Build with zlib support", default = false, type = "boolean"})
    add_configs("hdf5", {description = "Build with hdf5 support", default = false, type = "boolean"})
    add_configs("extended_sparse", {description = "Enable extended sparse matrix data types not supported in MATLAB", default = false, type = "boolean"})
    add_configs("mat73", {description = "Enable support for version 7.3 MAT files", default = false, type = "boolean"})
    add_configs("default_file_version", {description = "Select what MAT file format version is used by default", default = "5", type = "string", values = {"4", "5", "7.5"}})

    add_deps("cmake")

    on_load(function (package)
        if package:config("zlib") then
            package:add("deps", "zlib >=1.2.3")
        end
        if package:config("hdf5") then
            package:add("deps", "hdf5 >=1.8.x")
        end
    end)

    on_install("windows", "linux", "macosx", "bsd", "android", "iphoneos", "cross", "wasm", function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:is_debug() and "Debug" or "Release"))
        table.insert(configs, "-DMATIO_SHARED=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_PIC=" .. (package:config("pic") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_WITH_ZLIB=" .. (package:config("zlib") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_WITH_HDF5=" .. (package:config("hdf5") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_EXTENDED_SPARSE=" .. (package:config("extended_sparse") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_MAT73=" .. (package:config("mat73") and "ON" or "OFF"))
        table.insert(configs, "-DMATIO_DEFAULT_FILE_VERSION=" .. package:config("default_file_version"))
        io.replace("CMakeLists.txt", "include(cmake/tools.cmake)", "", {plain = true})
        io.replace("CMakeLists.txt", "include(cmake/test.cmake)", "", {plain = true})
        io.replace("cmake/src.cmake", "    ${PROJECT_BINARY_DIR}/src/matio_pubconf.h\n)", 
[[
    ${PROJECT_BINARY_DIR}/src/matio_pubconf.h
    ${PROJECT_BINARY_DIR}/src/matioConfig.h
    ${PROJECT_SOURCE_DIR}/src/matio_private.h
)
]], {plain = true})

        local packagedeps = {}
        if package:config("hdf5") then
            table.insert(packagedeps, "hdf5")
        end

        import("package.tools.cmake").install(package, configs, {packagedeps = packagedeps})
    end)

    on_test(function (package)
        assert(package:has_cfuncs("Mat_Open", {includes = "matio_private.h"}))
    end)
package_end()

set_languages("cxx17")
set_runtimes("MD")

add_requires("libmatio2", {
    configs = {zlib = true, hdf5 = true, mat73 = true},
})
add_requires("pybind11")

target("pymatio")
    set_kind("shared")
    add_packages("libmatio2", "pybind11")
    add_cxflags("$(shell python -m pybind11 --includes)")
    set_extension("$(shell python -c \"print%(__import__%('sysconfig'%).get_config_var%('EXT_SUFFIX'%), end=''%)\")")

    add_files("src/*.cpp")
    add_includedirs("src")

target("test")
    set_kind("binary")
    add_packages("libmatio2")
    add_files("src/test_matio.cpp")
    add_includedirs("src")
