cmake_minimum_required(VERSION 3.18...3.27 FATAL_ERROR)
# 设置默认构建类型为静态库
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

# <utils>

macro(get_all_targets_recursive targets dir)
  get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
  foreach(subdir ${subdirectories})
    get_all_targets_recursive(${targets} ${subdir})
  endforeach()

  get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${targets} ${current_targets})
endmacro()

function(print_all_targets)
  set(targets)
  get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
  message("All targets: ${targets}")
endfunction()


function(print_target_properties TARGET_NAME)
  if(NOT TARGET ${TARGET_NAME})
    message(STATUS "目标 '${TARGET_NAME}' 不存在")
    return()
  endif()

  message(STATUS "目标 '${TARGET_NAME}' 的属性:")

  # 获取目标类型
  get_property(TARGET_TYPE TARGET ${TARGET_NAME} PROPERTY TYPE)
  message(STATUS "  类型: ${TARGET_TYPE}")

  # 获取源文件
  get_property(TARGET_SOURCES TARGET ${TARGET_NAME} PROPERTY SOURCES)
  message(STATUS "  源文件: ${TARGET_SOURCES}")

  # 获取包含目录
  get_property(TARGET_INCLUDE_DIRS TARGET ${TARGET_NAME} PROPERTY INCLUDE_DIRECTORIES)
  message(STATUS "  包含目录: ${TARGET_INCLUDE_DIRS}")

  # 获取链接库
  get_property(TARGET_LINK_LIBRARIES TARGET ${TARGET_NAME} PROPERTY LINK_LIBRARIES)
  message(STATUS "  链接库: ${TARGET_LINK_LIBRARIES}")

  # 获取编译定义
  get_property(TARGET_COMPILE_DEFINITIONS TARGET ${TARGET_NAME} PROPERTY COMPILE_DEFINITIONS)
  message(STATUS "  编译定义: ${TARGET_COMPILE_DEFINITIONS}")

  # 获取编译选项
  get_property(TARGET_COMPILE_OPTIONS TARGET ${TARGET_NAME} PROPERTY COMPILE_OPTIONS)
  message(STATUS "  编译选项: ${TARGET_COMPILE_OPTIONS}")

  # 获取链接选项
  get_property(TARGET_LINK_OPTIONS TARGET ${TARGET_NAME} PROPERTY LINK_OPTIONS)
  message(STATUS "  链接选项: ${TARGET_LINK_OPTIONS}")

  # 如果是库，获取输出名称
  if(TARGET_TYPE STREQUAL "STATIC_LIBRARY" OR TARGET_TYPE STREQUAL "SHARED_LIBRARY")
    get_property(TARGET_OUTPUT_NAME TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME)
    message(STATUS "  输出名称: ${TARGET_OUTPUT_NAME}")
  endif()

  # 获取自定义属性（如果有的话）
  get_property(TARGET_CUSTOM_PROPERTIES TARGET ${TARGET_NAME} PROPERTY CUSTOM_PROPERTIES)
  if(TARGET_CUSTOM_PROPERTIES)
    message(STATUS "  自定义属性: ${TARGET_CUSTOM_PROPERTIES}")
  endif()
endfunction()
# </utils>

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# <setup nanobind>-----------------------------
set(DEV_MODULE Development)
find_package(Python 3.8
  REQUIRED COMPONENTS Interpreter Development.Module
  OPTIONAL_COMPONENTS Development.SABIModule)

if(Python_FOUND)
  if(WIN32)
    set(Python_LIBRARY ${Python_INCLUDE_DIRS}/../libs)
    set(Python_LIBRARIES ${Python_LIBRARY}/python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}.lib)
    link_directories(${Python_LIBRARY})
  endif()

  message(STATUS "Python_EXECUTABLE: ${Python_EXECUTABLE}")
  message(STATUS "Python_LIBRARY: ${Python_LIBRARY}")
  message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}")
  message(STATUS "Python_INCLUDE_DIRS: ${Python_INCLUDE_DIRS}")
  

  # In some cases, the extension name (.so) may be lost, so specify it manually
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import sysconfig; print(sysconfig.get_config_var('EXT_SUFFIX'))"
    OUTPUT_VARIABLE PYTHON_MODULE_EXTENSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
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

if(MSVC)
  add_compile_options(/utf-8 /wd4819)
endif()

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


nanobind_add_module(libpymatio FREE_THREADED STABLE_ABI NB_STATIC src/pymatio.cpp)
nanobind_add_stub(
  libpymatio-stub
  MODULE libpymatio
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/__init__.pyi
  PYTHON_PATH $<TARGET_FILE_DIR:libpymatio>
  DEPENDS libpymatio
  MARKER_FILE ${CMAKE_CURRENT_BINARY_DIR}/py.typed
)

set_target_properties(libpymatio PROPERTIES
  SUFFIX "${PYTHON_MODULE_EXTENSION}"
)

# <include CPM and packages>-----------------------------

if(EXISTS "/host/home/myuan/.cache/CPM/")
  set(CPM_SOURCE_CACHE "/host/home/myuan/.cache/CPM/")
else()
  set(CPM_SOURCE_CACHE $ENV{HOME}/.cache/CPM/)
endif()

message(STATUS "CPM_SOURCE_CACHE: ${CPM_SOURCE_CACHE}")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
  NAME ZLIB
  GITHUB_REPOSITORY madler/zlib
  GIT_TAG v1.3.1

  OPTIONS
  "ZLIB_BUILD_EXAMPLES OFF"
)
print_all_targets()
print_target_properties(zlib)

if (ZLIB_ADDED) 
  # target_include_directories(zlib PUBLIC ${ZLIB_BINARY_DIR})
  set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

  target_include_directories(zlibstatic PUBLIC ${ZLIB_BINARY_DIR})
  configure_file(${ZLIB_BINARY_DIR}/zconf.h ${ZLIB_SOURCE_DIR} COPYONLY)

  add_library(zlib-static ALIAS zlibstatic)
endif()
print_all_targets()
print_target_properties(zlib-static)
message(STATUS "ZLIB_SOURCE_DIR: ${ZLIB_SOURCE_DIR};${ZLIB_BINARY_DIR}")


set(HDF5_VERSION_STRING "1.14.5")

message("H5_ZLIB_INCLUDE_DIRS: ${H5_ZLIB_INCLUDE_DIRS}")
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

    # "ZLIB_LIBRARIES libz"
    "H5_ZLIB_INCLUDE_DIRS ${ZLIB_SOURCE_DIR}"
    "H5_ZLIB_HEADER zlib.h"
    "HDF5_USE_ZLIB_STATIC ON"
    "H5_ZLIB_STATIC_LIBRARY zlib-static"

    "HDF5_EXTERNALLY_CONFIGURED ON"

    "HDF5_BUILD_STATIC_LIBS ON"
    "HDF5_BUILD_SHARED_LIBS OFF"
    "CMAKE_POSITION_INDEPENDENT_CODE ON"
)
if(HDF5_ADDED)
  message("HDF5 added ${HDF5_LIBRARIES}")
  if(TARGET hdf5-static)
    set_target_properties(hdf5-static PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(hdf5-static PRIVATE zlib-static)
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
    "HDF5_USE_STATIC_LIBRARIES ON"

    "MATIO_SHARED OFF"
    "CMAKE_POSITION_INDEPENDENT_CODE ON"

)
if (matio_ADDED)
  set_target_properties(matio PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(matio PUBLIC ${HDF5_LIBRARY_NAME} zlib-static)
endif()

CPMAddPackage("gh:fmtlib/fmt#11.0.2")
if(fmt_ADDED)
  set_target_properties(fmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
# </include CPM and packages>-----------------------------


target_link_libraries(libpymatio PRIVATE 
  fmt
  zlib-static
  ${HDF5_LIBRARY_NAME}
  matio
)
set_target_properties(libpymatio PROPERTIES POSITION_INDEPENDENT_CODE ON)

print_target_properties(libpymatio)

# set_target_properties(libpymatio PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++")

# 只安装 Python 模块
install(TARGETS libpymatio LIBRARY DESTINATION pymatio)

# 明确不安装其他文件
install(CODE "
  file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/include)
  file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/lib)
  file(REMOVE_RECURSE \${CMAKE_INSTALL_PREFIX}/bin)
")

install(
  FILES 
    ${CMAKE_CURRENT_BINARY_DIR}/py.typed
    ${CMAKE_CURRENT_BINARY_DIR}/__init__.pyi
  DESTINATION pymatio
)
