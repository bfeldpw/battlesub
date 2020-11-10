#ifndef DEBUG_RENDER_SYSTEM_HPP
#define DEBUG_RENDER_SYSTEM_HPP

#include <entt/entity/registry.hpp>

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Primitives/Circle.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData.h>

#include "fluid_grid.h"
#include "fluid_probes_component.hpp"
#include "physics_component.hpp"

using namespace Magnum;

class DebugRenderSystem
{

    public:

        explicit DebugRenderSystem(entt::registry& _Reg, FluidGrid& _FluidGrid) : Reg_(_Reg),
                                                                                  FluidGrid_(_FluidGrid)
        {
            Mesh_ = MeshTools::compile(Primitives::circle2DSolid(32));
            Shader_.setColor({1.0f, 0.0f, 0.0f, 0.2f});
        }

        DebugRenderSystem() = delete;

        void renderVelocityProbes(const Matrix3& ProjectionMatrix)
        {
            Reg_.view<FluidProbeComponent, PhysicsComponent>().each(
                [&](const auto& _ProbeComp, const auto& _PhysComp)
            {
                Matrix3 TransformationMatrix;
                b2Body* Body = _PhysComp.Body_;

                auto ProbePos = Body->GetWorldPoint({_ProbeComp.ProbeX_, _ProbeComp.ProbeY_});
                auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
                auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbeComp.ProbeX_, _ProbeComp.ProbeY_});
                b2Vec2 Vel = {VelF.x() - VelB.x, VelF.y() - VelB.y};

                TransformationMatrix = Matrix3::translation({ProbePos.x, ProbePos.y})*
                                       Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
                Shader_.setTransformationProjectionMatrix(ProjectionMatrix*TransformationMatrix);
                Shader_.draw(Mesh_);
            });
            Reg_.view<FluidProbesComponent<8>, PhysicsComponent>().each([&](const auto& _ProbesComp, const auto& _PhysComp)
            {
                for (int i=0; i<_ProbesComp.N_; ++i)
                {
                    Matrix3 TransformationMatrix;
                    b2Body* Body = _PhysComp.Body_;

                    auto ProbePos = Body->GetWorldPoint({_ProbesComp.ProbeX_[i], _ProbesComp.ProbeY_[i]});
                    auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
                    auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbesComp.ProbeX_[i], _ProbesComp.ProbeY_[i]});
                    auto NormB = Body->GetWorldVector({_ProbesComp.ProbeNormX_[i], _ProbesComp.ProbeNormY_[i]});
                    b2Vec2 VelR = {VelF.x() - VelB.x, VelF.y() - VelB.y};
                    NormB.Normalize(); // Just in case
                    b2Vec2 Vel = b2Dot(VelR, NormB)*NormB;

                    TransformationMatrix = Matrix3::translation({ProbePos.x, ProbePos.y})*
                                           Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
                    Shader_.setTransformationProjectionMatrix(ProjectionMatrix*TransformationMatrix);
                    Shader_.draw(Mesh_);
                }
            });
        }

    private:

        GL::Mesh Mesh_;
        Shaders::Flat2D Shader_;

        entt::registry& Reg_;
        FluidGrid& FluidGrid_;

};

#endif // DEBUG_RENDER_SYSTEM_HPP
