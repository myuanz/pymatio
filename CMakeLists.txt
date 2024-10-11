cmake_minimum_required(VERSION 3.18...3.27 FATAL_ERROR)

# <include CPM and packages>-----------------------------
include(cmake/CPM.cmake)


CPMAddPackage(
  NAME szip
  URL "https://docs.hdfgroup.org/archive/support/ftp/lib-external/szip/2.1.1/src/szip-2.1.1.tar.gz"
  OPTIONS
    "SZIP_ENABLE_ENCODING ON"
    # "BUILD_SHARED_LIBS OFF"
)
CPMAddPackage("gh:fmtlib/fmt#11.0.2")
CPMAddPackage(
  NAME zlib
  URL "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.tar.gz"
  OPTIONS "ZLIB_BUILD_TESTS OFF"
  OPTIONS "ZLIB_BUILD_EXAMPLES OFF"
)

CPMAddPackage("gh:HDFGroup/hdf5#hdf5_1.14.5")
CPMAddPackage("gh:tbeu/matio#v1.5.27")
CPMAddPackage("gh:bombela/backward-cpp#v1.6")

set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
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


project(libpymatio LANGUAGES CXX)

if (NOT SKBUILD)
  message(WARNING "\
  This CMake file is meant to be executed using 'scikit-build-core'.
  Running it directly will almost certainly not produce the desired
  result. If you are a user trying to install this package, use the
  command below, which will install all necessary build dependencies,
  compile the package in an isolated environment, and then install it.
  =====================================================================
   $ pip install .
  =====================================================================
  If you are a software developer, and this is your own package, then
  it is usually much more efficient to install the build dependencies
  in your environment once and use the following command that avoids
  a costly creation of a new virtual environment at every compilation:
  =====================================================================
   $ pip install nanobind scikit-build-core[pyproject]
   $ pip install --no-build-isolation -ve .
  =====================================================================
  You may optionally add -Ceditable.rebuild=true to auto-rebuild when
  the package is imported. Otherwise, you need to rerun the above
  after editing C++ files.")
endif()

# Configure and make available zlib

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

target_link_libraries(libpymatio PRIVATE fmt zlib szip hdf5 matio)

target_compile_definitions(libpymatio PRIVATE UNICODE)
if(MSVC)
    target_compile_options(libpymatio PRIVATE /utf-8)
endif()

install(TARGETS libpymatio LIBRARY DESTINATION lib)
