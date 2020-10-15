#include "emitter.h"

std::mt19937 Emitter::Generator_;

void Emitter::emit(entt::registry& _Reg)
{
    std::normal_distribution<float> DistAngle(Angle_, AngleStdDev_);
    std::normal_distribution<float> DistScale(Scale_, ScaleStdDev_);

    for (auto i=0; i<Number_; ++i)
    {
        auto Debris = _Reg.create();

        Color3 Col = {0.2f, 0.2f, 0.3f};
        if (Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
        {
            Col = {0.1f, 0.1f, 0.2f};
        }

        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.active = true;
        BodyDef.angularDamping = 1.0f;
        BodyDef.linearDamping = 1.0f;
        BodyDef.position.Set(OriginX_+i*0.01f, OriginY_);
        BodyDef.linearVelocity.Set(std::cos(DistAngle(Generator_))*(Velocity_+VelocityStdDev_),
                                   std::sin(DistAngle(Generator_))*(Velocity_+VelocityStdDev_));
        BodyDef.angularVelocity = 0.5f*DistAngle(Generator_);
        _Reg.ctx<GameObjectFactory>().create(Debris, this, Type_,
                                             DrawableGroupsTypeE::WEAPON, Col, BodyDef);

        //--- Adjust physics body for non-self collisions ---//
        auto& PhysComp = _Reg.get<PhysicsComponent>(Debris);

        b2Filter Filter;
        Filter.categoryBits = 0x0002;
        Filter.maskBits = 0xFFF9;
        PhysComp.Body_->GetFixtureList()->SetFilterData(Filter);

        //--- Adjust visuals for varying size ---//
        auto& VisComp = _Reg.get<VisualsComponent>(Debris);

        auto Size = DistScale(Generator_);
        auto Asymmetry = 0.5f;
        if (Type_ == GameObjectTypeE::DEBRIS_SUBMARINE)
        {
            Size *= 0.5f;
            Asymmetry = 2.0f;
        }
        VisComp.Visuals_->setScaling({Size, Size+Asymmetry*DistScale(Generator_)});
    }
    Counter_ += Number_;
}
