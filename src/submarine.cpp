#include "submarine.h"

#include "drawable_generic.h"
#include "global_factories.h"
#include "global_resources.h"

void Submarine::create(const float PosX, const float PosY, const float Angle)
{
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(PosX, PosY);
    BodyDef.angle = Angle;
    BodyDef.angularDamping = 0.8f;
    BodyDef.linearDamping = 0.2f;
    Hull.setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
        .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::SUBMARINE_HULL))
        .setScene(GlobalResources::Get.getScene())
        .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::SUBMARINE_HULL))
        .setShader(GlobalResources::Get.getShader())
        .setWorld(GlobalResources::Get.getWorld())
        .init(GameObjectTypeE::SUBMARINE_HULL, BodyDef);
    b2BodyDef BodyDefRudder;
    BodyDefRudder.type = b2_dynamicBody;
    BodyDefRudder.active = true;
    BodyDefRudder.position.Set(PosX+8.0f*std::sin(Angle), PosY-8.0f*std::cos(Angle));
    BodyDefRudder.angle = Angle;
    BodyDefRudder.angularDamping = 0.8f;
    BodyDefRudder.linearDamping = 0.2f;
    Rudder.setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::DEFAULT))
          .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::SUBMARINE_RUDDER))
          .setScene(GlobalResources::Get.getScene())
          .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::SUBMARINE_RUDDER))
          .setShader(GlobalResources::Get.getShader())
          .setWorld(GlobalResources::Get.getWorld())
          .init(GameObjectTypeE::SUBMARINE_RUDDER, BodyDefRudder);

    b2RevoluteJointDef jointDef;
    jointDef.lowerAngle = -0.25f * b2_pi; // -45 degrees
    jointDef.upperAngle =  0.25f * b2_pi; // 45 degrees
    jointDef.enableLimit = true;
    jointDef.bodyA = Hull.getBody();
    jointDef.localAnchorA = {0.0f, -6.0f};
    jointDef.bodyB = Rudder.getBody();
    jointDef.localAnchorB = {0.0f,  1.0f};
    jointDef.maxMotorTorque = 10000.0f;
    jointDef.motorSpeed = 0.0f;
    jointDef.enableMotor = true;
    jointDef.collideConnected = false;
    RudderJoint = static_cast<b2RevoluteJoint*>(GlobalResources::Get.getWorld()->CreateJoint(&jointDef));
}

void Submarine::fire()
{
    assert(Hull.getBody() != nullptr);

    float GunPos = 1.0f;

    GunSide == GUN_LEFT ? GunPos = -1.0f : GunPos = 1.0f; 

    if (GunRateTimer.split() > 0.1)
    {
        GameObject* Bullet = GlobalFactories::Projectiles.create();
    
        b2BodyDef BodyDef;
        BodyDef.type = b2_dynamicBody;
        BodyDef.active = true;
        BodyDef.position.Set(Hull.getBody()->GetPosition().x +
                            Hull.getBody()->GetWorldVector({GunPos, 8.0f}).x,
                            Hull.getBody()->GetPosition().y +
                            Hull.getBody()->GetWorldVector({GunPos, 8.0f}).y);
        BodyDef.angle = Hull.getBody()->GetAngle();
        BodyDef.angularDamping = 5.0f;
        BodyDef.linearDamping = 1.0f;
        BodyDef.bullet = true;
        Bullet->setColor({0.7f, 0.5f, 0.3f, 1.0f})
            .setDrawableGroup(GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON))
            .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::PROJECTILE))
            .setScene(GlobalResources::Get.getScene())
            .setShader(GlobalResources::Get.getShader())
            .setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::PROJECTILE))
            .setWorld(GlobalResources::Get.getWorld())
            .init(GameObjectTypeE::PROJECTILE, BodyDef);
        b2Filter Filter;
        Filter.categoryBits = 0x0004;
    //     Filter.maskBits = 0xFFFD;
        Bullet->getBody()->GetFixtureList()->SetFilterData(Filter);
        Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                            Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
        Hull.getBody()->ApplyLinearImpulse(Hull.getBody()->GetWorldVector({0.0f,-1.0f}),
                                           Hull.getBody()->GetWorldPoint({GunPos, 8.0f}), true);
        GunSide^=true; // Just toggle
        GunRateTimer.restart();
    }
}

void Submarine::update()
{
    assert(Hull.getBody() != nullptr);
    
    constexpr float WATER_FORCE_FACTOR = 3000.0f;
    constexpr float RUDDER_LENGTH = 2.0f;

    float WaterResistanceOnRudder = RUDDER_LENGTH * Rudder.getBody()->GetLocalVector(Hull.getBody()->GetLinearVelocity()).x * WATER_FORCE_FACTOR;
    
    Hull.getBody()->ApplyForceToCenter(Hull.getBody()->GetWorldVector({0.0f, Throttle_}), true);
    Hull.getBody()->ApplyForce(Rudder.getBody()->GetWorldVector({-WaterResistanceOnRudder, 0.0f }),
                               Hull.getBody()->GetWorldPoint({0.0f, -6.0f}), true);

    RudderJoint->SetMotorSpeed(0.0f);
}
