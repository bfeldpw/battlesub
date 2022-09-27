#include "fluid_interaction_system.hpp"

#include "fluid_grid.h"
#include "fluid_source_component.hpp"
#include "physics_component.hpp"

void FluidInteractionSystem::addFluidProbe(FluidProbeComponent& _Probe, const float _M, const float _X, const float _Y)
{
    _Probe.M_ = _M;
    _Probe.X_ = _X;
    _Probe.Y_ = _Y;
}

void FluidInteractionSystem::addSources()
{
    auto& Fluid = Reg_.ctx().at<FluidGrid>();
    Reg_.view<PhysicsComponent, FluidSourceComponent>().each([&](const auto& _PhysComp, const auto& _FluidComp)
    {
        auto Pos = _PhysComp.Body_->GetPosition();
        auto Vel = _PhysComp.Body_->GetLinearVelocity();
        auto VelBP = _FluidComp.VelocityBackProjection_;
        auto DenBP = _FluidComp.DensityBackProjection_;
        auto R = (_FluidComp.DensityStaticR_ + _FluidComp.DensityDynamicR_*Vel.Length());
        auto G = (_FluidComp.DensityStaticG_ + _FluidComp.DensityDynamicG_*Vel.Length());
        auto B = (_FluidComp.DensityStaticB_ + _FluidComp.DensityDynamicB_*Vel.Length());

        Fluid.addDensityLn(Pos.x, Pos.y,
                           Pos.x-Vel.x*DenBP, Pos.y-Vel.y*DenBP,
                           R, G, B)
             .addVelocity(Pos.x, Pos.y, Vel.x, Vel.y,
                       Pos.x-Vel.x*VelBP,
                       Pos.y-Vel.y*VelBP,
                       Vel.x, Vel.y,
                       _FluidComp.VelocityWeight_);
    });
}

void FluidInteractionSystem::applyForces()
{
    auto& Fluid = Reg_.ctx().at<FluidGrid>();
    Reg_.view<FluidProbeComponent, PhysicsComponent>().each(
        [&](const auto& _ProbeComp, const auto& _PhysComp)
    {

        const float f = _ProbeComp.M_*60.0f;

        b2Body* Body = _PhysComp.Body_;

        auto ProbePos = Body->GetWorldPoint({_ProbeComp.X_, _ProbeComp.Y_});
        auto VelF = Fluid.getVelocity(ProbePos.x, ProbePos.y);
        auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbeComp.X_, _ProbeComp.Y_});
        b2Vec2 Vel = {VelF.x() - VelB.x, VelF.y() - VelB.y};

        _PhysComp.Body_->ApplyForce({Vel.x*f, Vel.y*f}, ProbePos, true);
    });
    Reg_.view<FluidProbesComponent<8>, PhysicsComponent>().each(
        [&](const auto& _ProbesComp, const auto& _PhysComp)
    {

        for (int i=0; i<_ProbesComp.N_; ++i)
        {

            const float f = _ProbesComp.M_/_ProbesComp.N_*60.0f;

            b2Body* Body = _PhysComp.Body_;

            auto ProbePos = Body->GetWorldPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto VelF = Fluid.getVelocity(ProbePos.x, ProbePos.y);
            auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto NormB = Body->GetWorldVector({_ProbesComp.NormX_[i], _ProbesComp.NormY_[i]});
            b2Vec2 VelR = {VelF.x() - VelB.x, VelF.y() - VelB.y};
            b2Vec2 Vel = b2Dot(VelR, NormB)*NormB;

            _PhysComp.Body_->ApplyForce({Vel.x*f, Vel.y*f}, ProbePos, true);
        }
    });
}
