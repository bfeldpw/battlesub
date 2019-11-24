cmake_minimum_required(VERSION 3.1)

project(BattleSub)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    add_compile_options(-std=c++14 -mtune=native -Wall -pedantic)
else()
    add_compile_options(-std=c++14 -mtune=native -Wall -Wextra -pedantic)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set (BOX2D_LIBRARY_LOCAL "${PROJECT_SOURCE_DIR}/3rdparty/Box2D-2.3.1/install_debug/lib/libBox2D.a")
else()
    set (BOX2D_LIBRARY_LOCAL "${PROJECT_SOURCE_DIR}/3rdparty/Box2D-2.3.1/install/lib/libBox2D.a")
endif()

find_package(Magnum REQUIRED
    GL
    MeshTools
    Primitives
    SceneGraph
    Sdl2Application
    Shaders
    Trade)
    
set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

set(LIB_NOISE_HEADERS
        ${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/src/noise.h
)

set(HEADERS
        battle_sub.h
        common.h
        contact_listener.h
        debris_factory.h
        density_shader.h
        drawable_generic.h
        emitter.h
        emitter_factory.h
        entity.h
        entity_factory.h
        error_handler.h
        fluid_grid.h
        game_object.h
        global_emitter_factory.h
        global_factories.h
        global_resources.h
        landscape_factory.h
        memory_pool.h
        message_handler.h
        projectile_factory.h
        resource_storage.h
        submarine.h
        submarine_definition.h
        submarine_factory.h
        timer.h
        world_def.h
)

set(LIB_NOISE_LIBRARY
        ${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/build/src/libnoise2d.so
)

set(SOURCES
        battle_sub.cpp
        emitter.cpp
        entity.cpp
        game_object.cpp
        global_emitter_factory.cpp
        global_factories.cpp
        global_resources.cpp
        resource_storage.cpp
        submarine.cpp
)

add_executable(battle_sub ${LIB_NOISE_HEADERS} ${HEADERS} ${SOURCES})

target_include_directories(battle_sub PRIVATE ${MAGNUM_INCLUDE_DIR}})
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/Box2D-2.3.1/install/include")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/src/")

target_link_libraries(battle_sub PRIVATE
    OpenGL::GL
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::SceneGraph
    Magnum::Shaders
    ${BOX2D_LIBRARY_LOCAL}
    ${LIB_NOISE_LIBRARY}
)

# install(TARGETS battle_sub DESTINATION ${MAGNUM_BINARY_INSTALL_DIR})

