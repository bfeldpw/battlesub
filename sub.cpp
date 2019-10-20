#include "sub.h"

#include "projectile_factory.h"

void Sub::fire(GL::Mesh& Mesh, Shaders::Flat2D& Shader, SceneGraph::DrawableGroup2D& Drawables,
               float GunPos)
{
    Projectile* Bullet = GlobalProjectileFactory.create();
    Bullet->create(World_, Scene_);
    
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(this->Body_->GetPosition().x +
                            this->Body_->GetWorldVector({GunPos, 8.01f}).x,
                            this->Body_->GetPosition().y + 
                            this->Body_->GetWorldVector({GunPos, 8.01f}).y);
    BodyDef.angle = this->Body_->GetAngle();
    BodyDef.angularDamping = 10.0f;
    BodyDef.linearDamping = 2.0f;
//             BodyDef.linearVelocity = 1000.0 * this->Body_->GetWorldVector({0.0f, 1.0f});
    Bullet->init(BodyDef);
    Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                            Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
    this->Body_->ApplyLinearImpulse(this->Body_->GetWorldVector({0.0f,-1.0f}),
                                    this->Body_->GetWorldPoint({GunPos, 8.0f}), true);
    
//         Projectiles_.push_back(Bullet);
//         ProjectileDrawables_.push_back(
    new ProjectileDrawable(Bullet->getVisuals(), Mesh, Shader, 0xdf0000_rgbf, Drawables);
}
