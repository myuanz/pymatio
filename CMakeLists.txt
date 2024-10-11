cmake_minimum_required(VERSION 3.18...3.27 FATAL_ERROR)

# <include CPM and packages>-----------------------------
include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#11.0.2")
CPMAddPackage("gh:madler/zlib#v1.3.1")
CPMAddPackage("gh:HDFGroup/hdf5#hdf5_1.14.5")
CPMAddPackage("gh:tbeu/matio#v1.5.27")
CPMAddPackage("gh:bombela/backward-cpp#v1.6")

# </include CPM and packages>-----------------------------


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_COMMANDS_OUTPUT_DIRECTORY .vscode)

# <setup nanobind>-----------------------------
set(DEV_MODULE Development)
find_package(Python 3.8 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

# </setup nanobind>-----------------------------


project(libpymatio)
# Configure and make available zlib
FetchContent_MakeAvailable(zlib)

# Configure HDF5
set(HDF5_BUILD_SHARED_LIBS OFF CACHE BOOL "Build Shared Libraries" FORCE)
set(HDF5_BUILD_STATIC_LIBS ON CACHE BOOL "Build Static Libraries" FORCE)
set(HDF5_ENABLE_Z_LIB_SUPPORT ON CACHE BOOL "Enable Zlib Filters" FORCE)
set(HDF5_BUILD_TOOLS OFF CACHE BOOL "Build HDF5 Tools" FORCE)
set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "Build HDF5 Library Examples" FORCE)

# Configure matio
set(MATIO_WITH_HDF5 ON CACHE BOOL "Enable HDF5 support" FORCE)
set(MATIO_WITH_ZLIB ON CACHE BOOL "Enable Zlib support" FORCE)
set(MATIO_MAT73 ON CACHE BOOL "Enable support for version 7.3 MAT files" FORCE)
set(MATIO_DEFAULT_FILE_VERSION "5" CACHE STRING "Select what MAT file format version is used by default" FORCE)

set(MATIO_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(MATIO_STATIC ON CACHE BOOL "Build static library" FORCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

nanobind_add_module(libpymatio NB_STATIC src/pymatio.cpp)

target_link_libraries(libpymatio PRIVATE fmt zlib hdf5 matio)

target_compile_definitions(libpymatio PRIVATE UNICODE)
if(MSVC)
    target_compile_options(libpymatio PRIVATE /utf-8)
endif()
