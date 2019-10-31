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
            
            assert(Visuals_ != nullptr);
            assert(Meshes_ != nullptr);
            assert(Shader_ != nullptr);
            
            Body_->SetBullet(true);

            for (auto Shape : *Shapes_)
            {
                b2PolygonShape Shp;
                Shp.Set(Shape.data(), Shape.size());
            
                b2FixtureDef fixture;
                fixture.density = 10.0f;
                fixture.friction = 0.8f;
                fixture.shape = &Shp;
                Body_->CreateFixture(&fixture);
            }
            
            for (auto i=0; i<Meshes_->size(); ++i)
            {
                new ProjectileDrawable(Visuals_, &((*Meshes_)[i]), Shader_, 0xdf0000_rgbf, DrawableGrp_);
            }
        }
        
        void update()
        {
            // Body starts sinking if too slow
            if (Body_->GetLinearVelocity().Length() < 0.01f)
            {
                this->sink();
            }
        }
        
};

#endif // PROJECTILE_H
