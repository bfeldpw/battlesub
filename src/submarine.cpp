#include "submarine.h"

#include "fluid_probes_component.hpp"
#include "fluid_source_component.hpp"

void Submarine::create(entt::registry& _Reg, const float PosX, const float PosY, const float Angle)
{
    {
        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.active = true;
        BodyDef.position.Set(PosX, PosY);
        BodyDef.angle = Angle;
        BodyDef.angularDamping = 0.0f;
        BodyDef.linearDamping = 0.0f;

        Hull = _Reg.create();
        _Reg.ctx<GameObjectFactory>().create(Hull, this, GameObjectTypeE::SUBMARINE_HULL, DrawableGroupsTypeE::DEFAULT,
                                             {0.1f, 0.1f, 0.2f, 1.0f}, BodyDef);
        DBLK(
            _Reg.ctx<MessageHandler>().reportDebug("created submarine hull");
            _Reg.ctx<MessageHandler>().reportDebug("  - mass: "+std::to_string(_Reg.get<PhysicsComponent>(Hull).Body_->GetMass()));
        )

        auto& FldProbesComp = _Reg.emplace<FluidProbesComponent<8>>(Hull);
        FldProbesComp.Mass_ = 300.0f;
        FldProbesComp.N_=8;
        FldProbesComp.ProbeX_[0] = -1.7f;
        FldProbesComp.ProbeY_[0] = -7.2f;
        FldProbesComp.ProbeX_[1] = 1.7f;
        FldProbesComp.ProbeY_[1] = -7.2f;
        FldProbesComp.ProbeX_[2] = 2.1f;
        FldProbesComp.ProbeY_[2] = 0.5f;
        FldProbesComp.ProbeX_[3] = 1.7f;
        FldProbesComp.ProbeY_[3] = 8.2f;
        FldProbesComp.ProbeX_[4] = 0.7f;
        FldProbesComp.ProbeY_[4] = 9.2f;
        FldProbesComp.ProbeX_[5] = -0.7f;
        FldProbesComp.ProbeY_[5] = 9.2f;
        FldProbesComp.ProbeX_[6] = -1.7f;
        FldProbesComp.ProbeY_[6] = 8.2f;
        FldProbesComp.ProbeX_[7] = -2.1f;
        FldProbesComp.ProbeY_[7] = 0.5f;
        FldProbesComp.ProbeNormX_[0] = -0.7071f;
        FldProbesComp.ProbeNormY_[0] = -0.7071f;
        FldProbesComp.ProbeNormX_[1] =  0.7071f;
        FldProbesComp.ProbeNormY_[1] = -0.7071f;
        FldProbesComp.ProbeNormX_[2] =  1.0f;
        FldProbesComp.ProbeNormY_[2] =  0.0f;
        FldProbesComp.ProbeNormX_[3] = 0.7071f;
        FldProbesComp.ProbeNormY_[3] = 0.7071f;
        FldProbesComp.ProbeNormX_[4] = 0.0f;
        FldProbesComp.ProbeNormY_[4] = 1.0f;
        FldProbesComp.ProbeNormX_[5] = 0.0f;
        FldProbesComp.ProbeNormY_[5] = 1.0f;
        FldProbesComp.ProbeNormX_[6] = -0.7071f;
        FldProbesComp.ProbeNormY_[6] =  0.7071f;
        FldProbesComp.ProbeNormX_[7] = -1.0f;
        FldProbesComp.ProbeNormY_[7] = 0.0f;

        b2BodyDef BodyDefRudder;
        BodyDefRudder.type = b2_dynamicBody;
        BodyDefRudder.active = true;
        BodyDefRudder.position.Set(PosX+8.0f*std::sin(Angle), PosY-8.0f*std::cos(Angle));
        BodyDefRudder.angle = Angle;
        // BodyDefRudder.angularDamping = 0.8f;
        // BodyDefRudder.linearDamping = 0.2f;
        BodyDefRudder.angularDamping = 0.0f;
        BodyDefRudder.linearDamping = 0.0f;

        Rudder = _Reg.create();
        _Reg.ctx<GameObjectFactory>().create(Rudder, this, GameObjectTypeE::SUBMARINE_RUDDER, DrawableGroupsTypeE::DEFAULT,
                                             {0.1f, 0.1f, 0.2f, 1.0f}, BodyDefRudder);
        DBLK(
            _Reg.ctx<MessageHandler>().reportDebug("created submarine rudder");
            _Reg.ctx<MessageHandler>().reportDebug("  - mass: "+std::to_string(_Reg.get<PhysicsComponent>(Rudder).Body_->GetMass()));
        )
        auto& FluidProbesComponentRudder = _Reg.emplace<FluidProbesComponent<8>>(Rudder);
        FluidProbesComponentRudder.Mass_ = 50.0f;
        FluidProbesComponentRudder.N_=2;
        FluidProbesComponentRudder.ProbeX_[0] = -0.1f;
        FluidProbesComponentRudder.ProbeY_[0] = -1.0f;
        FluidProbesComponentRudder.ProbeX_[1] =  0.1f;
        FluidProbesComponentRudder.ProbeY_[1] = -1.0f;
        FluidProbesComponentRudder.ProbeNormX_[0] = -1.0f;
        FluidProbesComponentRudder.ProbeNormY_[0] =  0.0f;
        FluidProbesComponentRudder.ProbeNormX_[1] =  1.0f;
        FluidProbesComponentRudder.ProbeNormY_[1] =  0.0f;


        // auto Joint = _Reg.create();
        {
            // auto Parent = _Reg.emplace<ParentComponent>(Joint);
            // auto Physics = _Reg.emplace<PhysicsComponent>(Joint);
            b2RevoluteJointDef jointDef;
            jointDef.lowerAngle = -0.25f * b2_pi; // -45 degrees
            jointDef.upperAngle =  0.25f * b2_pi; // 45 degrees
            jointDef.enableLimit = true;
            jointDef.bodyA = _Reg.get<PhysicsComponent>(Hull).Body_;
            jointDef.localAnchorA = {0.0f, -6.0f};
            jointDef.bodyB = _Reg.get<PhysicsComponent>(Rudder).Body_;
            jointDef.localAnchorB = {0.0f,  1.0f};
            jointDef.maxMotorTorque = 100000.0f;
            jointDef.motorSpeed = 0.0f;
            jointDef.enableMotor = true;
            jointDef.collideConnected = false;
            RudderJoint = static_cast<b2RevoluteJoint*>(GlobalResources::Get.getWorld()->CreateJoint(&jointDef));
        }
    }
}

void Submarine::fire(entt::registry& _Reg)
{
    b2Body* const Body = _Reg.get<PhysicsComponent>(Hull).Body_;
    assert(Body != nullptr);

    float GunPos = 1.0f;

    GunSide == GUN_LEFT ? GunPos = -1.0f : GunPos = 1.0f; 

    if (GunRateTimer.split() > 0.1)
    {
        auto Bullet = _Reg.create();

        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.active = true;
        BodyDef.position.Set(Body->GetPosition().x +
                             Body->GetWorldVector({GunPos, 8.75f}).x,
                             Body->GetPosition().y +
                             Body->GetWorldVector({GunPos, 8.75f}).y);
        BodyDef.angle = Body->GetAngle();
        BodyDef.angularDamping = 5.0f;
        BodyDef.linearDamping = 0.0f;
        BodyDef.bullet = true;
        _Reg.ctx<GameObjectFactory>().create(Bullet, this, GameObjectTypeE::PROJECTILE, DrawableGroupsTypeE::WEAPON,
                                             {0.7f, 0.5f, 0.3f, 1.0f}, BodyDef);

        auto& FldProbesComp = _Reg.emplace<FluidProbeComponent>(Bullet);
        FldProbesComp.ProbeX_ = 0.0f;
        FldProbesComp.ProbeY_ = 0.0f;

        auto& FldSrcComp = _Reg.emplace<FluidSourceComponent>(Bullet);
        FldSrcComp.VelocityBackProjection_ = 1.0f/30.0f;
        FldSrcComp.DensityWeight_ = 10.0f;
        FldSrcComp.VelocityWeight_ = 0.1f;


        b2Filter Filter;
        Filter.categoryBits = 0x0004;
    //     Filter.maskBits = 0xFFFD;
        b2Body* const BulletBody = _Reg.get<PhysicsComponent>(Bullet).Body_;
        BulletBody->GetFixtureList()->SetFilterData(Filter);
        BulletBody->ApplyLinearImpulse(BulletBody->GetWorldVector({0.0f,1.0e2f}),
                                       BulletBody->GetWorldPoint({0.0f, 0.0f}), true);
        Body->ApplyLinearImpulse(Body->GetWorldVector({0.0f,-1.0f}),
                                 Body->GetWorldPoint({GunPos, 8.0f}), true);

        GunSide ^= true; // Just toggle
        GunRateTimer.restart();
    }
}

void Submarine::update(entt::registry& _Reg)
{
    b2Body* const HullBody = _Reg.get<PhysicsComponent>(Hull).Body_;
    assert(HullBody != nullptr);

    HullBody->ApplyForceToCenter(HullBody->GetWorldVector({0.0f, Throttle_}), true);
}
