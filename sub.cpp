#include "sub.h"

#include "global_factories.h"
#include "global_resources.h"
#include "projectile_drawable.h"

void Sub::fire(float GunPos)
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
    BodyDef.angularDamping = 10.0f;
    BodyDef.linearDamping = 2.0f;
    Bullet->setShapes(GlobalResources::Get.getShapes(GameObjectType::PROJECTILE))
           .setMeshes(GlobalResources::Get.getMeshes(GameObjectType::PROJECTILE))
           .setShader(GlobalResources::Get.getShader());
    Bullet->init(Hull.getWorld(), GlobalResources::Get.getScene(), BodyDef, GlobalResources::Get.getDrawables());
    Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                          Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
    Hull.getBody()->ApplyLinearImpulse(Hull.getBody()->GetWorldVector({0.0f,-1.0f}),
                                       Hull.getBody()->GetWorldPoint({GunPos, 8.0f}), true);
}

void Sub::update()
{
    assert(Hull.getBody() != nullptr);
    
    Hull.getBody()->ApplyForce(Hull.getBody()->GetWorldVector({Rudder_, 0.0f}),
                               Hull.getBody()->GetWorldPoint({0.0f, 1.0f}), true);
    Hull.getBody()->ApplyForce(Hull.getBody()->GetWorldVector({0.0f, Throttle_}),
                               Hull.getBody()->GetWorldPoint({0.0f, 0.0f}), true);
}
