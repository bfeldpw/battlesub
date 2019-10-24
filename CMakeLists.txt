cmake_minimum_required(VERSION 3.1)

project(BattleSub)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    add_compile_options(-std=c++14 -mtune=native -Wall -pedantic)
else()
    add_compile_options(-std=c++14 -mtune=native -Wall -Wextra -pedantic)
endif()

# Add module path in case this is project root
if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../modules/" ${CMAKE_MODULE_PATH})
endif()

set (BOX2D_LIBRARY_LOCAL "${PROJECT_SOURCE_DIR}/3rdparty/Box2D-2.3.1/install/lib/libBox2D.a")

find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    SceneGraph
    Sdl2Application
    Shaders
    Trade)
    
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(HEADERS
        battle_sub.h
        common.h
        entity.h
        error_handler.h
        landscape.h
        landscape_drawable.h
        landscape_factory.h
        game_object.h
        game_object_factory.h
        memory_pool.h
        message_handler.h
        projectile.h
        projectile_drawable.h
        projectile_factory.h
        sub.h
        sub_drawable.h
        submarine_factory.h
        timer.h
)

set(SOURCES
        battle_sub.cpp
        entity.cpp
        sub.cpp
)

add_executable(battle_sub ${HEADERS} ${SOURCES})

target_include_directories(battle_sub PRIVATE ${MAGNUM_INCLUDE_DIR}})
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/Box2D-2.3.1/install/include")

target_link_libraries(battle_sub PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Trade
    ${BOX2D_LIBRARY_LOCAL})

# install(TARGETS battle_sub DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})
