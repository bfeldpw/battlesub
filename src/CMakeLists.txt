set(HEADERS
        battle_sub.h
        camera_dynamics.h
        common.h
        contact_listener.h
        debris_factory.h
        density_advection_shader.h
        density_diffusion_shader.h
        density_display_shader.h
        density_sources_shader.h
        drawable_generic.h
        emitter.h
        emitter_factory.h
        entity.h
        entity_factory.h
        error_handler.h
        fluid_final_composition_shader.h
        fluid_grid.h
        game_object.h
        global_emitter_factory.h
        global_factories.h
        global_resources.h
        landscape_factory.h
        main_display_shader.h
        memory_pool.h
        message_handler.h
        projectile_factory.h
        resource_storage.h
        shader_path.h
        submarine.h
        submarine_definition.h
        submarine_factory.h
        timer.h
        velocity_advection_shader.h
        velocity_diffusion_shader.h
        velocity_display_shader.h
        velocity_sources_shader.h
        world_def.h
)

set(LIB_NOISE_LIBRARY
        ${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/build/src/libnoise2d.so
)

set(SOURCES
        ${ImGui_INCLUDE_DIR}/imgui.cpp
        battle_sub.cpp
        camera_dynamics.cpp
        emitter.cpp
        entity.cpp
        fluid_grid.cpp
        game_object.cpp
        global_emitter_factory.cpp
        global_factories.cpp
        global_resources.cpp
        resource_storage.cpp
        submarine.cpp
)

add_executable(battle_sub ${LIB_NOISE_HEADERS} ${HEADERS} ${SOURCES})

target_include_directories(battle_sub PRIVATE ${MAGNUM_INCLUDE_DIR})
target_include_directories(battle_sub PRIVATE ${ImGui_INCLUDE_DIR})
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/box2d/Box2D/Box2D")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/src/")

target_link_libraries(battle_sub PRIVATE
    Threads::Threads
    OpenGL::GL
    Corrade::Main
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::SceneGraph
    Magnum::Shaders
    MagnumIntegration::ImGui
    ${BOX2D_LIBRARY_LOCAL}
    ${LIB_NOISE_LIBRARY}
)


set_property(TARGET battle_sub PROPERTY CXX_STANDARD 17)

install(TARGETS battle_sub DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/shaders/" DESTINATION "${PROJECT_SOURCE_DIR}/install/share/shaders/")
