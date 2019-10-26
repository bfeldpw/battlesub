#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "game_object.h"
#include "projectile_drawable.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Projectile : public GameObject
{
    public:
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            Body_->SetBullet(true);
            
            std::vector<b2Vec2> Verts;
            Verts.push_back({-0.02f, 0.0f});
            Verts.push_back({ 0.02f, 0.0f});
            Verts.push_back({ 0.02f, 0.1f});
            Verts.push_back({ 0.0f,  0.15f});
            Verts.push_back({-0.02f, 0.1f});
            
            b2PolygonShape Shape;
            Shape.Set(Verts.data(), Verts.size());
            
            b2FixtureDef fixture;
            fixture.density = 10.0f;
            fixture.friction = 0.8f;
            fixture.shape = &Shape;
            Body_->CreateFixture(&fixture);
            
            new ProjectileDrawable(*Visuals_, Mesh_, Shader_, 0xdf0000_rgbf, *DrawableGrp_);
        }
        
        void update()
        {
            // Body starts sinking if too slow
            if (Body_->GetLinearVelocity().Length() < 0.01)
            {
                this->sink();
            }
        }
        
};

#endif // PROJECTILE_H
