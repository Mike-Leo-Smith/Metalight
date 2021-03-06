cmake_minimum_required(VERSION 3.17..3.18)
cmake_policy(VERSION 3.17)

project(LuisaRender LANGUAGES C CXX VERSION 0.4.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/bin/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(CheckLanguage)

check_language(OBJCXX)
if (CMAKE_OBJCXX_COMPILER AND ${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    set(LUISA_RENDER_ENABLE_METAL_DEFAULT ON)
else ()
    set(LUISA_RENDER_ENABLE_METAL_DEFAULT OFF)
endif ()

find_package(CUDAToolkit)
if (CUDAToolkit_FOUND)
    set(LUISA_RENDER_ENABLE_CUDA_DEFAULT ON)
else ()
    set(LUISA_RENDER_ENABLE_CUDA_DEFAULT OFF)
endif ()

option(LUISA_RENDER_ENABLE_METAL "Build with Apple Metal support" ${LUISA_RENDER_ENABLE_METAL_DEFAULT})
option(LUISA_RENDER_ENABLE_CUDA "Build with NVIDIA CUDA support" ${LUISA_RENDER_ENABLE_CUDA_DEFAULT})
option(LUISA_RENDER_ENABLE_OPENCL "Build with OpenCL support" ON)

add_subdirectory(src)
