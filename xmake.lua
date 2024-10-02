add_rules("mode.debug", "mode.release")

-- xrepo 的 matio 不包含私有头，但是要想看到 mat_t 结构体，又必须有似有头
package("_matio")
    set_homepage("https://matio.sourceforge.io")
    set_description("MATLAB MAT File I/O Library")
    set_license("BSD-2-Clause")

    add_urls("https://github.com/tbeu/matio/archive/refs/tags/$(version).tar.gz",
             "https://github.com/tbeu/matio.git", {submodules = false})

    add_versions("v1.5.27", "2efe7c4a206885287c0f56128f3a36aa6e453077d03e4c2c42cdce9d332b67eb")
    add_versions("v1.5.26", "4aa5ac827ee49a3111f88f8d9b8ae034b8757384477e8f29cb64582c7d54e156")

    add_configs("zlib", {description = "Build with zlib support", default = true, type = "boolean"})
    add_configs("hdf5", {description = "Build with hdf5 support", default = true, type = "boolean"})
    add_configs("extended_sparse", {description = "Enable extended sparse matrix data types not supported in MATLAB", default = false, type = "boolean"})
    add_configs("mat73", {description = "Enable support for version 7.3 MAT files", default = true, type = "boolean"})
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

package("_pybind11")

    set_kind("headeronly")
    set_homepage("https://github.com/pybind/pybind11")
    set_description("Seamless operability between C++11 and Python.")
    set_license("BSD-3-Clause")

    add_urls("https://github.com/pybind/pybind11/archive/refs/tags/$(version).zip",
             "https://github.com/pybind/pybind11.git")
    add_versions("v2.13.6", "d0a116e91f64a4a2d8fb7590c34242df92258a61ec644b79127951e821b47be6")
    add_versions("v2.13.5", "0b4f2d6a0187171c6d41e20cbac2b0413a66e10e014932c14fae36e64f23c565")
    add_versions("v2.5.0", "1859f121837f6c41b0c6223d617b85a63f2f72132bae3135a2aa290582d61520")
    add_versions("v2.6.2", "0bdb5fd9616fcfa20918d043501883bf912502843d5afc5bc7329a8bceb157b3")
    add_versions("v2.7.1", "350ebf8f4c025687503a80350897c95d8271bf536d98261f0b8ed2c1a697070f")
    add_versions("v2.8.1", "90907e50b76c8e04f1b99e751958d18e72c4cffa750474b5395a93042035e4a3")
    add_versions("v2.9.1", "ef9e63be55b3b29b4447ead511a7a898fdf36847f21cec27a13df0db051ed96b")
    add_versions("v2.9.2", "d1646e6f70d8a3acb2ddd85ce1ed543b5dd579c68b8fb8e9638282af20edead8")
    add_versions("v2.10.0", "225df6e6dea7cea7c5754d4ed954e9ca7c43947b849b3795f87cb56437f1bd19")
    add_versions("v2.12.0", "411f77380c43798506b39ec594fc7f2b532a13c4db674fcf2b1ca344efaefb68")
    add_versions("v2.13.1", "a3c9ea1225cb731b257f2759a0c12164db8409c207ea5cf851d4b95679dda072")


    add_deps("cmake")

    on_load("linux", function (package)
        local python_headeronly = package:config("use_python_headeronly")

        cibuildwheel = os.getenv("CIBUILDWHEEL")
        if not cibuildwheel then
            return
        else 
            -- cibuildwheel 容器
            os.exec("printenv")
            local envs = os.getenvs()
            print("all envs:")
            for k, v in pairs(envs) do
                print(k, v, path.is_absolute(v))
            end
            package:config("use_python_headeronly", true)
            package:add("sysincludedirs", envs["XMAKE_PYBIND11_INCLUDE"] .. "/", {public = true})
            package:add("sysincludedirs", envs["XMAKE_PYTHON_INCLUDE"] .. "/", {public = true})
            os.exec("ls -l " .. envs["XMAKE_PYTHON_INCLUDE"])
            os.exec("ls -l " .. envs["XMAKE_PYBIND11_INCLUDE"])
        end
    end)

    on_install("windows|native", "macosx", "linux", function (package)
        import("package.tools.cmake").install(package, {
            "-DPYBIND11_TEST=OFF",
            "-DPYBIND11_FINDPYTHON=ON",
            -- "-DPYTHON_EXECUTABLE=$(env XMAKE_PYTHON_BIN)",
            -- "-DPYTHON_INCLUDE_DIR=$(env XMAKE_PYTHON_INCLUDE)",
            -- "-DPYTHON_LIBRARY=$(env XMAKE_PYTHON_LIB)",
            -- "-DPYTHON_SITE_PACKAGES=$(env XMAKE_PYTHON_SITE_PACKAGES)",
        })
    end)

    on_test(function (package)
        cibuildwheel = os.getenv("CIBUILDWHEEL")
        if cibuildwheel then
            return
        end
        assert(package:check_cxxsnippets({test = [[
            #include <pybind11/pybind11.h>
            int add(int i, int j) {
                return i + j;
            }
            PYBIND11_MODULE(example, m) {
                m.def("add", &add, "A function which adds two numbers");
            }
        ]]}, {configs = {languages = "c++11"}}))
    end)
package_end()

set_runtimes("MD")
set_languages("cxx17")

add_requires("_matio", {
    configs = {zlib = true , hdf5 = true, mat73 = true},
})
add_requires("_pybind11")

-- add_requireconfs("pybind11", {override = true}) -- 如果系统自带了 pybind11，偶尔会出现一些问题，所以这里一定要用 xrepo 的
-- add_requireconfs("pybind11")

target("libpymatio")
    set_kind("shared")
    add_packages("_matio", "_pybind11")
    -- 添加 include 目录
    add_includedirs("$(env XMAKE_PYBIND11_INCLUDE)/", {public = true})
    add_includedirs("$(env XMAKE_PYTHON_INCLUDE)/", {public = true})
    add_linkdirs("$(env XMAKE_PYTHON_LIB)/", {public = true})

    set_extension("$(shell python -c \"print%(__import__%('sysconfig'%).get_config_var%('EXT_SUFFIX'%), end=''%)\")")
    add_files("src/*.cpp")
    add_includedirs("src")

    -- Windows 下，生成的文件名是 libpymatio，但 Linux 下会是 liblibpymatio，所以这里手动设置前缀和基本名称
    set_prefixname("")
    set_basename("libpymatio")
    before_build(function (target)
        print("包含目录:")
        for _, dir in ipairs(target:get("includedirs")) do
            print(dir)
        end
        
        print("\n库目录:")
        for _, dir in ipairs(target:get("libdirs")) do
            print(dir)
        end

        local envs = os.getenvs()
        print("all envs:")
        for k, v in pairs(envs) do
            print(k, v, path.is_absolute(v))
        end
        -- package:add("sysincludedirs", envs["XMAKE_PYBIND11_INCLUDE"] .. "/", {public = true})
        -- package:add("sysincludedirs", envs["XMAKE_PYTHON_INCLUDE"] .. "/", {public = true})

    end)

    -- print(os.getenv("PYTHONPATH"))
    -- print(os.getenv("PATH"))

    -- after_build(function (target)
    --     -- local pydir = path.join(os.projectdir(), "pymatio")
    --     -- local target_file_name = path.filename(target:targetfile())

    --     -- remove the prefix "lib"
    --     -- local target_file_name = target_file_name:sub(4, -1)
    --     -- local target_file = path.join(pydir, target_file_name)
    --     -- print("target_file: " .. target_file)

    --     -- local pydir = path.join("$(buildir)", "binary")
    --     -- os.mkdir(pydir)

    --     -- local f = io.open("C:/Users/myuan/logs/log.log", "w")
    --     -- os.rm(pydir .. "/*.pyd")
    --     -- os.rm(pydir .. "/*.so")
    --     -- f:write("pydir " .. pydir .. "\n")
    --     -- f:write("target:targetfile() " .. target:targetfile() .. "\n")

    --     -- os.cp(target:targetfile(), target_file)
    -- end)