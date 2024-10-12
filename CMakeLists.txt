cmake_minimum_required(VERSION 3.18...3.27 FATAL_ERROR)
# 设置默认构建类型为静态库
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

# <include CPM and packages>-----------------------------

if(EXISTS "/host/home/myuan/.cache/CPM/")
  set(CPM_SOURCE_CACHE "/host/home/myuan/.cache/CPM/")
else()
  set(CPM_SOURCE_CACHE $ENV{HOME}/.cache/CPM/)
endif()

message(STATUS "CPM_SOURCE_CACHE: ${CPM_SOURCE_CACHE}")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:madler/zlib@1.3.1")

set(HDF5_VERSION_STRING "1.14.5")
CPMAddPackage(
  NAME HDF5
  GITHUB_REPOSITORY HDFGroup/hdf5
  GIT_TAG hdf5_1.14.5
  OPTIONS
    "HDF5_ENABLE_Z_LIB_SUPPORT ON"
    "HDF5_BUILD_TOOLS OFF"
    "HDF5_BUILD_EXAMPLES OFF"
    "HDF5_ENABLE_SZIP_ENCODING OFF"
    "HDF5_ENABLE_SZIP_SUPPORT OFF"

    "HDF5_BUILD_STATIC_LIBS ON"
    "HDF5_BUILD_SHARED_LIBS OFF"
)
if(HDF5_ADDED)
  message("HDF5 added ${HDF5_LIBRARIES}")
  if(TARGET hdf5-static)
    target_link_directories(hdf5-static INTERFACE ${HDF5_BINARY_DIR}/src)
  endif()

  if(BUILD_SHARED_LIBS)
    target_link_directories(hdf5-shared INTERFACE ${HDF5_BINARY_DIR}/src)
  endif()

  if(BUILD_SHARED_LIBS)
    set(HDF5_LIBRARY_NAME hdf5-shared)
  else()
    set(HDF5_LIBRARY_NAME hdf5-static)
  endif()
  message("HDF5_LIBRARY_NAME: ${HDF5_LIBRARY_NAME}")
endif()


CPMAddPackage(
  NAME matio
  GITHUB_REPOSITORY tbeu/matio
  GIT_TAG v1.5.27
  OPTIONS
    "MATIO_WITH_HDF5 ON"
    "MATIO_WITH_ZLIB ON"
    "MATIO_MAT73 ON"
    "MATIO_DEFAULT_FILE_VERSION 5"
    "HDF5_VERSION ${HDF5_VERSION_STRING}"

    "MATIO_BUILD_STATIC_LIBS ON"
    "MATIO_BUILD_SHARED_LIBS OFF"
)
if (matio_ADDED)
  target_link_libraries(matio PUBLIC ${HDF5_LIBRARY_NAME})
endif()

CPMAddPackage("gh:fmtlib/fmt#11.0.2")

set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
# </include CPM and packages>-----------------------------


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# <setup nanobind>-----------------------------
set(DEV_MODULE Development)
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

if(Python_FOUND)
  if(WIN32)
    set(Python_LIBRARY ${Python_EXECUTABLE}/../libs)
    set(Python_LIBRARIES ${Python_LIBRARY}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.lib)
    link_directories(${Python_LIBRARY})

  endif()
  message(STATUS "Python_EXECUTABLE: ${Python_EXECUTABLE}")
  message(STATUS "Python_LIBRARY: ${Python_LIBRARY}")
  message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}")
  message(STATUS "Python_INCLUDE_DIRS: ${Python_INCLUDE_DIRS}")
  
endif()

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
link_directories(build)


nanobind_add_module(libpymatio NB_STATIC src/pymatio.cpp)

target_link_libraries(libpymatio PRIVATE 
  fmt
  ${HDF5_LIBRARY_NAME}
  matio
)

if (WIN32)
  target_link_libraries(libpymatio PRIVATE)
endif()

target_compile_definitions(libpymatio PRIVATE UNICODE)
if(MSVC)
    target_compile_options(libpymatio PRIVATE /utf-8)
endif()

set_target_properties(libpymatio PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")

# 只安装 Python 模块
install(TARGETS libpymatio DESTINATION .)

# 明确不安装其他文件
install(CODE "
    file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/include)
    file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/lib)
    file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/bin)
")
