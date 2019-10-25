#include "sub.h"

#include "projectile_drawable.h"
#include "projectile_factory.h"

void Sub::fire(Shaders::Flat2D& Shader, SceneGraph::DrawableGroup2D& Drawables,
               float GunPos, ResourceStorage* Resources)
{
    assert(Body_ != nullptr);
    assert(Resources != nullptr);
    
    Projectile* Bullet = GlobalProjectileFactory.create();
    
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
    Bullet->init(World_, Scene_, BodyDef);
    Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                          Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
    this->Body_->ApplyLinearImpulse(this->Body_->GetWorldVector({0.0f,-1.0f}),
                                    this->Body_->GetWorldPoint({GunPos, 8.0f}), true);
    
    new ProjectileDrawable(Bullet->getVisuals(), Resources->getMeshProjectile(), Shader, 0xdf0000_rgbf, Drawables);
}

void Sub::update(SceneGraph::DrawableGroup2D& Drawables)
{
    assert(Body_ != nullptr);
    
    this->Body_->ApplyForce(this->Body_->GetWorldVector({Rudder_, 0.0f}),
                            this->Body_->GetWorldPoint({0.0f, 1.0f}), true);
    this->Body_->ApplyForce(this->Body_->GetWorldVector({0.0f, Throttle_}),
                            this->Body_->GetWorldPoint({0.0f, 0.0f}), true);
    for (auto Bullet : GlobalProjectileFactory.getEntities())
    {
        Bullet.second->update();
        if (Bullet.second->isSunk())
        {
            GlobalProjectileFactory.destroy(Bullet.second);
            break;
        }
    }
}
