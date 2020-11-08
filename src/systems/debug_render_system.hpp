#ifndef DEBUG_RENDER_SYSTEM_HPP
#define DEBUG_RENDER_SYSTEM_HPP

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData.h>

#include "fluid_probes_component.hpp"
#include "physics_component.hpp"

using namespace Magnum;

class DebugRenderSystem
{

    public:

        explicit DebugRenderSystem(entt::registry& _Reg) : Reg_(_Reg)
        {
            Mesh_ = MeshTools::compile(Primitives::circle2DSolid(10));
            Shader_.setColor({1.0f, 0.0f, 0.0f, 0.2f});
        }

        DebugRenderSystem() = delete;

        void renderVelocityProbes(const Matrix3& ProjectionMatrix)
        {
            Reg_.view<FluidProbesComponent, PhysicsComponent>().each([&](const auto& _ProbesComp, const auto& _PhysComp)
            {
                for (int i=0; i<_ProbesComp.N_; ++i)
                {
                    Matrix3 TransformationMatrix;
                    b2Body* Body = _PhysComp.Body_;

                    auto ProbePos = Body->GetWorldPoint({_ProbesComp.ProbeX_[i], _ProbesComp.ProbeY_[i]});

                    TransformationMatrix = Matrix3::translation({ProbePos.x, ProbePos.y});
                    Shader_.setTransformationProjectionMatrix(ProjectionMatrix*TransformationMatrix);
                    Shader_.draw(Mesh_);
                }
            });
        }

    private:

        GL::Mesh Mesh_;
        Shaders::Flat2D Shader_;

        entt::registry& Reg_;

};

#endif // DEBUG_RENDER_SYSTEM_HPP
