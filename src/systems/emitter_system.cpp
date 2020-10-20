#include "emitter_system.hpp"

#include <Box2D/Box2D.h>
#include <Magnum/Math/Color.h>
#include <entt/entity/helper.hpp>
#include "fluid_source_component.hpp"
#include "game_object_factory.hpp"

void EmitterSystem::emit()
{
    Reg_.view<EmitterComponent>().each([this](auto& _EmComp)
    {
        std::normal_distribution<float> DistAngle(_EmComp.Angle_, _EmComp.AngleStdDev_);
        std::normal_distribution<float> DistScale(_EmComp.Scale_, _EmComp.ScaleStdDev_);

        for (auto i=0; i<_EmComp.Number_; ++i)
        {
            auto Debris = Reg_.create();

            Magnum::Math::Color3 Col = {0.2f, 0.2f, 0.3f};
            if (_EmComp.Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
            {
                Col = {0.1f, 0.1f, 0.2f};
            }

            b2BodyDef BodyDef;
            BodyDef.type = b2_dynamicBody;
            BodyDef.active = true;
            BodyDef.angularDamping = 1.0f;
            BodyDef.linearDamping = 1.0f;
            BodyDef.position.Set(_EmComp.OriginX_+i*0.01f, _EmComp.OriginY_);
            BodyDef.linearVelocity.Set(std::cos(DistAngle(_EmComp.Generator_))*
                                        (_EmComp.Velocity_+_EmComp.VelocityStdDev_),
                                        std::sin(DistAngle(_EmComp.Generator_))*
                                        (_EmComp.Velocity_+_EmComp.VelocityStdDev_));
            BodyDef.angularVelocity = 0.5f*DistAngle(_EmComp.Generator_);
            Reg_.ctx<GameObjectFactory>().create(Debris, this, _EmComp.Type_,
                                                DrawableGroupsTypeE::WEAPON, Col, BodyDef);
            Reg_.emplace<FluidSourceComponent>(Debris, 10.0f, 1.0f);

            //--- Adjust physics body for non-self collisions ---//
            auto& PhysComp = Reg_.get<PhysicsComponent>(Debris);

            b2Filter Filter;
            Filter.categoryBits = 0x0002;
            Filter.maskBits = 0xFFF9;
            PhysComp.Body_->GetFixtureList()->SetFilterData(Filter);

            //--- Adjust visuals for varying size ---//
            auto& VisComp = Reg_.get<VisualsComponent>(Debris);

            auto Size = DistScale(_EmComp.Generator_);
            auto Asymmetry = 0.5f;
            if (_EmComp.Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
            {
                Size *= 0.75f;
                Asymmetry = 1.5f;
            }
            VisComp.Visuals_->setScaling({Size, Size+Asymmetry*DistScale(_EmComp.Generator_)});
        }
        _EmComp.Counter_ += _EmComp.Number_;
        if (_EmComp.Counter_ > _EmComp.Number_)
        {
            auto e = entt::to_entity(Reg_, _EmComp);
            Reg_.destroy(e);
        }
    });
}
