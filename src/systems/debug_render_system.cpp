#include "debug_render_system.hpp"

#include <Magnum/Primitives/Circle.h>
#include <Magnum/Primitives/Line.h>
#include <Magnum/Trade/MeshData.h>

#include "fluid_probes_component.hpp"
#include "physics_component.hpp"

DebugRenderSystem::DebugRenderSystem(entt::registry& _Reg, FluidGrid& _FluidGrid) :
                                     Reg_(_Reg),
                                     FluidGrid_(_FluidGrid),
                                     MeshCircle_(MeshTools::compile(Primitives::circle2DSolid(16))),
                                     MeshLine_(MeshTools::compile(Primitives::line2D()))
{}

void DebugRenderSystem::renderVelocityProbes(const Matrix3& ProjectionMatrix)
{
    Reg_.view<FluidProbeComponent, PhysicsComponent>().each(
        [&](const auto& _ProbeComp, const auto& _PhysComp)
    {
        Matrix3 MatTransformation;
        Matrix3 MatRotation;
        b2Body* Body = _PhysComp.Body_;

        auto ProbePos = Body->GetWorldPoint({_ProbeComp.X_, _ProbeComp.Y_});
        auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
        auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbeComp.X_, _ProbeComp.Y_});
        b2Vec2 Vel = {VelF.x() - VelB.x, VelF.y() - VelB.y};

        MatTransformation = Matrix3::translation({ProbePos.x, ProbePos.y})*
                            Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
        MatRotation = Matrix3::rotation(Magnum::Rad(std::atan2(VelF.y(), VelF.x())));

        Shader_.setTransformationProjectionMatrix(ProjectionMatrix*MatTransformation)
               .setColor({1.0f, 0.0f, 0.0f, 0.2f})
               .draw(MeshCircle_);
        Shader_.setTransformationProjectionMatrix(ProjectionMatrix*MatTransformation*MatRotation)
               .setColor({1.0f, 0.0f, 0.0f, 0.5f})
               .draw(MeshLine_);
    });
    Reg_.view<FluidProbesComponent<8>, PhysicsComponent>().each([&](const auto& _ProbesComp, const auto& _PhysComp)
    {
        for (int i=0; i<_ProbesComp.N_; ++i)
        {
            Matrix3 MatTransformation;
            Matrix3 MatRotation;
            b2Body* Body = _PhysComp.Body_;

            auto ProbePos = Body->GetWorldPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
            auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto NormB = Body->GetWorldVector({_ProbesComp.NormX_[i], _ProbesComp.NormY_[i]});
            b2Vec2 VelR = {VelF.x() - VelB.x, VelF.y() - VelB.y};
            b2Vec2 Vel = b2Dot(VelR, NormB)*NormB;

            MatTransformation = Matrix3::translation({ProbePos.x, ProbePos.y})*
                                Matrix3::scaling({0.1f+Vel.Length()*0.2f, 0.1f+Vel.Length()*0.2f});
            MatRotation = Matrix3::rotation(Magnum::Rad(std::atan2(VelF.y(), VelF.x())));

            Shader_.setTransformationProjectionMatrix(ProjectionMatrix*MatTransformation)
                   .setColor({1.0f, 0.0f, 0.0f, 0.2f})
                   .draw(MeshCircle_);
            Shader_.setTransformationProjectionMatrix(ProjectionMatrix*MatTransformation*MatRotation)
                   .setColor({1.0f, 0.0f, 0.0f, 0.5f})
                   .draw(MeshLine_);
        }
    });
}
