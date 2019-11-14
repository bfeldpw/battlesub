#include "submarine.h"

#include "global_factories.h"
#include "global_resources.h"
#include "projectile_drawable.h"

void Submarine::fire(float GunPos)
{
    assert(Hull.getBody() != nullptr);
    
    Projectile* Bullet = GlobalFactories::Projectiles.create();
    
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
    Bullet->setShapes(GlobalResources::Get.getShapes(GameObjectTypeE::PROJECTILE))
           .setMeshes(GlobalResources::Get.getMeshes(GameObjectTypeE::PROJECTILE))
           .setShader(GlobalResources::Get.getShader());
    Bullet->init(GlobalResources::Get.getWorld(), GlobalResources::Get.getScene(), BodyDef, GlobalResources::Get.getDrawables(DrawableGroupsTypeE::WEAPON));
    Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                          Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
    Hull.getBody()->ApplyLinearImpulse(Hull.getBody()->GetWorldVector({0.0f,-1.0f}),
                                       Hull.getBody()->GetWorldPoint({GunPos, 8.0f}), true);
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
}
