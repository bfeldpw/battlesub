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

        explicit DebugRenderSystem(entt::registry& _Reg);
        DebugRenderSystem() = delete;

        void renderVelocityProbes(const Matrix3& _ProjectionMatrix);
        void renderVelocityVectors(const Matrix3& _ProjectionMatrix);

    private:

        entt::registry& Reg_;

        static constexpr int VEL_VEC_ARRAY_SIZE = (FLUID_GRID_ARRAY_SIZE >> (FluidGrid::VELOCITY_READBACK_SUBSAMPLE_XY-1));
        // static constexpr int VEL_VEC_ARRAY_SIZE = (FLUID_GRID_ARRAY_SIZE >> (FluidGrid::VELOCITY_READBACK_SUBSAMPLE_XY));
        typedef std::array<float, VEL_VEC_ARRAY_SIZE> VelocityVectorFieldDataType;

        VelocityVectorFieldDataType VelocityVectors_;

        GL::Mesh MeshCircle_;
        GL::Mesh MeshLine_;
        Shaders::Flat2D Shader_;

};

#endif // DEBUG_RENDER_SYSTEM_HPP
