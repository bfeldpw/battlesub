cmake_minimum_required(VERSION 3.1)

project(BattleSub)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    add_compile_options(-mtune=native -Wall -pedantic)
else()
    add_compile_options(-mtune=native -Wall -Wextra -pedantic)
endif()

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/3rdparty/modules" ${CMAKE_MODULE_PATH})

set (BOX2D_LIBRARY_LOCAL "${PROJECT_SOURCE_DIR}/install/lib/libBox2D.a")

find_package(Threads)
find_package(Corrade REQUIRED Main)
find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    SceneGraph
    Sdl2Application
    Shaders
    Trade)
find_package(MagnumIntegration REQUIRED ImGui)
    
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(LIB_NOISE_HEADERS
        ${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/src/noise.h
)

add_subdirectory(./src)
