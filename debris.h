#ifndef DEBRIS_H
#define DEBRIS_H

#include "game_object.h"
#include "projectile_drawable.h"

using namespace Magnum;
using namespace Magnum::Math::Literals;

class Debris : public GameObject
{
    public:
        
        Debris() {Type = GameObjectTypeE::DEBRIS;}
        
        void init(b2World* World, Scene2D* Scene, const b2BodyDef BodyDef, SceneGraph::DrawableGroup2D* DGrp)
        {
            GameObject::init(World, Scene, BodyDef, DGrp);
            
            for (auto i=0u; i<Meshes_->size(); ++i)
            {
                new ProjectileDrawable(Visuals_, &((*Meshes_)[i]), Shader_, 0x444455_rgbf, DrawableGrp_);
            }
        }
        
        void update()
        {
            assert(Body_ != nullptr);
            
            // Body starts sinking if too slow
            if (Body_->GetLinearVelocity().Length() < 0.01f)
            {
                this->sink();
            }
        }
        
};

#endif // DEBRIS_H
