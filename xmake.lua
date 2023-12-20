add_rules("mode.debug", "mode.release")
set_languages("cxx11")

add_requires("matio", {configs = {zlib = true, hdf5 = true, mat73 = true}})
add_requires("pybind11", {configs = {python = "python"}})

target("demo")
    set_kind("shared")
    add_packages("matio", "pybind11")
    add_cxflags("$(shell python -m pybind11 --includes)")

    -- add_files("src/*.cpp")
    set_extension("$(shell python -c \"print%(__import__%('sysconfig'%).get_config_var%('EXT_SUFFIX'%)%)\")")

    add_files("src/main.cpp")
    add_includedirs("src")

    after_build(
        function(target)
            local targetfile = target:targetfile()
            os.cp(targetfile, path.join("./", path.filename(targetfile):sub(4)))
        end
    )
