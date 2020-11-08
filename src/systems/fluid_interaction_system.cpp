#include "fluid_interaction_system.hpp"

#include "fluid_probes_component.hpp"
#include "fluid_source_component.hpp"
#include "physics_component.hpp"

void FluidInteractionSystem::addSources()
{
    Reg_.view<PhysicsComponent, FluidSourceComponent>().each([&](const auto& _PhysComp, const auto& _FluidComp)
    {
        auto Pos = _PhysComp.Body_->GetPosition();
        auto Vel = _PhysComp.Body_->GetLinearVelocity();

        FluidGrid_.addDensity(Pos.x, Pos.y,
                            Pos.x-Vel.x*1.0f/30.0f, Pos.y-Vel.y*1.0f/30.0f,
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
    Reg_.view<FluidProbesComponent, PhysicsComponent>().each(
        [&](const auto& _ProbesComp, const auto& _PhysComp)
    {

        for (int i=0; i<_ProbesComp.N_; ++i)
        {

            const float f = _ProbesComp.MassFactor_/_ProbesComp.N_*60.0f;
            const float m = _PhysComp.Body_->GetMass();

            b2Vec2 ProbePos = {_ProbesComp.ProbeX_[i], _ProbesComp.ProbeY_[i]};
            b2Vec2 ProbePosGlobal = _PhysComp.Body_->GetWorldPoint(ProbePos);

            auto VelF = FluidGrid_.getVelocity((ProbePosGlobal.x+256.0f)*4.0f, (ProbePosGlobal.y+128.0f)*4.0f);

            auto VelB = _PhysComp.Body_->GetLinearVelocityFromLocalPoint(ProbePos);
            b2Vec2 Vel = {VelF.x() - VelB.x, VelF.y() - VelB.y};

            _PhysComp.Body_->ApplyForce({Vel.x*f, Vel.y*f}, _PhysComp.Body_->GetWorldPoint(ProbePos), true);
        }
    });
}
