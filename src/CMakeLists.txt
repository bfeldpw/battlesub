set(HEADERS
        bindings/fluid_source_component_lua.hpp
        bindings/status_component_lua.hpp
        components/emitter_component.hpp
        components/fluid_probes_component.hpp
        components/fluid_source_component.hpp
        components/parent_component.hpp
        components/physics_component.hpp
        components/status_component.hpp
        components/visuals_component.hpp
        gui_adapter/fluid_source_component_config_gui_adapter.hpp
        gui_adapter/status_component_config_gui_adapter.hpp
        shaders/advect_shader.h
        shaders/density_display_shader.h
        shaders/density_sources_shader.h
        shaders/fluid_final_composition_shader.h
        shaders/jacobi_shader.h
        shaders/main_display_shader.h
        shaders/source_shader.h
        shaders/texture_2d32f_render_2d32f_shader.h
        shaders/velocity_display_shader.h
        shaders/velocity_divergence_shader.h
        shaders/velocity_gradient_substraction_shader.h
        shaders/velocity_sources_shader.h
        systems/boid_system.hpp
        systems/debug_render_system.hpp
        systems/emitter_system.hpp
        systems/fluid_interaction_system.hpp
        systems/game_object_system.hpp
        battle_sub.h
        camera_dynamics.h
        common.h
        contact_listener.h
        drawable_generic.h
        fluid_grid.h
        global_resources.h
        lua_manager.hpp
        lua_path.hpp
        message_handler.hpp
        resource_storage.h
        shader_path.h
        submarine.h
        submarine_definition.h
        timer.h
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
        systems/game_object_system.cpp
        battle_sub.cpp
        camera_dynamics.cpp
        contact_listener.cpp
        fluid_grid.cpp
        global_resources.cpp
        resource_storage.cpp
        submarine.cpp
)

add_executable(battle_sub ${LIB_NOISE_HEADERS} ${HEADERS} ${SOURCES})

target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/bindings/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/components/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/gui_adapter/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/shaders/")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/src/systems/")
target_include_directories(battle_sub PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(battle_sub PRIVATE ${MAGNUM_INCLUDE_DIR})
target_include_directories(battle_sub PRIVATE ${ImGui_INCLUDE_DIR})
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/box2d/include/box2d")
target_include_directories(battle_sub PRIVATE "${PROJECT_SOURCE_DIR}/3rdparty/libnoise2d/src/")

target_link_libraries(battle_sub PRIVATE
    Threads::Threads
    OpenGL::GL
    Corrade::Main
    Magnum::Application
    Magnum::DebugTools
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::Primitives
    Magnum::SceneGraph
    Magnum::Shaders
    MagnumIntegration::ImGui
    ${LUA_LIBRARIES}
    ${BOX2D_LIBRARY_LOCAL}
    ${LIB_NOISE_LIBRARY}
)

set_property(TARGET battle_sub PROPERTY CXX_STANDARD 17)

install(TARGETS battle_sub DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/shaders/glsl/" DESTINATION "${PROJECT_SOURCE_DIR}/install/share/shaders/")
install(DIRECTORY "${PROJECT_SOURCE_DIR}/src/lua/" DESTINATION "${PROJECT_SOURCE_DIR}/install/share/lua/")
