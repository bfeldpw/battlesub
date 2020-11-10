#ifndef DEBUG_RENDER_SYSTEM_HPP
#define DEBUG_RENDER_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Shaders/Flat.h>

#include "fluid_grid.h"

using namespace Magnum;

class DebugRenderSystem
{

    public:

        explicit DebugRenderSystem(entt::registry& _Reg, FluidGrid& _FluidGrid);
        DebugRenderSystem() = delete;

        void renderVelocityProbes(const Matrix3& ProjectionMatrix);

    private:

        entt::registry& Reg_;
        FluidGrid& FluidGrid_;

        GL::Mesh MeshCircle_;
        GL::Mesh MeshLine_;
        Shaders::Flat2D Shader_;

};

#endif // DEBUG_RENDER_SYSTEM_HPP
