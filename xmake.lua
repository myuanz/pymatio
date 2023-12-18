add_rules("mode.debug", "mode.release")
set_runtimes("MT")

add_requires("zlib", "xmake::hdf5", {system = false})

package("libmatio2")
    -- add_deps("cmake", "zlib", "hdf5")
    -- set_kind("static")

    -- set_sourcedir(path.join(os.scriptdir(), "."))


    -- on_install(function (package)

    --     local hdf5_info = find_package("hdf5")
    --     print(hdf5_info)
    --     -- TODO: 这里还没弄明白怎么找到 hdf5 path
    --     if hdf5_info then
    --         print(hdf5_info)
    --         print("hdf5_info: " .. hdf5_info.includedirs[1])
    --         setenv("HDF5_DIR", hdf5_info.includedirs)
    --         -- HDF5_LIBRARIES HDF5_INCLUDE_DIRS
    --         setenv("HDF5_LIBRARIES", hdf5_info.linkdirs)
    --         setenv("HDF5_INCLUDE_DIRS", hdf5_info.includedirs)
    --     else 
    --         print("hdf5_info: not found")
    --         -- setenv("HDF5_DIR", "C:/Users/myuan/AppData/Local/.xmake/packages/h/hdf5/1.14.0/5fed7b83d9954eaeb4f54a0905105404/cmake")
    --         -- os.setenv("HDF5_DIR", "C:/Users/myuan/AppData/Local/.xmake/packages/h/hdf5/1.14.0/5fed7b83d9954eaeb4f54a0905105404/")
    --         print(os.getenv("HDF5_DIR"))
    --     end

    --     local configs = {}
    --     table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
    --     -- table.insert(configs, "-DBUILD_SHARED=" .. (package:config("shared") and "ON" or "OFF"))
    --     table.insert(configs, "-DBUILD_SHARED=OFF")
    --     -- table.insert(configs, "-DCMAKE_INSTALL_PREFIX=" .. "'C:/Users/myuan/AppData/Local/.xmake/packages/h/hdf5/1.14.0/5fed7b83d9954eaeb4f54a0905105404/cmake'")
    --     table.insert(configs, "-DMATIO_MAT73=ON")
    --     import("package.tools.cmake").install(package, configs)
    -- end)
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
        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cfuncs("Mat_Open", {includes = "matio.h"}))
    end)
package_end()

add_requires("libmatio2", "cgetopt")

target("demo")
    set_kind("binary")
    set_policy("build.merge_archive", true)

    add_packages("libmatio2", "zlib", "hdf5", "cgetopt")
    add_includedirs("src", "getopt")
    add_linkdirs("src", "getopt")
    -- add_linkdirs("C:/Users/myuan/projects/pymatio/matio/build/.packages/l/libmatio2/latest/8574311f4b544fa89a3d8c3ee79d1929/lib/")
    -- add_links("libmatio")
    add_files("test/test_mat.c")
    -- add_ldflags("-static", {force = true})