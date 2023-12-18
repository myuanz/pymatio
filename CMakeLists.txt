cmake_minimum_required(VERSION 3.26)
project(pymatio)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(matio)
set(MODULE "_${PROJECT_NAME}")

find_package(pybind11 REQUIRED)
pybind11_add_module(${MODULE}
        src/libmatio.cpp
        src/pymatio.cpp
)
target_compile_definitions(${MODULE} PRIVATE MODULE=${MODULE})
target_link_libraries(${MODULE} PRIVATE matio)
