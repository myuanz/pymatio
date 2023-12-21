add_rules("mode.debug", "mode.release")
-- set_languages("cxx11")

add_requires("matio", {configs = {zlib = true, hdf5 = true, mat73 = true}})
add_requires("pybind11")

target("pymatio")
    set_kind("shared")
    add_packages("matio", "pybind11")
    add_cxflags("$(shell python -m pybind11 --includes)")

    set_extension("$(shell python -c \"print%(__import__%('sysconfig'%).get_config_var%('EXT_SUFFIX'%), end=''%)\")")

    -- add_files("src/main.cpp")
    add_files("src/*.cpp")
    -- add_files("src/test_matio.cpp")

    add_includedirs("src")

    -- after_build(
    --     function(target)
    --         local targetfile = target:targetfile()
    --         os.cp(targetfile, path.join("./", path.filename(targetfile):sub(4)))
    --     end
    -- )

target("test-matio")
    set_kind("binary")
    add_packages("matio", "pybind11")

    add_files("src/test_matio.cpp")
    add_includedirs("src")
