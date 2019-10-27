#include "sub.h"

#include "global_factories.h"
#include "projectile_drawable.h"

void Sub::fire(float GunPos)
{
    assert(Body_ != nullptr);
    
    Projectile* Bullet = GlobalFactories::Projectiles.create();
    
    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.active = true;
    BodyDef.position.Set(this->Body_->GetPosition().x +
                         this->Body_->GetWorldVector({GunPos, 8.0f}).x,
                         this->Body_->GetPosition().y + 
                         this->Body_->GetWorldVector({GunPos, 8.0f}).y);
    BodyDef.angle = this->Body_->GetAngle();
    BodyDef.angularDamping = 10.0f;
    BodyDef.linearDamping = 2.0f;
    Bullet->setGeometry(ResourceStorage::Global.getGeoProjectile());
    Bullet->setMesh(ResourceStorage::Global.getMeshProjectile());
    Bullet->setShader(Shader_);
    Bullet->init(World_, Scene_, BodyDef, DrawableGrp_);
    Bullet->getBody()->ApplyLinearImpulse(Bullet->getBody()->GetWorldVector({0.0f,1.0e2f}),
                                          Bullet->getBody()->GetWorldPoint({0.0f, 0.0f}), true);
    this->Body_->ApplyLinearImpulse(this->Body_->GetWorldVector({0.0f,-1.0f}),
                                    this->Body_->GetWorldPoint({GunPos, 8.0f}), true);
}

void Sub::update(SceneGraph::DrawableGroup2D& Drawables)
{
    assert(Body_ != nullptr);
    
    this->Body_->ApplyForce(this->Body_->GetWorldVector({Rudder_, 0.0f}),
                            this->Body_->GetWorldPoint({0.0f, 1.0f}), true);
    this->Body_->ApplyForce(this->Body_->GetWorldVector({0.0f, Throttle_}),
                            this->Body_->GetWorldPoint({0.0f, 0.0f}), true);
}
