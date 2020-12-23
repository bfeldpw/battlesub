#include "fluid_interaction_system.hpp"

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
    Reg_.view<PhysicsComponent, FluidSourceComponent>().each([&](const auto& _PhysComp, const auto& _FluidComp)
    {
        auto Pos = _PhysComp.Body_->GetPosition();
        auto Vel = _PhysComp.Body_->GetLinearVelocity();

        FluidGrid_.addDensity(Pos.x, Pos.y,
                            Pos.x-Vel.x*1.0f/60.0f, Pos.y-Vel.y*1.0f/60.0f,
                            Vel.Length()*_FluidComp.DensityWeight_)
                  .addVelocity(Pos.x, Pos.y, Vel.x, Vel.y,
                            Pos.x-Vel.x*_FluidComp.VelocityBackProjection_,
                            Pos.y-Vel.y*_FluidComp.VelocityBackProjection_,
                            Vel.x, Vel.y,
                            _FluidComp.VelocityWeight_);
    });
}

void FluidInteractionSystem::applyForces()
{
    Reg_.view<FluidProbeComponent, PhysicsComponent>().each(
        [&](const auto& _ProbeComp, const auto& _PhysComp)
    {

        const float f = _ProbeComp.M_*60.0f;

        b2Body* Body = _PhysComp.Body_;

        auto ProbePos = Body->GetWorldPoint({_ProbeComp.X_, _ProbeComp.Y_});
        auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
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
            auto VelF = FluidGrid_.getVelocity(ProbePos.x, ProbePos.y);
            auto VelB = Body->GetLinearVelocityFromLocalPoint({_ProbesComp.X_[i], _ProbesComp.Y_[i]});
            auto NormB = Body->GetWorldVector({_ProbesComp.NormX_[i], _ProbesComp.NormY_[i]});
            b2Vec2 VelR = {VelF.x() - VelB.x, VelF.y() - VelB.y};
            b2Vec2 Vel = b2Dot(VelR, NormB)*NormB;

            _PhysComp.Body_->ApplyForce({Vel.x*f, Vel.y*f}, ProbePos, true);
        }
    });
}
