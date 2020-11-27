set(HEADERS
        components/emitter_component.hpp
        components/fluid_probes_component.hpp
        components/fluid_source_component.hpp
        components/parent_component.hpp
        components/physics_component.hpp
        components/status_component.hpp
        components/visuals_component.hpp
        systems/debug_render_system.hpp
        systems/emitter_system.hpp
        systems/fluid_interaction_system.hpp
        systems/game_object_factory.hpp
        advect1d_shader.h
        advect2d_shader.h
        battle_sub.h
        camera_dynamics.h
        common.h
        contact_listener.h
        density_display_shader.h
        density_sources_shader.h
        drawable_generic.h
        entity.h
        entity_factory.h
        error_handler.h
        fluid_final_composition_shader.h
        fluid_grid.h
        global_factories.h
        global_resources.h
        jacobi_shader.h
        main_display_shader.h
        memory_pool.h
        message_handler.h
        resource_storage.h
        shader_path.h
        source1d_shader.h
        source2d_shader.h
        submarine.h
        submarine_definition.h
        submarine_factory.h
        texture_2d32f_render_2d32f_shader.h
        timer.h
        velocity_display_shader.h
        velocity_divergence_shader.h
        velocity_gradient_substraction_shader.h
        velocity_sources_shader.h
        world_def.h
)

set(LIB_NOISE_LIBRARY
        ${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/build/src/libnoise2d.so
)

set(SOURCES
        ${ImGui_INCLUDE_DIR}/imgui.cpp
        components/emitter_component.cpp
        systems/debug_render_system.cpp
        systems/emitter_system.cpp
        systems/fluid_interaction_system.cpp
        systems/game_object_factory.cpp
        battle_sub.cpp
        camera_dynamics.cpp
        contact_listener.cpp
        entity.cpp
        fluid_grid.cpp
        global_factories.cpp
        global_resources.cpp
        resource_storage.cpp
        submarine.cpp
)

add_executable(battle_sub ${LIB_NOISE_HEADERS} ${HEADERS} ${SOURCES})

target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/components/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/systems/")
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
